#include <stdlib.h>
#include "external/rlr_stb_ds.h"
#include "rlr/objects/static_model.h"
#include "rlr/resources/static_model.h"
#include "rlr/resources/uniform.h"
#include "rlr/resources/texture.h"
#include "rlr/resources/shader.h"
#include "internal/impl.h"
#include "model.h"

#define RLR_PIPELINE_MODEL    (&rlr()->pipeline_model)

const char simple_fragment[] = RLR_SHADER_INLINE(
    in vec2 frag_uv;
    out vec4 out_color;
    uniform sampler2D tex;

    void main() {
        out_color = texture(tex, frag_uv);
    }
);

const char simple_vertex[] = RLR_SHADER_INLINE(
    layout(location = 0) in vec3 pos;
    layout(location = 1) in vec3 normal;
    layout(location = 2) in vec2 uv;
    layout(location = 3) in vec4 instance_matrix_0;
    layout(location = 4) in vec4 instance_matrix_1;
    layout(location = 5) in vec4 instance_matrix_2;

    layout(std140) uniform ubo_model {
        mat4 vp;
        vec3 camera_pos;
    } model;

    out vec2 frag_uv;

    void main() {
        mat4 instance_matrix = mat4(
            vec4(instance_matrix_0.xyz, 0.0),
            vec4(instance_matrix_1.xyz, 0.0),
            vec4(instance_matrix_2.xyz, 0.0),
            vec4(instance_matrix_0.w, instance_matrix_1.w, instance_matrix_2.w, 1.0)
        );
        frag_uv = uv;
        gl_Position = model.vp * instance_matrix * vec4(pos, 1.0);
    }
);

const char model_fragment[] = RLR_SHADER_INLINE(
    in vec2 frag_uv;
    in vec3 frag_normal;
    in vec3 frag_vert_pos;
    out vec4 out_color;

    uniform sampler2D tex;
    uniform samplerCube cube_map;

    layout(std140) uniform ubo_model {
        mat4 vp;
        vec3 camera_pos;
    } model;

    layout(std140) uniform ubo_material {
        vec4 color;
        float shininess;
        float specular_strength;
        float reflectiveness;
    } material;

    struct PointLight {
        vec3 position;
        vec3 color;
    };

    const int LIGHT_COUNT = 8;
    const PointLight point_lights[LIGHT_COUNT] = PointLight[](
        PointLight(vec3(1.0, 1.0, 1.0), vec3(1.0, 0.0, 0.0)),
        PointLight(vec3(2.0, 1.0, 1.0), vec3(1.0, 1.0, 0.0)),
        PointLight(vec3(3.0, 1.0, 1.0), vec3(1.0, 0.0, 1.0)),
        PointLight(vec3(4.0, 1.0, 1.0), vec3(1.0, 1.0, 1.0)),
        PointLight(vec3(5.0, 1.0, 1.0), vec3(0.0, 1.0, 1.0)),
        PointLight(vec3(-1.0, 1.0, 1.0), vec3(0.0, 0.0, 1.0)),
        PointLight(vec3(-2.0, 1.0, 1.0), vec3(0.0, 1.0, 0.0)),
        PointLight(vec3(-3.0, 1.0, 1.0), vec3(0.0, 0.0, 0.0))
    );

    const vec3 light_dir = normalize(vec3(-0.4, -1.0, -0.3));
    const vec3 ambient_color = vec3(0.2, 0.5, 0.4);

    vec3 point_light(vec3 light_pos, vec3 light_col, vec3 normal, vec3 V, vec3 diffuse_col) {
        vec3 to_light = light_pos - frag_vert_pos;

        float distance = length(to_light);
        vec3 L = to_light / distance;
        vec3 H = normalize(L + V);

        float attenuation = 1.0 / (1.0 + 0.09 * distance + 0.032 * distance * distance);

        float diff = max(dot(normal, L), 0.0);

        vec3 diffuse =
            diffuse_col *
            light_col *
            diff *
            attenuation;

        float spec = 0.0;
        if(diff > 0.0) {
            spec = pow(
                max(dot(normal, H), 0.0),
                material.shininess
            );
        }

        vec3 specular = light_col * spec * material.specular_strength * attenuation;
        return diffuse + specular;
    }

    void main() {
        vec4 tex_color = texture(tex, frag_uv);
        vec3 diffuse_color = tex_color.rgb * material.color.rgb;

        vec3 N = normalize(frag_normal);
        vec3 L = normalize(-light_dir);
        vec3 V = normalize(model.camera_pos - frag_vert_pos);
        vec3 H = normalize(L + V);

        vec3 R = normalize(reflect(-V, N));
        vec3 reflected_color = texture(cube_map, R).rgb;

        vec3 spec_color = vec3(1.0);

        float ambient_strength = 0.35;
        vec3 ambient = diffuse_color * ambient_color * ambient_strength;

        float diff = max(dot(N, L), 0.0);
        vec3 diffuse = diffuse_color * diff;

        float spec = pow(max(dot(N, H), 0.0), material.shininess);
        vec3 specular = spec_color * spec * material.specular_strength;

        vec3 lit_color = ambient + diffuse + specular;

        // for(int i = 0; i < LIGHT_COUNT; i++) {
        //     lit_color += point_light(point_lights[i].position, point_lights[i].color, N, V, diffuse_color);
        // }

        vec3 final_rgb = mix(lit_color, reflected_color, material.reflectiveness);
        out_color = vec4(final_rgb, tex_color.a * material.color.a);
    }
);

const char static_model_vertex[] = RLR_SHADER_INLINE(
    layout(location = 0) in vec3 pos;
    layout(location = 1) in vec3 normal;
    layout(location = 2) in vec2 uv;
    layout(location = 3) in vec3 instance_mat_col_0;
    layout(location = 4) in vec3 instance_mat_col_1;
    layout(location = 5) in vec3 instance_mat_col_2;
    layout(location = 6) in vec3 instance_mat_col_3;

    layout(std140) uniform ubo_model {
        mat4 vp;
        vec3 camera_pos;
    } model;

    out vec2 frag_uv;
    out vec3 frag_normal;
    out vec3 frag_vert_pos;

    void main() {
        mat4 instance_matrix = mat4(
            vec4(instance_mat_col_0, 0.0),
            vec4(instance_mat_col_1, 0.0),
            vec4(instance_mat_col_2, 0.0),
            vec4(instance_mat_col_3, 1.0)
        );

        vec4 world_pos = instance_matrix * vec4(pos, 1.0);
        frag_vert_pos = world_pos.xyz;
        frag_normal = mat3(transpose(inverse(instance_matrix))) * normal;
        frag_uv = uv;
        gl_Position = model.vp * world_pos;
    }
);

const char animated_model_vertex[] = RLR_SHADER_INLINE(
    layout(location = 0) in vec3 pos;
    layout(location = 1) in vec3 normal;
    layout(location = 2) in vec2 uv;
    layout(location = 3) in uvec4 joints;
    layout(location = 4) in uvec4 weights;
    layout(location = 5) in vec3 instance_mat_col_0;
    layout(location = 6) in vec3 instance_mat_col_1;
    layout(location = 7) in vec3 instance_mat_col_2;
    layout(location = 8) in vec3 instance_mat_col_3;
    layout(location = 9) in uint pose_a_offset;
    layout(location = 10) in uint pose_b_offset;
    layout(location = 11) in float pose_lerp;

    layout(std140) uniform ubo_model {
        mat4 vp;
        vec3 camera_pos;
    } model;

    uniform sampler2D poses;

    out vec2 frag_uv;
    out vec3 frag_normal;
    out vec3 frag_vert_pos;

    mat4 fetch_pose(uint matrix_index, uint tex_w) {
        uint texel_index = matrix_index * 4u;
        uint x = texel_index % tex_w;
        uint y = texel_index / tex_w;

        vec3 c0 = texelFetch(poses, ivec2(x + 0u, y), 0).xyz;
        vec3 c1 = texelFetch(poses, ivec2(x + 1u, y), 0).xyz;
        vec3 c2 = texelFetch(poses, ivec2(x + 2u, y), 0).xyz;
        vec3 c3 = texelFetch(poses, ivec2(x + 3u, y), 0).xyz;

        return mat4(
            vec4(c0, 0.0),
            vec4(c1, 0.0),
            vec4(c2, 0.0),
            vec4(c3, 1.0)
        );
    }

    mat4 fetch_blended_joint(uint joint_index, uint tex_w) {
        mat4 a = fetch_pose(pose_a_offset + joint_index, tex_w);
        mat4 b = fetch_pose(pose_b_offset + joint_index, tex_w);
        return a * (1.0 - pose_lerp) + b * pose_lerp;
    }

    void main() {
        mat4 instance_matrix = mat4(
            vec4(instance_mat_col_0, 0.0),
            vec4(instance_mat_col_1, 0.0),
            vec4(instance_mat_col_2, 0.0),
            vec4(instance_mat_col_3, 1.0)
        );

        vec4 w = vec4(
            float(weights[0]) / 255.0,
            float(weights[1]) / 255.0,
            float(weights[2]) / 255.0,
            float(weights[3]) / 255.0
        );

        uint tex_w = uint(textureSize(poses, 0).x);
        mat4 skin = 
              (fetch_blended_joint(joints.x, tex_w) * w.x)
            + (fetch_blended_joint(joints.y, tex_w) * w.y)
            + (fetch_blended_joint(joints.z, tex_w) * w.z)
            + (fetch_blended_joint(joints.w, tex_w) * w.w);

        vec4 skinned_pos = skin * vec4(pos, 1.0);
        vec4 world_pos = instance_matrix * skinned_pos;
        frag_vert_pos = world_pos.xyz;
        frag_normal = mat3(transpose(inverse(instance_matrix))) * (skin * vec4(normal, 0)).xyz;
        frag_uv = uv;
        gl_Position = model.vp * world_pos;
    }
);

static rlr_pipeline_static_model_draw_command_t rlr_pipeline_model_create_static_model_draw_command(rlr_res_static_model_t* model, rlr_res_shader_t* shader, uint64_t new_index) {
    rlr_pipeline_model_t* pm = RLR_PIPELINE_MODEL;

    rlr_pipeline_static_model_draw_command_t cmd = {
        .model = model,
        .shader = shader,
        .instances = NULL,
        .mesh_vaos = NULL,
        .dirty = false,
        .generation = pm->generation_counter++,
        .index = new_index,
        .instance_vbo = rlr_backend()->create_buffer(),
    };

    for(int64_t i = 0; i < arrlen(model->meshes); i++) {
        rlr_res_static_mesh_t* mesh = &model->meshes[i];
        uint64_t vao = rlr_backend()->create_vertex_array();
        arrpush(cmd.mesh_vaos, vao);
        rlr_backend()->bind_vertex_array(vao);
        rlr_backend()->bind_buffer(mesh->vbo, RLR_BACKEND_BUFFER_ARRAY);
        rlr_backend()->set_vertex_array_attrib(RLR_BACKEND_VERTEX_ARRAY_ATTRIB_PER_VERTEX, 0, 3, RLR_BACKEND_BUFFER_TYPE_FLOAT, false, sizeof(rlr_static_model_vertex_t), offsetof(rlr_static_model_vertex_t, pos));
        rlr_backend()->set_vertex_array_attrib(RLR_BACKEND_VERTEX_ARRAY_ATTRIB_PER_VERTEX, 1, 3, RLR_BACKEND_BUFFER_TYPE_FLOAT, false, sizeof(rlr_static_model_vertex_t), offsetof(rlr_static_model_vertex_t, normal));
        rlr_backend()->set_vertex_array_attrib(RLR_BACKEND_VERTEX_ARRAY_ATTRIB_PER_VERTEX, 2, 2, RLR_BACKEND_BUFFER_TYPE_FLOAT, false, sizeof(rlr_static_model_vertex_t), offsetof(rlr_static_model_vertex_t, uv));
        rlr_backend()->bind_buffer(mesh->ebo, RLR_BACKEND_BUFFER_ELEMENT_ARRAY);
        rlr_backend()->bind_buffer(cmd.instance_vbo, RLR_BACKEND_BUFFER_ARRAY);
        rlr_backend()->set_vertex_array_attrib(RLR_BACKEND_VERTEX_ARRAY_ATTRIB_PER_INSTANCE, 3, 3, RLR_BACKEND_BUFFER_TYPE_FLOAT, false, sizeof(rlr_pipeline_static_model_instance_t), offsetof(rlr_pipeline_static_model_instance_t, matrix) + sizeof(float) * 3 * 0);
        rlr_backend()->set_vertex_array_attrib(RLR_BACKEND_VERTEX_ARRAY_ATTRIB_PER_INSTANCE, 4, 3, RLR_BACKEND_BUFFER_TYPE_FLOAT, false, sizeof(rlr_pipeline_static_model_instance_t), offsetof(rlr_pipeline_static_model_instance_t, matrix) + sizeof(float) * 3 * 1);
        rlr_backend()->set_vertex_array_attrib(RLR_BACKEND_VERTEX_ARRAY_ATTRIB_PER_INSTANCE, 5, 3, RLR_BACKEND_BUFFER_TYPE_FLOAT, false, sizeof(rlr_pipeline_static_model_instance_t), offsetof(rlr_pipeline_static_model_instance_t, matrix) + sizeof(float) * 3 * 2);
        rlr_backend()->set_vertex_array_attrib(RLR_BACKEND_VERTEX_ARRAY_ATTRIB_PER_INSTANCE, 6, 3, RLR_BACKEND_BUFFER_TYPE_FLOAT, false, sizeof(rlr_pipeline_static_model_instance_t), offsetof(rlr_pipeline_static_model_instance_t, matrix) + sizeof(float) * 3 * 3);
    }

    return cmd;
}

bool rlr_pipeline_model_init() {
    rlr_pipeline_model_t* pm = RLR_PIPELINE_MODEL;

    pm->opaque_static_model_commands = NULL;
    pm->obj_static_models = rlr_sparse_gen_allocator_create(sizeof(rlr_obj_static_model_t));
    pm->shader_opaque_static_model = rlr_res_shader_create(static_model_vertex, model_fragment);
    pm->shader_opaque_animated_model = rlr_res_shader_create(animated_model_vertex, model_fragment);
    pm->generation_counter = 0;
    if(!pm->shader_opaque_static_model) {
        goto err;
    }
    rlr_res_shader_bind_uniform_slot(pm->shader_opaque_static_model, "ubo_model", RLR_INTERNAL_UBO_MODEL);
    rlr_res_shader_bind_uniform_slot(pm->shader_opaque_static_model, "ubo_material", RLR_INTERNAL_UBO_MATERIAL);
    rlr_res_shader_bind_texture_slot(pm->shader_opaque_static_model, "tex", 0);
    rlr_res_shader_bind_texture_slot(pm->shader_opaque_static_model, "cube_map", 4);

    rlr_res_shader_bind_uniform_slot(pm->shader_opaque_animated_model, "ubo_model", RLR_INTERNAL_UBO_MODEL);
    rlr_res_shader_bind_uniform_slot(pm->shader_opaque_animated_model, "ubo_material", RLR_INTERNAL_UBO_MATERIAL);
    rlr_res_shader_bind_texture_slot(pm->shader_opaque_animated_model, "tex", 0);
    rlr_res_shader_bind_texture_slot(pm->shader_opaque_animated_model, "cube_map", 4);
    rlr_res_shader_bind_texture_slot(pm->shader_opaque_animated_model, "poses", 1);

    return true;
err:
    rlr_pipeline_model_deinit();
    return false;
}

void rlr_pipeline_model_draw() {
    rlr_pipeline_model_t* pm = RLR_PIPELINE_MODEL;

    //reupload command instance vbos that are dirty
    for(int64_t i = 0; i < arrlen(pm->opaque_static_model_commands); i++) {
        rlr_pipeline_static_model_draw_command_t* cmd = &pm->opaque_static_model_commands[i];
        if(cmd->dirty) {
            rlr_backend()->bind_buffer(cmd->instance_vbo, RLR_BACKEND_BUFFER_ARRAY);
            rlr_backend()->update_buffer(RLR_BACKEND_BUFFER_ARRAY, sizeof(rlr_pipeline_static_model_instance_t) * arrlenu(cmd->instances), cmd->instances, RLR_BACKEND_BUFFER_USAGE_DYNAMIC);
            cmd->dirty = false;
        }
    }

    //draw
    for(int64_t i = 0; i < arrlen(pm->opaque_static_model_commands); i++) {
        rlr_pipeline_static_model_draw_command_t* command = &pm->opaque_static_model_commands[i];
        rlr_res_shader_bind(pm->shader_opaque_static_model);
        
        for(int64_t j = 0; j < arrlen(command->mesh_vaos); j++) {
            rlr_res_static_mesh_t* mesh = &command->model->meshes[j];
            rlr_res_uniform_bind(mesh->material_ubo, RLR_INTERNAL_UBO_MATERIAL);
            if(mesh->texture_base) {
                rlr_res_texture_bind(mesh->texture_base, 0);
            } else {
                rlr_res_texture_bind(rlr()->texture_white, 0);
            }

            rlr_backend()->bind_vertex_array(command->mesh_vaos[j]);
            rlr_backend()->draw_elements_instanced(0, mesh->index_count, RLR_BACKEND_BUFFER_TYPE_U32, arrlenu(command->instances));
        }
    }
}

void rlr_pipeline_model_deinit() {
    rlr_pipeline_model_t* pm = RLR_PIPELINE_MODEL;
    if(!pm) {
        return;
    }

    arrfree(pm->opaque_static_model_commands);
    rlr_res_shader_free(pm->shader_opaque_static_model);
    rlr_res_shader_free(pm->shader_opaque_animated_model);
}

rlr_pipeline_static_model_draw_command_t* rlr_pipeline_model_find_static_model_draw_command(rlr_res_static_model_t* model, rlr_res_shader_t* shader) {
    rlr_pipeline_model_t* pm = RLR_PIPELINE_MODEL;

    //todo: use binary search instead
    for(int64_t i = 0; i < arrlen(pm->opaque_static_model_commands); i++) {
        rlr_pipeline_static_model_draw_command_t* cmd = &pm->opaque_static_model_commands[i];
        if(cmd->model == model && cmd->shader == shader) {
            return cmd;
        }
    }

    arrpush(pm->opaque_static_model_commands, rlr_pipeline_model_create_static_model_draw_command(model, shader, arrlen(pm->opaque_static_model_commands)));
    return &arrlast(pm->opaque_static_model_commands);
}

uint32_t rlr_pipeline_model_add_static_model_instance(rlr_pipeline_static_model_draw_command_t* command, rlr_pipeline_static_model_instance_t data) {
    arrpush(command->instances, data);
    command->dirty = true;
    return arrlen(command->instances) - 1;
}

void rlr_pipeline_model_update_static_model_instance(uint32_t cmd_index, uint32_t cmd_generation, uint32_t instance_index, rlr_pipeline_static_model_instance_t data) {
    rlr_pipeline_model_t* pm = RLR_PIPELINE_MODEL;

    if(cmd_index > arrlenu(pm->opaque_static_model_commands) - 1) {
        return;
    }
    rlr_pipeline_static_model_draw_command_t* cmd = &pm->opaque_static_model_commands[cmd_index];
    if(cmd->generation != cmd_generation) {
        return;
    }

    cmd->instances[instance_index] = data;
    cmd->dirty = true;
}

rlr_sparse_gen_allocator_handle_t rlr_pipeline_model_alloc_static_model() {
    rlr_pipeline_model_t* pm = RLR_PIPELINE_MODEL;
    return rlr_sparse_gen_allocator_alloc(pm->obj_static_models);
}

rlr_obj_static_model_t* rlr_pipeline_model_get_static_model(rlr_sparse_gen_allocator_handle_t handle) {
    rlr_pipeline_model_t* pm = RLR_PIPELINE_MODEL;
    return rlr_sparse_gen_allocator_get_unchecked(pm->obj_static_models, handle);
}

void rlr_pipeline_model_free_static_model(rlr_sparse_gen_allocator_handle_t handle) {
    rlr_pipeline_model_t* pm = RLR_PIPELINE_MODEL;
    rlr_sparse_gen_allocator_dealloc(pm->obj_static_models, handle);
}