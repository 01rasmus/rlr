#include <stdlib.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <stb_ds.h>
#include "internal/backends/backend_selection.h"
#include "internal/backends/backend.h"
#include "rlr/resources/static_model.h"
#include "rlr/resources/cube_map.h"
#include "rlr/resources/uniform.h"
#include "rlr/resources/shader.h"
#include "rlr/resources/font.h"
#include "rlr/objects/label.h"
#include "rlr/objects/sprite.h"
#include "rlr/math/matrix.h"
#include "rlr/error.h"
#include "rlr/rlr.h"
#include "impl.h"

static rlr_t* _rlr = NULL;

rlr_vec2_t rlr_quad_vertices[6] = {
    rlr_vec2(1, 1),
    rlr_vec2(1, 0),
    rlr_vec2(0, 0),
    rlr_vec2(0, 1),
    rlr_vec2(1, 1),
    rlr_vec2(0, 0)
};

const char model_fragment[] = RLR_SHADER_INLINE(
    in vec2 frag_uv;
    in vec3 frag_normal;
    in vec3 frag_vert_pos;
    out vec4 out_color;

    uniform sampler2D tex;
    uniform samplerCube cube_map;

    layout(std140) uniform ubo_model {
        mat4 model;
        mat4 mvp;
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

        vec3 specular =
            light_col *
            spec *
            material.specular_strength *
            attenuation;

        return diffuse + specular;
    }

    void main() {
        // out_color = texture(tex, frag_uv);
        // for(int i = 0; i < 50; i++) {
        //     out_color *= vec4(texture(tex, frag_uv).rgb * model.camera_pos * material.color.rgb, 1.0);
        // }
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

        for(int i = 0; i < LIGHT_COUNT; i++) {
            lit_color += point_light(point_lights[i].position, point_lights[i].color, N, V, diffuse_color);
        }

        vec3 final_rgb = mix(lit_color, reflected_color, material.reflectiveness);
        out_color = vec4(final_rgb, tex_color.a * material.color.a);
    }
);

const char model_vertex[] = RLR_SHADER_INLINE(
    layout(location = 0) in vec3 pos;
    layout(location = 1) in vec3 normal;
    layout(location = 2) in vec2 uv;

    layout(std140) uniform ubo_model {
        mat4 model;
        mat4 mvp;
        vec3 camera_pos;
    } model;

    out vec2 frag_uv;
    out vec3 frag_normal;
    out vec3 frag_vert_pos;

    void main() {
        vec4 world_pos = model.model * vec4(pos, 1.0);
        frag_vert_pos = world_pos.xyz;
        frag_normal = mat3(transpose(inverse(model.model))) * normal;
        frag_uv = uv;
        gl_Position = model.mvp * vec4(pos, 1.0);
    }
);

void rlr_init(const char* title, uint32_t window_width, uint32_t window_height, uint64_t flags) {
    _rlr = malloc(sizeof(rlr_t));
    if(!_rlr) {
        rlr_error_set(RLR_ERR_NO_MEMORY);
        goto err;
    }

    (*_rlr) = (rlr_t){0};
    _rlr->backend = NULL;
    _rlr->window = NULL;

    _rlr->statistics_interval = (rlr_statistics_t){0};
    _rlr->statistics_total = (rlr_statistics_t){0};
    _rlr->statistics_temp = (rlr_statistics_t){0};
    _rlr->statistics_timer = 0.0;

    if(!glfwInit()) {
        rlr_error_set(RLR_ERR_WINDOW_CREATION);
        goto err;
    }

    //dynamic backend selection
    GLFWmonitor* monitor = ((RLR_INIT_FLAG_FULLSCREEN & flags) == RLR_INIT_FLAG_FULLSCREEN) ? glfwGetPrimaryMonitor() : NULL;
    if(!rlr_internal_backend_selection(&_rlr->window, &_rlr->backend, monitor, window_width, window_height, title)) {
        rlr_error_set(RLR_ERR_COULD_NOT_FIND_SUITABLE_BACKEND);
        goto err;
    }
    glfwSwapInterval((RLR_INIT_FLAG_VSYNC & flags) == RLR_INIT_FLAG_VSYNC ? 1 : 0);

    //setup uniform buffer objects
    rlr()->ubos[RLR_INTERNAL_UBO_MODEL]         = rlr_res_uniform_create_dynamic(sizeof(rlr_uniform_model_t));
    rlr()->ubos[RLR_INTERNAL_UBO_MATERIAL]      = rlr_res_uniform_create_dynamic(sizeof(rlr_uniform_material_t));
    rlr()->ubos[RLR_INTERNAL_UBO_ENVIRONMENT]   = rlr_res_uniform_create_dynamic(sizeof(rlr_uniform_environment_t));
    rlr()->ubos[RLR_INTERNAL_UBO_UI]            = rlr_res_uniform_create_dynamic(sizeof(rlr_uniform_ui_t));
    for(size_t i = 0; i < RLR_INTERNAL_UBO_COUNT; i++) {
        rlr_res_uniform_t* ubo = rlr()->ubos[i];
        if(!ubo) {
            goto err;
        }
        rlr_res_uniform_bind(ubo, i);
    }

    //setup pipelines
    static const rlr_pipeline_init_function_t pipeline_init_functions[2] = {
        rlr_pipeline_ui_init,
        rlr_pipeline_stencil_init
    };
    for(size_t i = 0; i < sizeof(pipeline_init_functions) / sizeof(pipeline_init_functions[0]); i++) {
        rlr_pipeline_init_function_t pipeline_init = pipeline_init_functions[i];
        if(!pipeline_init()) {
            goto err;
        }
    }

    //setup default resources
    _rlr->texture_white = rlr_res_texture_default();

    // todo: remove
    // testing
    _rlr->test = rlr_res_static_model_load("assets/plane.glb");
    //_rlr->test_cube_map = rlr_res_cube_map_load("assets/skybox/right.jpg", "assets/skybox/left.jpg", "assets/skybox/top.jpg", "assets/skybox/bottom.jpg", "assets/skybox/front.jpg", "assets/skybox/back.jpg");
    _rlr->test_cube_map = rlr_res_cube_map_load(
        "assets/s/px.png",
        "assets/s/nx.png",
        "assets/s/py.png",
        "assets/s/ny.png",
        "assets/s/pz.png",
        "assets/s/nz.png"
    );
    rlr_res_cube_map_bind(_rlr->test_cube_map, 4);

    _rlr->shader_model = rlr_res_shader_create(model_vertex, model_fragment);
    rlr_res_shader_bind_uniform_slot(_rlr->shader_model, "ubo_model", RLR_INTERNAL_UBO_MODEL);
    rlr_res_shader_bind_uniform_slot(_rlr->shader_model, "ubo_material", RLR_INTERNAL_UBO_MATERIAL);
    rlr_res_shader_bind_texture_slot(_rlr->shader_model, "tex", 0);
    rlr_res_shader_bind_texture_slot(_rlr->shader_model, "cube_map", 4);

    rlr_vec3_t cam_pos = rlr_vec3_mulf(rlr_vec3(-0.05, 0.1, -0.1), 7);
    rlr_vec3_t scene_center = rlr_vec3(0, 0, 0);
    rlr_vec3_t up = rlr_vec3(0, 1, 0);
    rlr_mat4_t projection = rlr_mat4_perspective(1, (float)window_width / (float)window_height, 0.001, 100.0);
    rlr_mat4_t view = rlr_mat4_look_at(&cam_pos, &scene_center, &up);
    rlr_uniform_model_t ubo_model = {
        .mvp = rlr_mat4_mul(&projection, &view),
        .model = rlr_mat4_ident,
        .camera_pos = cam_pos,
    };

    rlr_res_uniform_update(_rlr->ubos[RLR_INTERNAL_UBO_MODEL], 0, &ubo_model, sizeof(rlr_uniform_model_t));
    return;
err:
    rlr_free();
    return;
}

rlr_t* rlr() {
    return _rlr;
}

rlr_backend_t* rlr_backend() {
    return _rlr->backend;
}

rlr_statistics_t* rlr_get_total_statistics() {
    return &_rlr->statistics_total;
}

rlr_statistics_t* rlr_get_statistics() {
    return &_rlr->statistics_interval;
}

rlr_res_texture_t* rlr_internal_get_white_texture() {
    return _rlr->texture_white;
}

const char* rlr_get_backend_implementation() {
    return rlr_backend()->get_implementation();
}

bool rlr_update() {
    glfwPollEvents();

    if(glfwWindowShouldClose(_rlr->window)) {
        return false;
    }

    int32_t width = 0;
    int32_t height = 0;
    glfwGetFramebufferSize(_rlr->window, &width, &height);
    if(width != _rlr->framebuffer_width || height != _rlr->framebuffer_height) {
        rlr_uniform_ui_t ui_uniform = (rlr_uniform_ui_t){
            .inv_x = 1.0 / (float)width,
            .inv_y = 1.0 / (float)height,
            .screen_width = (float)width,
            .screen_height = (float)height
        };
        rlr_res_uniform_update(_rlr->ubos[RLR_INTERNAL_UBO_UI], 0, &ui_uniform, sizeof(rlr_uniform_ui_t));
        rlr_backend()->set_viewport(0, 0, width, height);
    }

    rlr_backend()->set_clear_color(0.1, 0.2, 0.3, 1.0);
    rlr_backend()->set_clear_stencil(0);
    rlr_backend()->clear(RLR_BACKEND_CLEAR_BIT_COLOR | RLR_BACKEND_CLEAR_BIT_DEPTH | RLR_BACKEND_CLEAR_BIT_STENCIL);

    rlr_pipeline_stencil_draw();

    //render test monkey
    rlr_res_shader_bind(_rlr->shader_model);
    rlr_backend()->set_depth_test(true);
    for(int64_t i = 0; i < arrlen(_rlr->test->meshes); i++) {
        rlr_res_static_mesh_t* mesh = &_rlr->test->meshes[i];
        rlr_res_uniform_update(_rlr->ubos[RLR_INTERNAL_UBO_MATERIAL], 0, &mesh->material, sizeof(rlr_uniform_material_t));
        if(mesh->texture_base) {
            rlr_res_texture_bind(mesh->texture_base, 0);
        } else {
            rlr_res_texture_bind(_rlr->texture_white, 0);
        }
        rlr_backend()->bind_vertex_array(mesh->vao);
        rlr_backend()->draw_elements(0, mesh->index_count, RLR_BACKEND_BUFFER_TYPE_U32);
    }

    rlr_pipeline_ui_draw();

    //statistics
    double current_time = glfwGetTime();
    uint64_t draw_calls = rlr_backend()->get_draw_call_count();

    //temp
    _rlr->statistics_temp.draw_call_count += draw_calls;
    _rlr->statistics_temp.frame_count++;
    _rlr->statistics_temp.time = current_time;

    //total
    _rlr->statistics_total.draw_call_count += draw_calls;
    _rlr->statistics_total.frame_count++;
    _rlr->statistics_total.time = current_time;

    if(_rlr->statistics_timer <= current_time) {
        memcpy(&_rlr->statistics_interval, &_rlr->statistics_temp, sizeof(rlr_statistics_t));
        _rlr->statistics_temp = (rlr_statistics_t){0};
        _rlr->statistics_timer += 1.0;
    }
    rlr_backend()->reset_statistics();

    glfwSwapBuffers(_rlr->window);
    return true;
}

void rlr_free() {
    if(!_rlr) {
        return;
    }

    //free pipelines
    rlr_pipeline_ui_deinit();
    rlr_pipeline_stencil_deinit();

    //free resources
    rlr_res_shader_free(_rlr->shader_model);
    rlr_res_texture_free(_rlr->texture_white);

    //free ubos
    for(size_t i = 0; i < RLR_INTERNAL_UBO_COUNT; i++) {
        rlr_res_uniform_free(rlr()->ubos[i]);
    }

    //free backend API and window
    if(_rlr->backend) {
        rlr_backend()->free_backend();
        free(_rlr->backend);
    }
    if(_rlr->window) {
        glfwDestroyWindow(_rlr->window);
    }
    free(_rlr);
    glfwTerminate();
    _rlr = NULL;
}