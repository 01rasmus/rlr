#include <stdlib.h>
#include "../../external/stb_ds.h"
#include "../../rlr/objects/animated_model.h"
#include "../../rlr/objects/static_model.h"
#include "../../rlr/resources/animated_model.h"
#include "../../rlr/resources/static_model.h"
#include "../../rlr/resources/uniform.h"
#include "../../rlr/resources/texture.h"
#include "../../rlr/resources/shader.h"
#include "../../rlr/math/scalar.h"
#include "../core/res_types.h"
#include "../impl.h"
#include "model.h"

#define RLR_PIPELINE_MODEL    (&rlr()->pipeline_model)

const char simple_fragment[] = RLR_SHADER_INLINE(
    in vec2 frag_uv;
    out vec4 out_color;
    uniform sampler2D tex;

    layout(std140) uniform ubo_material {
        vec4 color;
        float shininess;
        float specular_strength;
        float reflectiveness;
    } material;

    void main() {
        out_color = texture(tex, frag_uv) * material.color;
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

const char animated_model_vertex_ubo_poses[] = RLR_SHADER_INLINE(
    layout(location = 0) in vec3 pos;
    layout(location = 1) in vec3 normal;
    layout(location = 2) in vec2 uv;
    layout(location = 3) in uvec4 joints;
    layout(location = 4) in uvec4 weights;
    layout(location = 5) in vec3 instance_mat_col_0;
    layout(location = 6) in vec3 instance_mat_col_1;
    layout(location = 7) in vec3 instance_mat_col_2;
    layout(location = 8) in vec3 instance_mat_col_3;

    layout(std140) uniform ubo_model {
        mat4 vp;
        vec3 camera_pos;
    } model;

    layout(std140) uniform ubo_bone_trans {
        vec4 translations[1024];
    };

    layout(std140) uniform ubo_bone_rots {
        vec4 rotations[1024];
    };

    layout(std140) uniform ubo_bone_info {
        uint joint_count;
    };

    out vec2 frag_uv;
    out vec3 frag_normal;
    out vec3 frag_vert_pos;

    vec3 quat_rotate(vec4 q, vec3 v) {
        vec3 temp = 2.0 * cross(q.xyz, v);
        return v + q.w * temp + cross(q.xyz, temp);
    }

    void main() {
        mat4 instance_matrix = mat4(
            vec4(instance_mat_col_0, 0.0),
            vec4(instance_mat_col_1, 0.0),
            vec4(instance_mat_col_2, 0.0),
            vec4(instance_mat_col_3, 1.0)
        );

        //get bone
        uint base = uint(gl_InstanceID) * joint_count;
        uint bone_index = base + joints.x;
        vec4 bone_rot = rotations[bone_index];
        vec3 bone_pos = translations[bone_index].xyz;
        vec3 skinned_pos = quat_rotate(bone_rot, pos) + bone_pos;

        vec4 world_pos = instance_matrix * vec4(skinned_pos, 1.0);
        frag_vert_pos = world_pos.xyz;
        frag_normal = normal;// mat3(transpose(inverse(instance_matrix))) * (skin * vec4(normal, 0.0)).xyz;
        frag_uv = uv;
        gl_Position = model.vp * world_pos;
    }
);

const char animated_model_vertex_texture_poses[] = RLR_SHADER_INLINE(
    layout(location = 0) in vec3 pos;
    layout(location = 1) in vec3 normal;
    layout(location = 2) in vec2 uv;
    layout(location = 3) in uvec4 joints;
    layout(location = 4) in uvec4 weights;
    layout(location = 5) in vec3 instance_mat_col_0;
    layout(location = 6) in vec3 instance_mat_col_1;
    layout(location = 7) in vec3 instance_mat_col_2;
    layout(location = 8) in vec3 instance_mat_col_3;
    layout(location = 9) in uint pose_a_offset_primary;
    layout(location = 10) in uint pose_b_offset_primary;
    layout(location = 11) in uint pose_lerp_primary;
    layout(location = 12) in uint pose_a_offset_secondary;
    layout(location = 13) in uint pose_b_offset_secondary;
    layout(location = 14) in uint pose_lerp_secondary;
    layout(location = 15) in uint transition_lerp;

    layout(std140) uniform ubo_model {
        mat4 vp;
        vec3 camera_pos;
    } model;

    uniform sampler2D poses;

    out vec2 frag_uv;
    out vec3 frag_normal;
    out vec3 frag_vert_pos;

    const float UINT8_MAX_RATIO = 1.0 / 255.0;
    const float UINT16_MAX_RATIO = 1.0 / 65535.0;

    struct rows_t {
        vec4 r0;
        vec4 r1;
        vec4 r2;
    };

    rows_t get_pose_rows(uint offset_a, uint offset_b, uint tex_w, float lerp) {
        rows_t r = rows_t(vec4(0.0), vec4(0.0), vec4(0.0));

        float inverse_lerp = 1.0 - lerp;

        for(int i = 0; i < 4; i++) {
            uint atexel_index = (offset_a + joints[i]) * 3u;
            uint ax = atexel_index % tex_w;
            uint ay = atexel_index / tex_w;
            uint btexel_index = (offset_b + joints[i]) * 3u;
            uint bx = btexel_index % tex_w;
            uint by = btexel_index / tex_w;

            r.r0 += (texelFetch(poses, ivec2(ax + 0u, ay), 0) * inverse_lerp + texelFetch(poses, ivec2(bx + 0u, by), 0) * lerp) * float(weights[i]) * UINT8_MAX_RATIO;
            r.r1 += (texelFetch(poses, ivec2(ax + 1u, ay), 0) * inverse_lerp + texelFetch(poses, ivec2(bx + 1u, by), 0) * lerp) * float(weights[i]) * UINT8_MAX_RATIO;
            r.r2 += (texelFetch(poses, ivec2(ax + 2u, ay), 0) * inverse_lerp + texelFetch(poses, ivec2(bx + 2u, by), 0) * lerp) * float(weights[i]) * UINT8_MAX_RATIO;
        }

        return r;
    }

    void main() {
        mat4 instance_matrix = mat4(
            vec4(instance_mat_col_0, 0.0),
            vec4(instance_mat_col_1, 0.0),
            vec4(instance_mat_col_2, 0.0),
            vec4(instance_mat_col_3, 1.0)
        );

        float lerp_prim = float(pose_lerp_primary) * UINT8_MAX_RATIO;
        float lerp_sec = float(pose_lerp_secondary) * UINT8_MAX_RATIO;
        float lerp_trans = float(transition_lerp) * UINT16_MAX_RATIO;
        uint tex_w = uint(textureSize(poses, 0).x);

        vec4 po = vec4(pos, 1.0);
        
        rows_t p = get_pose_rows(pose_a_offset_primary, pose_b_offset_primary, tex_w, lerp_prim);
        rows_t final = rows_t(p.r0, p.r1, p.r2);

        if(transition_lerp > 0u) {
            rows_t s = get_pose_rows(pose_a_offset_secondary, pose_b_offset_secondary, tex_w, lerp_sec);
            float inverse_lerp_trans = 1.0 - lerp_trans;
            final.r0 = final.r0 * inverse_lerp_trans + lerp_trans * s.r0;
            final.r1 = final.r1 * inverse_lerp_trans + lerp_trans * s.r1;
            final.r2 = final.r2 * inverse_lerp_trans + lerp_trans * s.r2;
        }

        vec4 skinned_pos = vec4(
            dot(final.r0, po),
            dot(final.r1, po),
            dot(final.r2, po),
            1.0
        );

        vec4 world_pos = instance_matrix * skinned_pos;
        frag_vert_pos = world_pos.xyz;
        frag_normal = mat3(transpose(inverse(instance_matrix))) * (skinned_pos * vec4(normal, 0.0)).xyz;
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
        rlr_backend()->bind_buffer(mesh->ebo, RLR_BACKEND_BUFFER_ELEMENT_ARRAY);
        rlr_backend()->bind_buffer(mesh->vbo, RLR_BACKEND_BUFFER_ARRAY);
        rlr_backend()->set_vertex_array_attrib(RLR_BACKEND_VERTEX_ARRAY_ATTRIB_PER_VERTEX, 0, 3, RLR_BACKEND_BUFFER_TYPE_FLOAT, false, sizeof(rlr_static_model_vertex_t), offsetof(rlr_static_model_vertex_t, pos));
        rlr_backend()->set_vertex_array_attrib(RLR_BACKEND_VERTEX_ARRAY_ATTRIB_PER_VERTEX, 1, 3, RLR_BACKEND_BUFFER_TYPE_FLOAT, false, sizeof(rlr_static_model_vertex_t), offsetof(rlr_static_model_vertex_t, normal));
        rlr_backend()->set_vertex_array_attrib(RLR_BACKEND_VERTEX_ARRAY_ATTRIB_PER_VERTEX, 2, 2, RLR_BACKEND_BUFFER_TYPE_FLOAT, false, sizeof(rlr_static_model_vertex_t), offsetof(rlr_static_model_vertex_t, uv));
        rlr_backend()->bind_buffer(cmd.instance_vbo, RLR_BACKEND_BUFFER_ARRAY);
        rlr_backend()->set_vertex_array_attrib(RLR_BACKEND_VERTEX_ARRAY_ATTRIB_PER_INSTANCE, 3, 3, RLR_BACKEND_BUFFER_TYPE_FLOAT, false, sizeof(rlr_pipeline_static_model_instance_t), offsetof(rlr_pipeline_static_model_instance_t, matrix) + sizeof(float) * 3 * 0);
        rlr_backend()->set_vertex_array_attrib(RLR_BACKEND_VERTEX_ARRAY_ATTRIB_PER_INSTANCE, 4, 3, RLR_BACKEND_BUFFER_TYPE_FLOAT, false, sizeof(rlr_pipeline_static_model_instance_t), offsetof(rlr_pipeline_static_model_instance_t, matrix) + sizeof(float) * 3 * 1);
        rlr_backend()->set_vertex_array_attrib(RLR_BACKEND_VERTEX_ARRAY_ATTRIB_PER_INSTANCE, 5, 3, RLR_BACKEND_BUFFER_TYPE_FLOAT, false, sizeof(rlr_pipeline_static_model_instance_t), offsetof(rlr_pipeline_static_model_instance_t, matrix) + sizeof(float) * 3 * 2);
        rlr_backend()->set_vertex_array_attrib(RLR_BACKEND_VERTEX_ARRAY_ATTRIB_PER_INSTANCE, 6, 3, RLR_BACKEND_BUFFER_TYPE_FLOAT, false, sizeof(rlr_pipeline_static_model_instance_t), offsetof(rlr_pipeline_static_model_instance_t, matrix) + sizeof(float) * 3 * 3);
    }

    return cmd;
}

static rlr_pipeline_animated_model_draw_command_t rlr_pipeline_model_create_animated_model_draw_command(rlr_res_animated_model_t* model, rlr_res_shader_t* shader, uint64_t new_index) {
    rlr_pipeline_model_t* pm = RLR_PIPELINE_MODEL;

    rlr_pipeline_animated_model_draw_command_t cmd = {
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
        rlr_res_animated_mesh_t* mesh = &model->meshes[i];
        uint64_t vao = rlr_backend()->create_vertex_array();
        arrpush(cmd.mesh_vaos, vao);
        rlr_backend()->bind_vertex_array(vao);
        rlr_backend()->bind_buffer(mesh->ebo, RLR_BACKEND_BUFFER_ELEMENT_ARRAY);
        rlr_backend()->bind_buffer(mesh->vbo, RLR_BACKEND_BUFFER_ARRAY);
        rlr_backend()->set_vertex_array_attrib(RLR_BACKEND_VERTEX_ARRAY_ATTRIB_PER_VERTEX, 0, 3, RLR_BACKEND_BUFFER_TYPE_FLOAT, false, sizeof(rlr_animated_model_vertex_t), offsetof(rlr_animated_model_vertex_t, pos));
        rlr_backend()->set_vertex_array_attrib(RLR_BACKEND_VERTEX_ARRAY_ATTRIB_PER_VERTEX, 1, 3, RLR_BACKEND_BUFFER_TYPE_FLOAT, false, sizeof(rlr_animated_model_vertex_t), offsetof(rlr_animated_model_vertex_t, normal));
        rlr_backend()->set_vertex_array_attrib(RLR_BACKEND_VERTEX_ARRAY_ATTRIB_PER_VERTEX, 2, 2, RLR_BACKEND_BUFFER_TYPE_FLOAT, false, sizeof(rlr_animated_model_vertex_t), offsetof(rlr_animated_model_vertex_t, uv));
        rlr_backend()->set_vertex_array_attribi(RLR_BACKEND_VERTEX_ARRAY_ATTRIB_PER_VERTEX, 3, 4, RLR_BACKEND_BUFFER_TYPE_U8, sizeof(rlr_animated_model_vertex_t), offsetof(rlr_animated_model_vertex_t, joints));
        rlr_backend()->set_vertex_array_attribi(RLR_BACKEND_VERTEX_ARRAY_ATTRIB_PER_VERTEX, 4, 4, RLR_BACKEND_BUFFER_TYPE_U8, sizeof(rlr_animated_model_vertex_t), offsetof(rlr_animated_model_vertex_t, weights));
        rlr_backend()->bind_buffer(cmd.instance_vbo, RLR_BACKEND_BUFFER_ARRAY);
        rlr_backend()->set_vertex_array_attrib(RLR_BACKEND_VERTEX_ARRAY_ATTRIB_PER_INSTANCE, 5, 3, RLR_BACKEND_BUFFER_TYPE_FLOAT, false, sizeof(rlr_pipeline_animated_model_instance_t), offsetof(rlr_pipeline_animated_model_instance_t, matrix) + sizeof(float) * 3 * 0);
        rlr_backend()->set_vertex_array_attrib(RLR_BACKEND_VERTEX_ARRAY_ATTRIB_PER_INSTANCE, 6, 3, RLR_BACKEND_BUFFER_TYPE_FLOAT, false, sizeof(rlr_pipeline_animated_model_instance_t), offsetof(rlr_pipeline_animated_model_instance_t, matrix) + sizeof(float) * 3 * 1);
        rlr_backend()->set_vertex_array_attrib(RLR_BACKEND_VERTEX_ARRAY_ATTRIB_PER_INSTANCE, 7, 3, RLR_BACKEND_BUFFER_TYPE_FLOAT, false, sizeof(rlr_pipeline_animated_model_instance_t), offsetof(rlr_pipeline_animated_model_instance_t, matrix) + sizeof(float) * 3 * 2);
        rlr_backend()->set_vertex_array_attrib(RLR_BACKEND_VERTEX_ARRAY_ATTRIB_PER_INSTANCE, 8, 3, RLR_BACKEND_BUFFER_TYPE_FLOAT, false, sizeof(rlr_pipeline_animated_model_instance_t), offsetof(rlr_pipeline_animated_model_instance_t, matrix) + sizeof(float) * 3 * 3);
        rlr_backend()->set_vertex_array_attribi(RLR_BACKEND_VERTEX_ARRAY_ATTRIB_PER_INSTANCE, 9, 1, RLR_BACKEND_BUFFER_TYPE_U32, sizeof(rlr_pipeline_animated_model_instance_t), offsetof(rlr_pipeline_animated_model_instance_t, pose_a_offset_primary));
        rlr_backend()->set_vertex_array_attribi(RLR_BACKEND_VERTEX_ARRAY_ATTRIB_PER_INSTANCE, 10, 1, RLR_BACKEND_BUFFER_TYPE_U32, sizeof(rlr_pipeline_animated_model_instance_t), offsetof(rlr_pipeline_animated_model_instance_t, pose_b_offset_primary));
        rlr_backend()->set_vertex_array_attribi(RLR_BACKEND_VERTEX_ARRAY_ATTRIB_PER_INSTANCE, 11, 1, RLR_BACKEND_BUFFER_TYPE_U8, sizeof(rlr_pipeline_animated_model_instance_t), offsetof(rlr_pipeline_animated_model_instance_t, lerp_primary));
        rlr_backend()->set_vertex_array_attribi(RLR_BACKEND_VERTEX_ARRAY_ATTRIB_PER_INSTANCE, 12, 1, RLR_BACKEND_BUFFER_TYPE_U32, sizeof(rlr_pipeline_animated_model_instance_t), offsetof(rlr_pipeline_animated_model_instance_t, pose_a_offset_secondary));
        rlr_backend()->set_vertex_array_attribi(RLR_BACKEND_VERTEX_ARRAY_ATTRIB_PER_INSTANCE, 13, 1, RLR_BACKEND_BUFFER_TYPE_U32, sizeof(rlr_pipeline_animated_model_instance_t), offsetof(rlr_pipeline_animated_model_instance_t, pose_b_offset_secondary));
        rlr_backend()->set_vertex_array_attribi(RLR_BACKEND_VERTEX_ARRAY_ATTRIB_PER_INSTANCE, 14, 1, RLR_BACKEND_BUFFER_TYPE_U8, sizeof(rlr_pipeline_animated_model_instance_t), offsetof(rlr_pipeline_animated_model_instance_t, lerp_secondary));
        rlr_backend()->set_vertex_array_attribi(RLR_BACKEND_VERTEX_ARRAY_ATTRIB_PER_INSTANCE, 15, 1, RLR_BACKEND_BUFFER_TYPE_U16, sizeof(rlr_pipeline_animated_model_instance_t), offsetof(rlr_pipeline_animated_model_instance_t, transition_lerp));
    }

    return cmd;
}

bool rlr_pipeline_model_init() {
    rlr_pipeline_model_t* pm = RLR_PIPELINE_MODEL;

    pm->opaque_static_model_commands = NULL;
    pm->opaque_animated_model_commands = NULL;
    pm->animated_model_path = RLR_PIPELINE_ANIMATED_MODEL_GPU;
    pm->shader_opaque_static_model = rlr_res_shader_create(static_model_vertex, model_fragment);
    pm->shader_opaque_animated_model_cpu = rlr_res_shader_create(animated_model_vertex_ubo_poses, model_fragment);
    pm->shader_opaque_animated_model_gpu = rlr_res_shader_create(animated_model_vertex_texture_poses, model_fragment);
    pm->generation_counter = 0;
    if(!pm->shader_opaque_static_model) {
        goto err;
    }
    if(!pm->shader_opaque_animated_model_cpu) {
        goto err;
    }
    if(!pm->shader_opaque_animated_model_gpu) {
        goto err;
    }

    //set static model shader slots
    rlr_res_shader_bind_uniform_slot(pm->shader_opaque_static_model, "ubo_model", RLR_INTERNAL_UBO_MODEL);
    rlr_res_shader_bind_uniform_slot(pm->shader_opaque_static_model, "ubo_material", RLR_INTERNAL_UBO_MATERIAL);
    rlr_res_shader_bind_texture_slot(pm->shader_opaque_static_model, "tex", 0);
    rlr_res_shader_bind_texture_slot(pm->shader_opaque_static_model, "cube_map", 4);

    //set animated model cpu calculated slots
    rlr_res_shader_bind_uniform_slot(pm->shader_opaque_animated_model_cpu, "ubo_model", RLR_INTERNAL_UBO_MODEL);
    rlr_res_shader_bind_uniform_slot(pm->shader_opaque_animated_model_cpu, "ubo_material", RLR_INTERNAL_UBO_MATERIAL);
    rlr_res_shader_bind_uniform_slot(pm->shader_opaque_animated_model_cpu, "ubo_bone_trans", RLR_INTERNAL_UBO_BONE_TRANSLATION);
    rlr_res_shader_bind_uniform_slot(pm->shader_opaque_animated_model_cpu, "ubo_bone_rots", RLR_INTERNAL_UBO_BONE_ROTATION);
    rlr_res_shader_bind_uniform_slot(pm->shader_opaque_animated_model_cpu, "ubo_bone_info", RLR_INTERNAL_UBO_BONE_INFO);
    rlr_res_shader_bind_texture_slot(pm->shader_opaque_animated_model_cpu, "tex", 0);
    rlr_res_shader_bind_texture_slot(pm->shader_opaque_animated_model_cpu, "cube_map", 4);

    //set animated model gpu skinned slots
    rlr_res_shader_bind_uniform_slot(pm->shader_opaque_animated_model_gpu, "ubo_model", RLR_INTERNAL_UBO_MODEL);
    rlr_res_shader_bind_uniform_slot(pm->shader_opaque_animated_model_gpu, "ubo_material", RLR_INTERNAL_UBO_MATERIAL);
    rlr_res_shader_bind_texture_slot(pm->shader_opaque_animated_model_gpu, "tex", 0);
    rlr_res_shader_bind_texture_slot(pm->shader_opaque_animated_model_gpu, "cube_map", 4);
    rlr_res_shader_bind_texture_slot(pm->shader_opaque_animated_model_gpu, "poses", 1);

    return true;
err:
    rlr_pipeline_model_deinit();
    return false;
}

static inline bool rlr_pipeline_update_animation_state(rlr_pipeline_model_t* pm, rlr_pipeline_animated_model_draw_command_t* cmd, rlr_obj_animation_state_t* state, double delta_time, float* out_lerp, uint32_t* out_pose_a, uint32_t* out_pose_b) {
    if(state->animation_index == -1) {
        return false;
    }

    rlr_res_animation_meta_t* meta = &cmd->model->animations.metas[state->animation_index];
    float frame_interval = 1.0 / meta->fps;
    uint32_t joint_count = cmd->model->animations.joint_count;

    float current_pose = state->animation_time / frame_interval;
    uint32_t pose_index_upper = rlr_clamp(ceilf(current_pose), 0, meta->pose_count - 1);
    uint32_t pose_index_below = rlr_clamp(floorf(current_pose), 0, meta->pose_count - 1);

    state->animation_time += delta_time * state->animation_speed;
    if(state->animation_time > meta->duration) {
        if(state->animation_loop) {
            state->animation_time -= meta->duration;
        } else {
            state->animation_index = -1;
        }
    }

    //set the out data
    *out_lerp = current_pose - floorf(current_pose);
    *out_pose_a = meta->pose_offset + pose_index_below * joint_count;
    *out_pose_b = meta->pose_offset + pose_index_upper * joint_count;
    return true;
}

static inline void rlr_pipeline_update_animations(rlr_pipeline_model_t* pm, double delta_time) {
    for(size_t i = 0; i < rlpp_len(pm->animation_states); i++) {
        rlr_pipeline_animated_model_animation_state_t* state = &pm->animation_states[i];

        rlr_pipeline_animated_model_draw_command_t* cmd = &pm->opaque_animated_model_commands[state->cmd_index];
        if(cmd->generation != state->cmd_generation) {
            continue;
        }

        float lerp_primary;
        float lerp_secondary;
        uint32_t pose_a_primary;
        uint32_t pose_b_primary;
        uint32_t pose_a_secondary;
        uint32_t pose_b_secondary;
        if(rlr_pipeline_update_animation_state(pm, cmd, &state->animation_states[RLR_OBJ_ANIMATION_PRIMARY], delta_time, &lerp_primary, &pose_a_primary, &pose_b_primary)) {
            cmd->instances[state->instance_index].lerp_primary = lerp_primary * UINT8_MAX;
            cmd->instances[state->instance_index].pose_a_offset_primary = pose_a_primary;
            cmd->instances[state->instance_index].pose_b_offset_primary = pose_b_primary;
        }
        if(rlr_pipeline_update_animation_state(pm, cmd, &state->animation_states[RLR_OBJ_ANIMATION_SECONDARY], delta_time, &lerp_secondary, &pose_a_secondary, &pose_b_secondary)) {
            cmd->instances[state->instance_index].lerp_secondary = lerp_secondary * UINT8_MAX;
            cmd->instances[state->instance_index].pose_a_offset_secondary = pose_a_secondary;
            cmd->instances[state->instance_index].pose_b_offset_secondary = pose_b_secondary;
        }

        uint16_t transition_amount = 0;
        state->current_transition_time += delta_time;
        if(state->transition_time > 0.0) {
            transition_amount = (1.0 - rlr_smoothstep(rlr_clampf(state->current_transition_time, 0.0, state->transition_time) / state->transition_time)) * UINT16_MAX;
        }
        
        cmd->instances[state->instance_index].transition_lerp = transition_amount;
        cmd->dirty = true;
    }
}

static inline void rlr_pipeline_get_animation(rlr_pipeline_animated_model_instance_t* inst, rlr_res_animation_sample_t* samples, size_t joint, rlr_quat_t* out_q, rlr_vec3_t* out_v) {
    const float UINT8_MAX_RATIO = 1.0 / 255.0;
    const float UINT16_MAX_RATIO = 1.0 / 65535.0;

    rlr_res_animation_sample_t* s_prim1 = &samples[inst->pose_a_offset_primary + joint];
    rlr_res_animation_sample_t* s_prim2 = &samples[inst->pose_b_offset_primary + joint];
    rlr_res_animation_sample_t* s_sec1 = &samples[inst->pose_a_offset_secondary + joint];
    rlr_res_animation_sample_t* s_sec2 = &samples[inst->pose_b_offset_secondary + joint];

    float lerp_prim = inst->lerp_primary * UINT8_MAX_RATIO;
    float lerp_sec = inst->lerp_secondary * UINT8_MAX_RATIO;
    float lerp_trans = inst->transition_lerp * UINT16_MAX_RATIO;

    rlr_quat_t prim_q = rlr_quat_slerp(&s_prim1->rotation, &s_prim2->rotation, lerp_prim);
    rlr_vec3_t prim_v = rlr_vec3_lerp(s_prim1->translation, s_prim2->translation, lerp_prim);

    if(inst->transition_lerp > 0) {
        rlr_quat_t sec_q = rlr_quat_slerp(&s_sec1->rotation, &s_sec2->rotation, lerp_sec);     
        rlr_vec3_t sec_v = rlr_vec3_lerp(s_sec1->translation, s_sec2->translation, lerp_sec);
        *out_q = rlr_quat_slerp(&prim_q, &sec_q, lerp_trans);
        *out_v = rlr_vec3_lerp(prim_v, sec_v, lerp_trans);
    } else {
        *out_q = prim_q;
        *out_v = prim_v;
    }
}

static inline void rlr_pipeline_update_animation_ubos(rlr_pipeline_animated_model_draw_command_t* cmd, uint64_t joint_count, size_t instance_offset, size_t instance_count) {
    static rlr_uniform_bone_pos_t pos = {0};
    static rlr_uniform_bone_quat_t rot = {0};

    for(size_t i = 0; i < instance_count; i++) {
        rlr_pipeline_animated_model_instance_t* inst = &cmd->instances[instance_offset + i];

        //update joints
        size_t offset = joint_count * i;
        for(size_t joint = 0; joint < joint_count; joint++) {
            rlr_quat_t q;
            rlr_vec3_t v;
            rlr_pipeline_get_animation(inst, cmd->model->animations.samples, joint, &rot.quat[offset + joint], &pos.vecs[offset + joint].vec);
        }
    }

    rlr_res_uniform_update(rlr()->ubos[RLR_INTERNAL_UBO_BONE_TRANSLATION], 0, pos.vecs, sizeof(pos.vecs));
    rlr_res_uniform_update(rlr()->ubos[RLR_INTERNAL_UBO_BONE_ROTATION], 0, rot.quat, sizeof(rot.quat));
}

static inline void rlr_pipeline_update_animated_model_instance_vbo(rlr_pipeline_animated_model_draw_command_t* cmd, size_t instance_offset, size_t instance_count) {
    rlr_backend()->bind_buffer(cmd->instance_vbo, RLR_BACKEND_BUFFER_ARRAY);
    rlr_backend()->update_buffer(RLR_BACKEND_BUFFER_ARRAY, sizeof(rlr_pipeline_animated_model_instance_t) * instance_count, cmd->instances + instance_offset, RLR_BACKEND_BUFFER_USAGE_DYNAMIC);
}

static inline void rlr_pipeline_draw_cpu_prepared_animated_model(rlr_pipeline_animated_model_draw_command_t* cmd, uint64_t joint_count, size_t instance_offset, size_t instance_count) {

    //prepare ubo and instance ebo
    rlr_pipeline_update_animation_ubos(cmd, joint_count, instance_offset, instance_count);
    rlr_pipeline_update_animated_model_instance_vbo(cmd, instance_offset, instance_count);

    //draw each mesh
    for(int64_t j = 0; j < arrlen(cmd->mesh_vaos); j++) {
        rlr_res_animated_mesh_t* mesh = &cmd->model->meshes[j];
        rlr_res_uniform_bind(mesh->material_ubo, RLR_INTERNAL_UBO_MATERIAL);
        rlr_backend()->bind_texture(cmd->model->animations.animation_texture, RLR_BACKEND_TEXTURE_2D, 1);
        if(mesh->texture_base) {
            rlr_res_texture_bind(mesh->texture_base, 0);
        } else {
            rlr_res_texture_bind(rlr()->texture_white, 0);
        }

        rlr_backend()->bind_vertex_array(cmd->mesh_vaos[j]);
        rlr_backend()->draw_elements_instanced(0, mesh->index_count, RLR_BACKEND_BUFFER_TYPE_U32, arrlenu(cmd->instances));
    }
}

void rlr_pipeline_model_prepare_animated_models_cpu(rlr_pipeline_model_t* pm, double delta_time) {
    rlr_pipeline_update_animations(pm, delta_time);
}

void rlr_pipeline_model_prepare_animated_models_gpu(rlr_pipeline_model_t* pm, double delta_time) {
    rlr_pipeline_update_animations(pm, delta_time);

    for(int64_t i = 0; i < arrlen(pm->opaque_animated_model_commands); i++) {
        rlr_pipeline_animated_model_draw_command_t* cmd = &pm->opaque_animated_model_commands[i];
        if(cmd->dirty) {
            rlr_pipeline_update_animated_model_instance_vbo(cmd, 0, arrlenu(cmd->instances));
            cmd->dirty = false;
        }
    }
}

void rlr_pipeline_model_draw_animated_models_cpu(rlr_pipeline_model_t* pm) {
    for(int64_t i = 0; i < arrlen(pm->opaque_animated_model_commands); i++) {
        rlr_pipeline_animated_model_draw_command_t* command = &pm->opaque_animated_model_commands[i];
        rlr_res_shader_bind(pm->shader_opaque_animated_model_cpu);
        
        //upload joint count
        uint64_t joint_count = command->model->animations.joint_count;
        rlr_uniform_bone_info_t info = {
            .joint_count = (uint32_t)joint_count
        };
        rlr_res_uniform_update(rlr()->ubos[RLR_INTERNAL_UBO_BONE_INFO], 0, &info, sizeof(rlr_uniform_bone_info_t));

        //calculate the amount of poses that can fit inside the UBO
        size_t instance_per_batch = MAX_UBO_SIZE / (sizeof(float) * 4 * joint_count);

        //divide the instances into draw call batches to make sure
        //all the poses fit inside the UBO buffers
        size_t instance_count = arrlenu(command->instances);
        size_t batch_count = instance_count / instance_per_batch;
        size_t remainder_count = instance_count % instance_per_batch;
        for(size_t batch = 0; batch < batch_count; batch++) {
            rlr_pipeline_draw_cpu_prepared_animated_model(command, joint_count, batch * instance_per_batch, instance_per_batch);
        }
        if(remainder_count > 0) {
            rlr_pipeline_draw_cpu_prepared_animated_model(command, joint_count, batch_count * instance_per_batch, remainder_count);
        }
    }
}

void rlr_pipeline_model_draw_animated_models_gpu(rlr_pipeline_model_t* pm) {
    for(int64_t i = 0; i < arrlen(pm->opaque_animated_model_commands); i++) {
        rlr_pipeline_animated_model_draw_command_t* command = &pm->opaque_animated_model_commands[i];
        rlr_res_shader_bind(pm->shader_opaque_animated_model_gpu);

        for(int64_t j = 0; j < arrlen(command->mesh_vaos); j++) {
            rlr_res_animated_mesh_t* mesh = &command->model->meshes[j];
            rlr_res_uniform_bind(mesh->material_ubo, RLR_INTERNAL_UBO_MATERIAL);
            rlr_backend()->bind_texture(command->model->animations.animation_texture, RLR_BACKEND_TEXTURE_2D, 1);
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


void rlr_pipeline_model_draw(double delta_time) {
    rlr_pipeline_model_t* pm = RLR_PIPELINE_MODEL;

    //prepare animated models
    switch(pm->animated_model_path) {
        case RLR_PIPELINE_ANIMATED_MODEL_CPU:
            rlr_pipeline_model_prepare_animated_models_cpu(pm, delta_time);
            break;
        case RLR_PIPELINE_ANIMATED_MODEL_GPU:
        default:
            rlr_pipeline_model_prepare_animated_models_gpu(pm, delta_time);
            break;
    }

    //prepare static models
    for(int64_t i = 0; i < arrlen(pm->opaque_static_model_commands); i++) {
        rlr_pipeline_static_model_draw_command_t* cmd = &pm->opaque_static_model_commands[i];
        if(cmd->dirty) {
            rlr_backend()->bind_buffer(cmd->instance_vbo, RLR_BACKEND_BUFFER_ARRAY);
            rlr_backend()->update_buffer(RLR_BACKEND_BUFFER_ARRAY, sizeof(rlr_pipeline_static_model_instance_t) * arrlenu(cmd->instances), cmd->instances, RLR_BACKEND_BUFFER_USAGE_DYNAMIC);
            cmd->dirty = false;
        }
    }

    //draw animated models
    switch(pm->animated_model_path) {
        case RLR_PIPELINE_ANIMATED_MODEL_CPU:
            rlr_pipeline_model_draw_animated_models_cpu(pm);
            break;
        case RLR_PIPELINE_ANIMATED_MODEL_GPU:
        default:
            rlr_pipeline_model_draw_animated_models_gpu(pm);
            break;
    }

    //draw static models
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
    arrfree(pm->opaque_animated_model_commands);
    rlr_res_shader_free(pm->shader_opaque_static_model);
    rlr_res_shader_free(pm->shader_opaque_animated_model_gpu);
    rlr_res_shader_free(pm->shader_opaque_animated_model_cpu);
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

    uint64_t new_index = arrlenu(pm->opaque_static_model_commands);
    rlr_pipeline_static_model_draw_command_t new_cmd = rlr_pipeline_model_create_static_model_draw_command(model, shader, new_index);
    arrpush(pm->opaque_static_model_commands, new_cmd);
    return &arrlast(pm->opaque_static_model_commands);
}

rlr_pipeline_animated_model_draw_command_t* rlr_pipeline_model_find_animated_model_draw_command(rlr_res_animated_model_t* model, rlr_res_shader_t* shader) {
    rlr_pipeline_model_t* pm = RLR_PIPELINE_MODEL;

    //todo: use binary search instead
    for(int64_t i = 0; i < arrlen(pm->opaque_animated_model_commands); i++) {
        rlr_pipeline_animated_model_draw_command_t* cmd = &pm->opaque_animated_model_commands[i];
        if(cmd->model == model && cmd->shader == shader) {
            return cmd;
        }
    }
    
    uint64_t new_index = arrlenu(pm->opaque_animated_model_commands);
    rlr_pipeline_animated_model_draw_command_t new_cmd = rlr_pipeline_model_create_animated_model_draw_command(model, shader, new_index);
    arrpush(pm->opaque_animated_model_commands, new_cmd);
    return &arrlast(pm->opaque_animated_model_commands);
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

uint32_t rlr_pipeline_model_add_animated_model_instance(rlr_pipeline_animated_model_draw_command_t* command, rlr_pipeline_animated_model_instance_t data) {
    arrpush(command->instances, data);
    command->dirty = true;
    return arrlen(command->instances) - 1;
}

bool rlpp_pipeline_model_set_animated_model_animation_state(rlr_obj_animated_model_t* model) {
    rlr_pipeline_model_t* pm = RLR_PIPELINE_MODEL;
    model->animation_state_ref = rlpp_alloc_to_ref(pm->animation_states, ((rlr_pipeline_animated_model_animation_state_t){
        .animation_states = {
            {.animation_index = -1, .animation_loop = false, .animation_speed = 1.0, .animation_time = 0.0 },
            {.animation_index = -1, .animation_loop = false, .animation_speed = 1.0, .animation_time = 0.0 },
        },
        .cmd_generation = model->cmd_generation,
        .cmd_index = model->cmd_index,
        .instance_index = model->instance_index,
        .current_transition_time = 0.0,
        .transition_time = 0.0,
    }));
    rlr_pipeline_animated_model_animation_state_t* state = rlpp_deref(pm->animation_states, model->animation_state_ref);
    return state != NULL;
}

void rlr_pipeline_model_update_animated_model_instance(uint32_t cmd_index, uint32_t cmd_generation, uint32_t instance_index, rlr_pipeline_animated_model_instance_t data) {
    rlr_pipeline_model_t* pm = RLR_PIPELINE_MODEL;

    if(cmd_index > arrlenu(pm->opaque_animated_model_commands) - 1) {
        return;
    }
    rlr_pipeline_animated_model_draw_command_t* cmd = &pm->opaque_animated_model_commands[cmd_index];
    if(cmd->generation != cmd_generation) {
        return;
    }

    cmd->instances[instance_index] = data;
    cmd->dirty = true;
}

void rlr_pipeline_model_swap_animation_states(uint32_t cmd_index, uint32_t cmd_generation, uint32_t instance_index) {
    rlr_pipeline_model_t* pm = RLR_PIPELINE_MODEL;

    if(cmd_index > arrlenu(pm->opaque_animated_model_commands) - 1) {
        return;
    }
    rlr_pipeline_animated_model_draw_command_t* cmd = &pm->opaque_animated_model_commands[cmd_index];
    if(cmd->generation != cmd_generation) {
        return;
    }
    cmd->instances[instance_index].lerp_secondary = cmd->instances[instance_index].lerp_primary;
    cmd->instances[instance_index].pose_a_offset_secondary = cmd->instances[instance_index].pose_a_offset_primary;
    cmd->instances[instance_index].pose_b_offset_secondary = cmd->instances[instance_index].pose_b_offset_primary;
    cmd->dirty = true;
}