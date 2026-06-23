#include <stdlib.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <stb_ds.h>
#include "internal/backends/backend.h"
#include "internal/core/backend_selection.h"
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
#include "rlr.h"

typedef struct uniform_model_t {
    rlr_mat4_t model;
    rlr_mat4_t mvp;
    rlr_vec3_t camera_pos;
} uniform_model_t;

typedef struct uniform_environment_t {
    rlr_vec3_t light_direction;
    rlr_vec3_t ambient_light_color;
    float ambient_light_strength;
} uniform_environment_t;

static rlr_t* _rlr = NULL;

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

    _rlr->framebuffer_width = window_width;
    _rlr->framebuffer_height = window_height;

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
    
    rlr_backend()->set_viewport(0, 0, window_width, window_height);
    
    //setup default resources
    _rlr->texture_white = rlr_res_texture_default();

    //setup pipelines
    if(!rlr_pipeline_ui_init(&_rlr->pipeline_ui)) {
        goto err;
    }
    if(!rlr_pipeline_stencil_init(&_rlr->pipeline_stencil)) {
        goto err;
    }
    rlr_pipeline_ui_set_viewport(window_width, window_height);

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

    _rlr->shader_model = rlr_res_create_shader(model_vertex, model_fragment);
    rlr_res_shader_bind_uniform_slot(_rlr->shader_model, "ubo_model", 1);
    rlr_res_shader_bind_uniform_slot(_rlr->shader_model, "ubo_material", 2);
    rlr_res_bind_shader_texture_slot(_rlr->shader_model, "tex", 0);
    rlr_res_bind_shader_texture_slot(_rlr->shader_model, "cube_map", 4);

    _rlr->ubo_material = rlr_res_uniform_create_dynamic(sizeof(rlr_res_material_t));
    rlr_res_uniform_bind(_rlr->ubo_material, 2);

    rlr_vec3_t cam_pos = rlr_vec3_mulf(rlr_vec3(-0.05, 0.1, -0.1), 7);
    rlr_vec3_t scene_center = rlr_vec3(0, 0, 0);
    rlr_vec3_t up = rlr_vec3(0, 1, 0);
    rlr_mat4_t projection = rlr_mat4_perspective(1, (float)window_width / (float)window_height, 0.001, 100.0);
    rlr_mat4_t view = rlr_mat4_look_at(&cam_pos, &scene_center, &up);
    uniform_model_t ubo_model = {
        .mvp = rlr_mat4_mul(&projection, &view),
        .model = rlr_mat4_ident,
        .camera_pos = cam_pos,
    };

    _rlr->ubo_model = rlr_res_uniform_create_dynamic(sizeof(uniform_model_t));
    rlr_res_uniform_update(_rlr->ubo_model, 0, &ubo_model, sizeof(uniform_model_t));
    rlr_res_uniform_bind(_rlr->ubo_model, 1);

    return;
err:
    rlr_free();
    return;
}

rlr_statistics_t* rlr_get_total_statistics() {
    return &_rlr->statistics_total;
}

rlr_statistics_t* rlr_get_statistics() {
    return &_rlr->statistics_interval;
}

const char* rlr_get_backend_implementation() {
    return rlr_backend()->get_implementation();
}

void rlr_free() {
    if(!_rlr) {
        return;
    }

    //free pipelines
    rlr_pipeline_ui_deinit(&_rlr->pipeline_ui);
    rlr_pipeline_stencil_deinit(&_rlr->pipeline_stencil);

    //free resources
    rlr_res_free_texture(_rlr->texture_white);
    rlr_res_uniform_free(_rlr->ubo_material);
    rlr_res_uniform_free(_rlr->ubo_model);
    rlr_res_free_shader(_rlr->shader_model);

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

double rlr_get_time() {
    return glfwGetTime();
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
        rlr_pipeline_ui_set_viewport(width, height);
        rlr_backend()->set_viewport(0, 0, width, height);
    }

    rlr_backend()->set_clear_color(0.1, 0.2, 0.3, 1.0);
    rlr_backend()->set_clear_stencil(0);
    rlr_backend()->clear(RLR_BACKEND_CLEAR_BIT_COLOR | RLR_BACKEND_CLEAR_BIT_DEPTH | RLR_BACKEND_CLEAR_BIT_STENCIL);

    rlr_pipeline_stencil_draw(&_rlr->pipeline_stencil);

    //render test monkey
    rlr_res_bind_shader(_rlr->shader_model);
    rlr_backend()->set_depth_test(true);
    for(int64_t i = 0; i < arrlen(_rlr->test->meshes); i++) {
        rlr_res_static_mesh_t* mesh = &_rlr->test->meshes[i];
        rlr_res_uniform_update(_rlr->ubo_material, 0, &mesh->material, sizeof(rlr_res_material_t));
        if(mesh->texture_base) {
            rlr_res_bind_texture(mesh->texture_base, 0);
        } else {
            rlr_res_bind_texture(_rlr->texture_white, 0);
        }
        rlr_backend()->bind_vertex_array(mesh->vao);
        rlr_backend()->draw_elements(0, mesh->index_count, RLR_BACKEND_BUFFER_TYPE_U32);
    }

    rlr_pipeline_ui_draw(&_rlr->pipeline_ui);

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

rlr_res_texture_t* rlr_internal_get_white_texture() {
    return _rlr->texture_white;
}

rlr_pipeline_stencil_t* rlr_internal_get_stencil_pipeline() {
    return &_rlr->pipeline_stencil;
}

rlr_pipeline_ui_t* rlr_internal_get_ui_pipeline() {
    return &_rlr->pipeline_ui;
}

rlr_backend_t* rlr_backend() {
    return _rlr->backend;
}