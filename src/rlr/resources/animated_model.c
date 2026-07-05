#include <stdlib.h>
#include <string.h>
#include <cgltf.h>
#include "external/rlr_stb_ds.h"
#include "internal/impl.h"
#include "rlr/resources/texture.h"
#include "rlr/resources/uniform.h"
#include "rlr/math/quat.h"
#include "rlr/math/matrix.h"
#include "rlr/error.h"
#include "model_shared.h"
#include "animated_model.h"

#include <stdio.h>

typedef struct animation_frame_joint_t {
    rlr_vec3_t t;
    rlr_quat_t r;
    rlr_vec3_t s;
} animation_frame_joint_t;

static float animation_duration(cgltf_animation* animation) {
    float duration = 0.0;

    for(size_t i = 0; i < animation->channels_count; i++) {
        cgltf_animation_channel* channel = &animation->channels[i];
        cgltf_accessor* input = channel->sampler->input;

        float last_time;
        cgltf_accessor_read_float(input, input->count - 1, &last_time, 1);

        if (last_time > duration) {
            duration = last_time;
        }
    }

    return duration;
}

static void reset_frames(animation_frame_joint_t* poses, cgltf_node* nodes, uint32_t count) {
    for(uint32_t i = 0; i < count; i++) {
        cgltf_node* node = &nodes[i];
        animation_frame_joint_t* final = &poses[i];

        if(node->has_matrix) {
            printf("has matrix\n");
        }

        if(node->has_translation) {
            final->t = rlr_vec3(node->translation[0], node->translation[1], node->translation[2]);
        } else {
            final->t = rlr_vec3_zero;
        }

        if(node->has_rotation) {
            final->r = rlr_quat(node->rotation[3], node->rotation[0], node->rotation[1], node->rotation[2]);
        } else {
            final->r = rlr_quat_ident;
        }

        if(node->has_scale) {
            final->s = rlr_vec3(node->scale[0], node->scale[1], node->scale[2]);
        } else {
            final->s = rlr_vec3(1.0, 1.0, 1.0);
        }
    }
}

static void find_keyframe_interval(cgltf_accessor* times, float t, uint32_t* out_key0, uint32_t* out_key1, float* out_alpha) {
    uint32_t count = (uint32_t)times->count;

    if(count == 1) {
        *out_key0 = 0;
        *out_key1 = 0;
        *out_alpha = 0.0f;
        return;
    }

    float first_time = 0.0f;
    cgltf_accessor_read_float(times, 0, &first_time, 1);

    if(t <= first_time) {
        *out_key0 = 0;
        *out_key1 = 0;
        *out_alpha = 0.0f;
        return;
    }

    float last_time = 0.0f;
    cgltf_accessor_read_float(times, count - 1, &last_time, 1);

    if(t >= last_time) {
        *out_key0 = count - 1;
        *out_key1 = count - 1;
        *out_alpha = 0.0f;
        return;
    }

    for(uint32_t i = 0; i < count - 1; i++) {
        float t0 = 0.0f;
        float t1 = 0.0f;

        cgltf_accessor_read_float(times, i, &t0, 1);
        cgltf_accessor_read_float(times, i + 1, &t1, 1);

        if (t >= t0 && t <= t1) {
            *out_key0 = i;
            *out_key1 = i + 1;
            *out_alpha = (t - t0) / (t1 - t0);
            return;
        }
    }

    *out_key0 = count - 1;
    *out_key1 = count - 1;
    *out_alpha = 0.0f;
}

static rlr_vec3_t sample_vec3(cgltf_animation_sampler* sampler, float t) {
    uint32_t k0 = 0;
    uint32_t k1 = 0;
    float alpha = 0.0;
    find_keyframe_interval(sampler->input, t, &k0, &k1, &alpha);

    if(sampler->interpolation == cgltf_interpolation_type_step) {
        alpha = 0.0;
    }

    float a[3];
    float b[3];
    cgltf_accessor_read_float(sampler->output, k0, a, 3);
    cgltf_accessor_read_float(sampler->output, k1, b, 3);

    return rlr_vec3_lerp(rlr_vec3(a[0], a[1], a[2]), rlr_vec3(b[0], b[1], b[2]), alpha);
}

static rlr_quat_t sample_quaternion(cgltf_animation_sampler* sampler, float t) {
    uint32_t k0 = 0;
    uint32_t k1 = 0;
    float alpha = 0.0;
    find_keyframe_interval(sampler->input, t, &k0, &k1, &alpha);

    if(sampler->interpolation == cgltf_interpolation_type_step) {
        alpha = 0.0f;
    }

    float a[4];
    float b[4];
    cgltf_accessor_read_float(sampler->output, k0, a, 4);
    cgltf_accessor_read_float(sampler->output, k1, b, 4);

    return rlr_quat_slerp(&rlr_quat(a[3], a[0], a[1], a[2]), &rlr_quat(b[3], b[0], b[1], b[2]), alpha);
}

static void build_global_poses_rec(cgltf_node* nodes, cgltf_node* node, rlr_mat4x4_t* local_poses, rlr_mat4x4_t* out_global_poses, uint32_t node_count, rlr_mat4x4_t parent_matrix) {
    uint32_t node_index = (uint32_t)(node - nodes);

    rlr_mat4x4_t current_matrix = rlr_mat4x4_mul(&parent_matrix, &local_poses[node_index]);
    out_global_poses[node_index] = current_matrix;

    for(uint32_t i = 0; i < node->children_count; i++) {
        build_global_poses_rec(nodes, node->children[i], local_poses, out_global_poses, node_count, current_matrix);
    }
}

static void build_global_poses(cgltf_node* nodes, rlr_mat4x4_t* local_poses, rlr_mat4x4_t* out_global_poses, uint32_t node_count) {
    for(uint32_t i = 0; i < node_count; i++) {
        cgltf_node* node = &nodes[i];

        if(node->parent == NULL) {
            build_global_poses_rec(nodes, node, local_poses, out_global_poses, node_count, rlr_mat4x4_ident);
        }
    }
}

static cgltf_node* find_mesh_node_for_skin(cgltf_data* data, cgltf_skin* skin) {
    for(uint32_t i = 0; i < data->nodes_count; i++) {
        cgltf_node* node = &data->nodes[i];

        if(node->mesh && node->skin == skin) {
            return node;
        }
    }

    return NULL;
}

static rlr_res_animations_t load_animations(cgltf_data* model, float fps) {
    rlr_res_animations_t final = {0};
    rlr_mat4x4_t* matrices = NULL;

    if(model->skins_count != 1) {
        printf("err: more than 1 skin detected..\n");
        return (rlr_res_animations_t){0};
    }
    cgltf_skin* skin = &model->skins[0];
    rlr_mat4x4_t* animation_matrices = NULL; // the final, baked, gpu data

    final.joint_count = model->skins[0].joints_count;
    const uint32_t joint_count = model->skins[0].joints_count;
    const uint32_t node_count = model->nodes_count;
    animation_frame_joint_t* poses = malloc(sizeof(animation_frame_joint_t) * node_count);
    rlr_mat4x4_t* local_matrices = malloc(sizeof(rlr_mat4x4_t) * node_count);
    rlr_mat4x4_t* global_matrices = malloc(sizeof(rlr_mat4x4_t) * node_count);

    //make identity pose(which is used for meshes that don't have any weights and joints)
    for(int32_t i = 0; i < model->skins[0].joints_count; i++) {
        arrpush(animation_matrices, rlr_mat4x4_ident);
    }

    cgltf_node* mesh_node = find_mesh_node_for_skin(model, skin);
    uint32_t mesh_node_index = mesh_node - model->nodes;

    for(int64_t i = 0; i < model->animations_count; i++) {
        cgltf_animation* animation = &model->animations[i];
        rlr_res_animation_meta_t anim_meta = {0};
        strncpy(anim_meta.name, animation->name, RLR_RES_ANIMATED_MODEL_ANIMATION_NAME_LENGTH - 1);
        anim_meta.name[RLR_RES_ANIMATED_MODEL_ANIMATION_NAME_LENGTH - 1] = '\0';
        anim_meta.fps = fps;
        anim_meta.duration = animation_duration(animation);
        anim_meta.pose_offset = arrlenu(animation_matrices);

        float frame_interval = 1.0 / anim_meta.fps;
        bool done = false;
        for(float t = 0.0;; t += frame_interval) {
            if(t > anim_meta.duration) {
                t = anim_meta.duration;
                done = true;
            }

            anim_meta.pose_count++;
            reset_frames(poses, model->nodes, node_count);

            for(uint32_t c = 0; c < animation->channels_count; c++) {
                cgltf_animation_channel* channel = &animation->channels[c];
                cgltf_animation_sampler* sampler = channel->sampler;
                uint64_t target_node_index = channel->target_node - model->nodes;
                switch(channel->target_path) {
                    case cgltf_animation_path_type_translation: {
                        poses[target_node_index].t = sample_vec3(sampler, t);
                        break;
                    }
                    case cgltf_animation_path_type_rotation: {
                        poses[target_node_index].r = sample_quaternion(sampler, t);
                        break;
                    }
                    case cgltf_animation_path_type_scale: {
                        poses[target_node_index].s = sample_vec3(sampler, t);
                        break;
                    }
                }
            }

            //make local matrices
            for(uint32_t l = 0; l < node_count; l++) {
                animation_frame_joint_t* pose = &poses[l];
                local_matrices[l] = rlr_mat4x4_trs(&pose->t, &pose->r, &pose->s);
            }

            build_global_poses(model->nodes, local_matrices, global_matrices, node_count);
            rlr_mat4x4_t inv_mesh_global = mesh_node != NULL ? rlr_mat4x4_inverse(&global_matrices[mesh_node_index]) : rlr_mat4x4_ident;

            for(uint32_t j = 0; j < skin->joints_count; j++) {
                rlr_mat4x4_t inverse_bind_matrix = rlr_mat4x4_ident;
                cgltf_accessor_read_float(skin->inverse_bind_matrices, j, (float*)&inverse_bind_matrix, 16);
                uint32_t node_index = skin->joints[j] - model->nodes;

                rlr_mat4x4_t joint = rlr_mat4x4_mul(&global_matrices[node_index], &inverse_bind_matrix);
                rlr_mat4x4_t final = rlr_mat4x4_mul(&inv_mesh_global, &joint);
                arrpush(animation_matrices, final);
            }

            if(done) {
                break;
            }
        }

        arrpush(final.metas, anim_meta);
    }

    final.animation_texture = rlr_backend()->create_animation_texture(animation_matrices, arrlenu(animation_matrices), RLR_RES_ANIMATED_MODEL_ANIMATION_TEXTURE_SIZE);
    printf("tex %d\n", final.animation_texture);
    printf("baked animation data info: \nmatrix count=%d\nsize=%d\ntexel count=%d\n", arrlen(animation_matrices), arrlen(animation_matrices) * sizeof(rlr_affine_mat4x3_t), arrlen(animation_matrices) * 4);
    return final;
}

rlr_res_animated_model_t* rlr_res_animated_model_load_glb(const char* glb_model_location) {
    cgltf_data* data = NULL;
    rlr_res_animated_model_t* model = malloc(sizeof(rlr_res_animated_model_t));
    rlr_animated_model_vertex_t* vertices = NULL;
    uint32_t* indices = NULL;
    if(!model) {
        rlr_error_set(RLR_ERR_NO_MEMORY);
        goto err;
    }

    model->meshes = NULL;
    model->animations = (rlr_res_animations_t) {
        .animation_texture = 0,
        .joint_count = 0,
        .metas = NULL
    };

    data = rlr_res_model_load_glb(glb_model_location);
    if(!data) {
        goto err;
    }

    //load the animations first
    model->animations = load_animations(data, RLR_RES_ANIMATED_MODEL_ANIMATION_FPS);
    if(model->animations.animation_texture == 0) {
        rlr_error_set(RLR_ERR_MODEL_COULD_NOT_LOAD_ANIMATION);
        goto err;
    }

    printf("Animation count: %d with %d joints\n", arrlen(model->animations.metas), model->animations.joint_count);
    for(int32_t i = 0; i < arrlen(model->animations.metas); i++) {
        rlr_res_animation_meta_t* meta = &model->animations.metas[i];
        printf("Animation \"%s\" is %f seconds long. It's animation matrices starts at offset %d and it has %d animation keyframes\n", meta->name, meta->duration, meta->pose_offset, meta->pose_count);
    }

    for(size_t i = 0; i < data->meshes_count; i++) {
        cgltf_mesh* mesh = &data->meshes[i];

        for(size_t p = 0; p < mesh->primitives_count; p++) {
            cgltf_primitive* primitive = &mesh->primitives[p];
            if(primitive->type != cgltf_primitive_type_triangles) {
                rlr_error_set(RLR_ERR_MODEL_PRIMITIVE_NOT_TRIANGLES);
                goto err;
            }

            arrsetlen(vertices, 0);
            arrsetlen(indices, 0);
            size_t count_position = 0;
            size_t count_normal = 0;
            size_t count_uv = 0;
            size_t count_weights = 0;
            size_t count_joints = 0;
            cgltf_accessor* accessor_position = NULL;
            cgltf_accessor* accessor_normal = NULL;
            cgltf_accessor* accessor_uv = NULL;
            cgltf_accessor* accessor_weights = NULL;
            cgltf_accessor* accessor_joints = NULL;

            for(size_t a = 0; a < primitive->attributes_count; a++) {
                cgltf_attribute* attribute = &primitive->attributes[a];
                switch(attribute->type) {
                    case cgltf_attribute_type_position: {
                        accessor_position = attribute->data;
                        count_position = attribute->data->count;
                        break;
                    }
                    case cgltf_attribute_type_normal: {
                        accessor_normal = attribute->data;
                        count_normal = attribute->data->count;
                        break;
                    }
                    case cgltf_attribute_type_texcoord: {
                        accessor_uv = attribute->data;
                        count_uv = attribute->data->count;
                        break;
                    }
                    case cgltf_attribute_type_joints: {
                        accessor_joints = attribute->data;
                        count_joints = attribute->data->count;
                        break;
                    }
                    case cgltf_attribute_type_weights: {
                        accessor_weights = attribute->data;
                        count_weights = attribute->data->count;
                        break;
                    }
                    default:
                        break;
                }
            }

            bool has_joints = accessor_joints != NULL;
            bool has_weights = accessor_weights != NULL;
            if(has_joints != has_weights) {
                rlr_error_set(RLR_ERR_ANIMATED_MODEL_NEEDS_BOTH_JOINTS_AND_WEIGHTS_OR_NONE);
                goto err;
            }

            if(!accessor_position || count_position == 0) {
                rlr_error_set(RLR_ERR_MODEL_NO_POSITION_ATTRIBUTE);
                goto err;
            }
            if(!accessor_normal || count_normal != count_position) {
                rlr_error_set(RLR_ERR_MODEL_ATTRIBUTE_COUNT_ARE_DIFFERENT);
                goto err;
            }
            if(accessor_uv && count_uv != count_position) {
                rlr_error_set(RLR_ERR_MODEL_ATTRIBUTE_COUNT_ARE_DIFFERENT);
                goto err;
            }
            if(!accessor_joints || count_joints != count_position) {
                rlr_error_set(RLR_ERR_MODEL_ATTRIBUTE_COUNT_ARE_DIFFERENT);
                goto err;
            }
            if(!accessor_weights || count_weights != count_position) {
                rlr_error_set(RLR_ERR_MODEL_ATTRIBUTE_COUNT_ARE_DIFFERENT);
                goto err;
            }

            //load vertices
            for(size_t v = 0; v < count_position; v++) {
                float pos[3];
                float norm[3] = {0};
                float uv[2] = {0};
                uint32_t joints[4] = {0};
                float weights[4] = {0};
                cgltf_accessor_read_float(accessor_position, v, pos, 3);
                if(accessor_normal) {
                    cgltf_accessor_read_float(accessor_normal, v, norm, 3);
                }
                if(accessor_uv) {
                    cgltf_accessor_read_float(accessor_uv, v, uv, 2);
                }
                if(accessor_joints) {
                    cgltf_accessor_read_uint(accessor_joints, v, joints, 4);
                }
                if(accessor_weights) {
                    cgltf_accessor_read_float(accessor_weights, v, weights, 4);
                }

                rlr_animated_model_vertex_t vertex = {
                    .pos = rlr_vec3(pos[0], pos[1], pos[2]),
                    .normal = rlr_vec3(norm[0], norm[1], norm[2]),
                    .uv = rlr_vec2(uv[0], uv[1]),
                };

                if(has_joints && has_weights) {
                    for(uint32_t jw = 0; jw < 4; jw++) {
                        if(joints[jw] > 255) {
                            goto err;
                        }
                        vertex.joints[jw] = (uint8_t)joints[jw];
    
                        float w = weights[jw];
                        if(w < 0.0) {
                            w = 0.0;
                        }
                        if(w > 1.0) {
                            w = 1.0;
                        }
                        vertex.weights[jw] = (uint8_t)roundf(w * 255.0);
                    }
                } else {
                    static const uint8_t default_joints[4] = {0, 0, 0, 0};
                    static const uint8_t default_weights[4] = {255, 0, 0, 0};
                    memcpy(vertex.joints, default_joints, sizeof(default_joints));
                    memcpy(vertex.weights, default_weights, sizeof(default_weights));
                }

                arrpush(vertices, vertex);
            }

            //load indices
            for(size_t ind = 0; ind < primitive->indices->count; ind++) {
                uint32_t index = cgltf_accessor_read_index(primitive->indices, ind);
                arrpush(indices, index);
            }

            printf("mesh %s: vertices=%u indices=%u triangles=%u\n",
                mesh->name,
                count_position,
                primitive->indices->count,
                primitive->indices->count / 3);

            //create mesh
            arrpush(model->meshes, (rlr_res_animated_mesh_t){0});
            rlr_res_animated_mesh_t* mesh = &arrlast(model->meshes);
            mesh->is_skinned = has_joints && has_weights;
            mesh->material_ubo = NULL;
            mesh->vbo = rlr_backend()->create_buffer();
            mesh->ebo = rlr_backend()->create_buffer();
            mesh->index_count = arrlenu(indices);
            if(mesh->vbo == 0 || mesh->ebo == 0) {
                rlr_error_set(RLR_ERR_BACKEND_NULL_HANDLE);
                goto err;
            }

            //fill buffers
            rlr_backend()->bind_buffer(mesh->vbo, RLR_BACKEND_BUFFER_ARRAY);
            rlr_backend()->update_buffer(RLR_BACKEND_BUFFER_ARRAY, sizeof(rlr_animated_model_vertex_t) * arrlenu(vertices), vertices, RLR_BACKEND_BUFFER_USAGE_STATIC);
            rlr_backend()->bind_buffer(mesh->ebo, RLR_BACKEND_BUFFER_ELEMENT_ARRAY);
            rlr_backend()->update_buffer(RLR_BACKEND_BUFFER_ELEMENT_ARRAY, sizeof(uint32_t) * arrlenu(indices), indices, RLR_BACKEND_BUFFER_USAGE_STATIC);

            //material
            if(!rlr_res_model_parse_cgltf_material(primitive->material, &mesh->material_ubo, &mesh->texture_base)) {
                goto err;
            }
        }
    }

    cgltf_free(data);
    return model;
err:
    cgltf_free(data);
    rlr_res_animated_model_free(model);
    return NULL;
}

void rlr_res_animated_model_free(rlr_res_animated_model_t* model) {
    if(!model) {
        return;
    }
    rlr_backend()->free_texture(model->animations.animation_texture);
    arrfree(model->animations.metas);

    for(int32_t i = 0; i < arrlen(model->meshes); i++) {
        rlr_res_animated_mesh_t* mesh = &model->meshes[i];
        rlr_backend()->free_buffer(mesh->ebo);
        rlr_backend()->free_buffer(mesh->vbo);
        rlr_res_uniform_free(mesh->material_ubo);
        rlr_res_texture_free(mesh->texture_base);
    }
    arrfree(model->meshes);
    free(model);
}