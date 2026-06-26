#include <stdlib.h>
#include <stb_ds.h>
#include "rlr/objects/static_model.h"
#include "rlr/resources/static_model.h"
#include "rlr/resources/uniform.h"
#include "rlr/resources/texture.h"
#include "rlr/resources/shader.h"
#include "internal/impl.h"
#include "model.h"

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
    layout(location = 6) in float instance_alpha;

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
    flat in float frag_alpha;
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
        out_color = vec4(final_rgb, tex_color.a * material.color.a * frag_alpha);
    }
);

const char model_vertex[] = RLR_SHADER_INLINE(
    layout(location = 0) in vec3 pos;
    layout(location = 1) in vec3 normal;
    layout(location = 2) in vec2 uv;
    layout(location = 3) in vec3 instance_mat_col_0;
    layout(location = 4) in vec3 instance_mat_col_1;
    layout(location = 5) in vec3 instance_mat_col_2;
    layout(location = 6) in vec3 instance_mat_col_3;
    layout(location = 7) in float instance_alpha;

    layout(std140) uniform ubo_model {
        mat4 vp;
        vec3 camera_pos;
    } model;

    out vec2 frag_uv;
    out vec3 frag_normal;
    out vec3 frag_vert_pos;
    flat out float frag_alpha;

    void main() {
        mat4 instance_matrix = mat4(
            vec4(instance_mat_col_0, 0.0),
            vec4(instance_mat_col_1, 0.0),
            vec4(instance_mat_col_2, 0.0),
            vec4(instance_mat_col_3, 1.0)
        );

        frag_alpha = instance_alpha;
        vec4 world_pos = instance_matrix * vec4(pos, 1.0);
        frag_vert_pos = world_pos.xyz;
        frag_normal = mat3(transpose(inverse(instance_matrix))) * normal;
        frag_uv = uv;
        gl_Position = model.vp * world_pos;
    }
);

static rlr_pipeline_model_draw_command_t rlr_pipeline_model_new_command(rlr_res_static_mesh_t* mesh, rlr_res_shader_t* shader) {
    rlr_pipeline_model_draw_command_t cmd = {
        .mesh = mesh,
        .shader = shader,
        .instances = NULL,
        .dirty = false,
        .vao = rlr_backend()->create_vertex_array(),
        .instance_vbo = rlr_backend()->create_buffer()
    };
    rlr_backend()->bind_vertex_array(cmd.vao);
    rlr_backend()->bind_buffer(mesh->vbo, RLR_BACKEND_BUFFER_ARRAY);
    rlr_backend()->set_vertex_array_attrib(RLR_BACKEND_VERTEX_ARRAY_ATTRIB_PER_VERTEX, 0, 3, RLR_BACKEND_BUFFER_TYPE_FLOAT, false, sizeof(rlr_model_static_vertex_t), offsetof(rlr_model_static_vertex_t, pos));
    rlr_backend()->set_vertex_array_attrib(RLR_BACKEND_VERTEX_ARRAY_ATTRIB_PER_VERTEX, 1, 3, RLR_BACKEND_BUFFER_TYPE_FLOAT, false, sizeof(rlr_model_static_vertex_t), offsetof(rlr_model_static_vertex_t, normal));
    rlr_backend()->set_vertex_array_attrib(RLR_BACKEND_VERTEX_ARRAY_ATTRIB_PER_VERTEX, 2, 2, RLR_BACKEND_BUFFER_TYPE_FLOAT, false, sizeof(rlr_model_static_vertex_t), offsetof(rlr_model_static_vertex_t, uv));
    rlr_backend()->bind_buffer(mesh->ebo, RLR_BACKEND_BUFFER_ELEMENT_ARRAY);
    rlr_backend()->bind_buffer(cmd.instance_vbo, RLR_BACKEND_BUFFER_ARRAY);
    rlr_backend()->set_vertex_array_attrib(RLR_BACKEND_VERTEX_ARRAY_ATTRIB_PER_INSTANCE, 3, 3, RLR_BACKEND_BUFFER_TYPE_FLOAT, false, sizeof(rlr_pipeline_model_instance_t), offsetof(rlr_pipeline_model_instance_t, matrix) + sizeof(float) * 3 * 0);
    rlr_backend()->set_vertex_array_attrib(RLR_BACKEND_VERTEX_ARRAY_ATTRIB_PER_INSTANCE, 4, 3, RLR_BACKEND_BUFFER_TYPE_FLOAT, false, sizeof(rlr_pipeline_model_instance_t), offsetof(rlr_pipeline_model_instance_t, matrix) + sizeof(float) * 3 * 1);
    rlr_backend()->set_vertex_array_attrib(RLR_BACKEND_VERTEX_ARRAY_ATTRIB_PER_INSTANCE, 5, 3, RLR_BACKEND_BUFFER_TYPE_FLOAT, false, sizeof(rlr_pipeline_model_instance_t), offsetof(rlr_pipeline_model_instance_t, matrix) + sizeof(float) * 3 * 2);
    rlr_backend()->set_vertex_array_attrib(RLR_BACKEND_VERTEX_ARRAY_ATTRIB_PER_INSTANCE, 6, 3, RLR_BACKEND_BUFFER_TYPE_FLOAT, false, sizeof(rlr_pipeline_model_instance_t), offsetof(rlr_pipeline_model_instance_t, matrix) + sizeof(float) * 3 * 3);
    rlr_backend()->set_vertex_array_attrib(RLR_BACKEND_VERTEX_ARRAY_ATTRIB_PER_INSTANCE, 7, 1, RLR_BACKEND_BUFFER_TYPE_FLOAT, false, sizeof(rlr_pipeline_model_instance_t), offsetof(rlr_pipeline_model_instance_t, alpha));
    return cmd;
}

bool rlr_pipeline_model_init() {
    rlr_pipeline_model_t* pm = RLR_PIPELINE_MODEL;

    pm->commands = NULL;
    pm->obj_models = NULL;
    pm->shader_transparent = NULL;
    pm->shader_opaque = rlr_res_shader_create(model_vertex, model_fragment);
    if(!pm->shader_opaque) {
        goto err;
    }
    rlr_res_shader_bind_uniform_slot(pm->shader_opaque, "ubo_model", RLR_INTERNAL_UBO_MODEL);
    rlr_res_shader_bind_uniform_slot(pm->shader_opaque, "ubo_material", RLR_INTERNAL_UBO_MATERIAL);
    rlr_res_shader_bind_texture_slot(pm->shader_opaque, "tex", 0);
    rlr_res_shader_bind_texture_slot(pm->shader_opaque, "cube_map", 4);

    return true;
err:
    rlr_pipeline_model_deinit();
    return false;
}

void rlr_pipeline_model_draw() {
    rlr_pipeline_model_t* pm = RLR_PIPELINE_MODEL;

    if(arrlen(pm->commands) == 0) {
        for(int32_t i = 0; i < arrlen(pm->obj_models); i++) {
            rlr_obj_static_model_t* model = &pm->obj_models[i];

            for(int32_t s = 0; s < arrlen(model->model->meshes); s++) {
                rlr_res_static_mesh_t* mesh = &model->model->meshes[s];
                rlr_pipeline_model_draw_command_t* cmd = rlr_pipeline_model_find_draw_command(mesh, pm->shader_opaque);

                rlr_mat4x4_t inverse = rlr_mat4x4_inverse(&model->matrix);
                rlr_mat4x4_t normal = rlr_mat4x4_transpose(&inverse);

                rlr_pipeline_model_instance_t inst = (rlr_pipeline_model_instance_t){
                    .matrix = rlr_mat4x4_to_affine_mat4x3(&model->matrix),
                    .alpha = model->alpha,
                };

                arrpush(cmd->instances, inst);
                cmd->dirty = true;
            }
        }
    }

    //reupload command instance vbos that are dirty
    for(int64_t i = 0; i < arrlen(pm->commands); i++) {
        rlr_pipeline_model_draw_command_t* cmd = &pm->commands[i];
        if(cmd->dirty) {
            rlr_backend()->bind_buffer(cmd->instance_vbo, RLR_BACKEND_BUFFER_ARRAY);
            rlr_backend()->update_buffer(RLR_BACKEND_BUFFER_ARRAY, sizeof(rlr_pipeline_model_instance_t) * arrlenu(cmd->instances), cmd->instances, RLR_BACKEND_BUFFER_USAGE_DYNAMIC);
            cmd->dirty = false;
        }
    }

    //draw
    for(int64_t i = 0; i < arrlen(pm->commands); i++) {
        rlr_pipeline_model_draw_command_t* command = &pm->commands[i];
        rlr_res_shader_bind(command->shader);
        rlr_res_uniform_update(rlr()->ubos[RLR_INTERNAL_UBO_MATERIAL], 0, &command->mesh->material, sizeof(rlr_uniform_material_t));
        if(command->mesh->texture_base) {
            rlr_res_texture_bind(command->mesh->texture_base, 0);
        } else {
            rlr_res_texture_bind(rlr()->texture_white, 0);
        }
        rlr_backend()->bind_vertex_array(command->vao);
        rlr_backend()->draw_elements_instanced(0, command->mesh->index_count, RLR_BACKEND_BUFFER_TYPE_U32, arrlenu(command->instances));
    }
}

void rlr_pipeline_model_deinit() {
    rlr_pipeline_model_t* pm = RLR_PIPELINE_MODEL;
    if(!pm) {
        return;
    }

    arrfree(pm->commands);
    rlr_res_shader_free(pm->shader_opaque);
    rlr_res_shader_free(pm->shader_transparent);
}

rlr_pipeline_model_draw_command_t* rlr_pipeline_model_find_draw_command(rlr_res_static_mesh_t* mesh, rlr_res_shader_t* shader) {
    rlr_pipeline_model_t* pm = RLR_PIPELINE_MODEL;

    //todo: use binary search instead
    for(int64_t i = 0; i < arrlen(pm->commands); i++) {
        rlr_pipeline_model_draw_command_t* cmd = &pm->commands[i];
        if(cmd->mesh == mesh && cmd->shader == shader) {
            return cmd;
        }
    }

    rlr_pipeline_model_draw_command_t new_command = rlr_pipeline_model_new_command(mesh, shader);
    arrpush(pm->commands, new_command);
    return &arrlast(pm->commands);
}

rlr_obj_static_model_t* rlr_pipeline_model_alloc_static_model() {
    rlr_pipeline_model_t* pm = RLR_PIPELINE_MODEL;
    arrpush(pm->obj_models, (rlr_obj_static_model_t){0});
    rlr_obj_static_model_t* sm = &arrlast(pm->obj_models);
    sm->index = arrlenu(pm->obj_models) - 1;
    return sm;
}

void rlr_pipeline_model_free_static_model(rlr_obj_static_model_t* sm) {

}