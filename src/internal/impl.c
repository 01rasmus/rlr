#include <stdlib.h>
#include <float.h>
#include <string.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include "../external/stb_ds.h"
#include "backends/backend_selection.h"
#include "backends/backend.h"
#include "../rlr/resources/static_model.h"
#include "../rlr/resources/model_shared.h"
#include "../rlr/resources/cube_map.h"
#include "../rlr/resources/uniform.h"
#include "../rlr/resources/shader.h"
#include "../rlr/resources/font.h"
#include "../rlr/objects/label.h"
#include "../rlr/objects/sprite.h"
#include "../rlr/math/matrix.h"
#include "../rlr/error.h"
#include "../rlr/rlr.h"
#include "impl.h"

static rlr_t* ctx = NULL;

rlr_vec2_t rlr_quad_vertices[4] = {
    rlr_vec2(0, 0),
    rlr_vec2(1, 0),
    rlr_vec2(1, 1),
    rlr_vec2(0, 1)
};

uint8_t rlr_quad_indices[6] = {
    2, 1, 0, 3, 2, 0
};

static void _rlr_key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if(ctx->callback_key_input) {
        ctx->callback_key_input(key, scancode, action, mods, ctx->user);
    }
}

static void _rlr_mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    if(ctx->callback_mouse_input) {
        ctx->callback_mouse_input(button, action, mods, rlr_get_mouse_position(), ctx->user);
    }
}

void rlr_init(const char* title, uint32_t window_width, uint32_t window_height, rlr_init_flags_t flags) {
    ctx = malloc(sizeof(rlr_t));
    if(!ctx) {
        rlr_error_set(RLR_ERR_NO_MEMORY);
        goto err;
    }

    (*ctx) = (rlr_t){0};
    ctx->backend = NULL;
    ctx->window = NULL;

    ctx->statistics_interval = (rlr_statistics_t){0};
    ctx->statistics_total = (rlr_statistics_t){0};
    ctx->statistics_temp = (rlr_statistics_t){0};
    ctx->statistics_timer = 0.0;

    if(!glfwInit()) {
        rlr_error_set(RLR_ERR_WINDOW_CREATION);
        goto err;
    }

    //dynamic backend selection
    GLFWmonitor* monitor = ((RLR_INIT_FLAG_FULLSCREEN & flags) == RLR_INIT_FLAG_FULLSCREEN) ? glfwGetPrimaryMonitor() : NULL;
    if(!rlr_internal_backend_selection(&ctx->window, &ctx->backend, monitor, window_width, window_height, title)) {
        rlr_error_set(RLR_ERR_COULD_NOT_FIND_SUITABLE_BACKEND);
        goto err;
    }
    glfwSwapInterval((RLR_INIT_FLAG_VSYNC & flags) == RLR_INIT_FLAG_VSYNC ? 1 : 0);
    glfwSetMouseButtonCallback(ctx->window, _rlr_mouse_button_callback);
    glfwSetKeyCallback(ctx->window, _rlr_key_callback);

    //setup resource manager
    if(!rlr_mem_man_init(&ctx->res_man)) {
        rlr_error_set(RLR_ERR_COULD_NOT_FIND_SUITABLE_BACKEND);
        goto err;
    }

    //setup uniform buffer objects
    rlr()->ubos[RLR_INTERNAL_UBO_MODEL]         = rlr_res_uniform_create_dynamic(sizeof(rlr_uniform_model_t));
    rlr()->ubos[RLR_INTERNAL_UBO_MATERIAL]      = rlr_res_uniform_create_dynamic(sizeof(rlr_uniform_material_t));
    rlr()->ubos[RLR_INTERNAL_UBO_ENVIRONMENT]   = rlr_res_uniform_create_dynamic(sizeof(rlr_uniform_environment_t));
    rlr()->ubos[RLR_INTERNAL_UBO_UI]            = rlr_res_uniform_create_dynamic(sizeof(rlr_uniform_ui_t));
    for(size_t i = 0; i < RLR_INTERNAL_UBO_COUNT; i++) {
        rlr_res_t ubo = rlr()->ubos[i];
        if(ubo == RLR_NULL) {
            goto err;
        }
        rlr_res_uniform_bind(ubo, i);
    }

    //setup pipelines
    static const rlr_pipeline_init_function_t pipeline_init_functions[3] = {
        rlr_pipeline_ui_init,
        rlr_pipeline_stencil_init,
        rlr_pipeline_model_init,
    };
    for(size_t i = 0; i < sizeof(pipeline_init_functions) / sizeof(pipeline_init_functions[0]); i++) {
        rlr_pipeline_init_function_t pipeline_init = pipeline_init_functions[i];
        if(!pipeline_init()) {
            goto err;
        }
    }

    //setup default values
    rlr_backend()->set_clear_color(0.0, 0.0, 0.0, 1.0);
    ctx->last_time = glfwGetTime();

    //setup default resources
    ctx->texture_white = rlr_res_texture_default();
    ctx->cube_map_white = rlr_res_cube_map_default();
    rlr_res_cube_map_bind(ctx->cube_map_white, 4);
    return;
err:
    rlr_free();
    return;
}

rlr_t* rlr() {
    return ctx;
}

rlr_backend_t* rlr_backend() {
    return ctx->backend;
}

rlr_mem_man_t* rlr_mem_man() {
    return &ctx->res_man;
}

void rlr_set_camera(rlr_vec3_t pos, rlr_quat_t rotation) {
    const rlr_vec3_t up = rlr_vec3(0, 1, 0);
    rlr_mat4x4_t projection = rlr_mat4x4_perspective(1, (float)ctx->framebuffer_width / (float)ctx->framebuffer_height, 0.001, 1000.0);
    rlr_mat4x4_t view = rlr_mat4x4_look_towards_quat(&pos, &rotation);
    rlr_mat4x4_t view_projection = rlr_mat4x4_mul(&projection, &view);
    rlr_uniform_model_t ubo_model = {
        .vp = view_projection,
        .camera_pos = pos,
    };
    ctx->camera_pos = pos;
    ctx->camera_rot = rotation;
    ctx->view_projection = view_projection;
    rlr_res_uniform_update(ctx->ubos[RLR_INTERNAL_UBO_MODEL], 0, &ubo_model, sizeof(rlr_uniform_model_t));
}

rlr_vec3_t rlr_get_camera_pos() {
    return ctx->camera_pos;
}

rlr_quat_t rlr_get_camera_rot() {
    return ctx->camera_rot;
}

void rlr_set_clear_color(float r, float g, float b, float a) {
    rlr_backend()->set_clear_color(r, g, b, a);
}

void rlr_set_cube_map(rlr_res_t cube_map_id) {
    rlr_res_t cube_map = cube_map_id == RLR_NULL ? ctx->cube_map_white : cube_map_id;
    rlr_res_cube_map_bind(cube_map, 4);
}

rlr_vec2_t rlr_get_framebuffer_size() {
    return rlr_vec2(ctx->framebuffer_width, ctx->framebuffer_height);
}

bool rlr_screen_pos_to_ground(rlr_vec2_t mouse_pos, rlr_vec3_t* out_position) {
    float ndc_x = (2.0f * mouse_pos.x) / ctx->framebuffer_width - 1.0f;
    float ndc_y = 1.0f - (2.0f * mouse_pos.y) / ctx->framebuffer_height;

    rlr_vec4_t near_clip = {
        .x = ndc_x,
        .y = ndc_y,
        .z = -1.0f,
        .w = 1.0,
    };
    rlr_vec4_t far_clip = {
        .x = ndc_x,
        .y = ndc_y,
        .z = 1.0f,
        .w = 1.0,
    };

    rlr_mat4x4_t inverse_view_projection = rlr_mat4x4_inverse(&ctx->view_projection);
    rlr_vec4_t near_world = rlr_mat4x4_mul_vec4(&inverse_view_projection, &near_clip);
    rlr_vec4_t far_world = rlr_mat4x4_mul_vec4(&inverse_view_projection, &far_clip);

    near_world.x /= near_world.w;
    near_world.y /= near_world.w;
    near_world.z /= near_world.w;

    far_world.x /= far_world.w;
    far_world.y /= far_world.w;
    far_world.z /= far_world.w;

    rlr_vec3_t direction = {
        .x = far_world.x - near_world.x,
        .y = far_world.y - near_world.y,
        .z = far_world.z - near_world.z
    };

    if(fabsf(direction.y) < 0.000001f) {
        return false;
    }
    float t = -near_world.y / direction.y;
    if(t < 0.0f) {
        return false;
    }

    out_position->x = near_world.x + direction.x * t;
    out_position->y = 0.0f;
    out_position->z = near_world.z + direction.z * t;
    return true;
}

rlr_vec2_t rlr_world_to_screen(rlr_vec3_t world_pos) {
    rlr_vec4_t clip = rlr_mat4x4_mul_vec4(&ctx->view_projection, &(rlr_vec4_t){
        .x = world_pos.x,
        .y = world_pos.y,
        .z = world_pos.z,
        .w = 1.0,
    });
    
    if(clip.w <= 0.0) {
        return rlr_vec2(-FLT_MAX, -FLT_MAX);
    }

    float inv_w = 1.0 / clip.w;
    float ndc_x = clip.x * inv_w;
    float ndc_y = clip.y * inv_w;
    return rlr_vec2((ndc_x * 0.5 + 0.5) * ctx->framebuffer_width, (1.0 - (ndc_y * 0.5 + 0.5)) * ctx->framebuffer_height);
}

void rlr_set_user(void* user) {
    ctx->user = user;
}

void rlr_set_mouse_input_callback(rlr_input_mouse_callback_t func) {
    ctx->callback_mouse_input = func;
}

void rlr_set_key_input_callback(rlr_input_key_callback_t func) {
    ctx->callback_key_input = func;
}

rlr_vec2_t rlr_get_mouse_position() {
    double x;
    double y;
    glfwGetCursorPos(ctx->window, &x, &y);
    return (rlr_vec2_t){.x = x, .y = y};
}

rlr_statistics_t* rlr_get_total_statistics() {
    return &ctx->statistics_total;
}

rlr_statistics_t* rlr_get_statistics() {
    return &ctx->statistics_interval;
}

rlr_res_t rlr_internal_get_white_texture() {
    return ctx->texture_white;
}

const char* rlr_get_gpu_name() {
    return rlr_backend()->get_gpu_name();
}

const char* rlr_get_backend_context() {
    return rlr_backend()->get_context_version();
}

const char* rlr_get_backend_implementation() {
    return rlr_backend()->get_implementation();
}

bool rlr_update() {
    glfwPollEvents();

    if(glfwWindowShouldClose(ctx->window)) {
        return false;
    }

    double current_time = glfwGetTime();
    double delta_time = current_time - ctx->last_time;
    ctx->last_time = current_time;

    int32_t width = 0;
    int32_t height = 0;
    glfwGetFramebufferSize(ctx->window, &width, &height);
    if(width != ctx->framebuffer_width || height != ctx->framebuffer_height) {
        rlr_uniform_ui_t ui_uniform = (rlr_uniform_ui_t){
            .inv_x = 1.0 / (float)width,
            .inv_y = 1.0 / (float)height,
            .screen_width = (float)width,
            .screen_height = (float)height
        };
        ctx->framebuffer_width = width;
        ctx->framebuffer_height = height;
        rlr_res_uniform_update(ctx->ubos[RLR_INTERNAL_UBO_UI], 0, &ui_uniform, sizeof(rlr_uniform_ui_t));
        rlr_backend()->set_viewport(0, 0, width, height);
        rlr_set_camera(ctx->camera_pos, ctx->camera_rot);
    }

    rlr_backend()->set_clear_stencil(0);
    rlr_backend()->set_stencil_mask(0xff);
    rlr_backend()->clear(RLR_BACKEND_CLEAR_BIT_COLOR | RLR_BACKEND_CLEAR_BIT_DEPTH | RLR_BACKEND_CLEAR_BIT_STENCIL);

    rlr_pipeline_stencil_draw();
    rlr_pipeline_model_draw(delta_time);
    rlr_pipeline_ui_draw();

    //statistics
    uint64_t draw_calls = rlr_backend()->get_draw_call_count();

    //temp
    ctx->statistics_temp.draw_call_count += draw_calls;
    ctx->statistics_temp.frame_count++;
    ctx->statistics_temp.time = current_time;

    //total
    ctx->statistics_total.draw_call_count += draw_calls;
    ctx->statistics_total.frame_count++;
    ctx->statistics_total.time = current_time;

    if(ctx->statistics_timer <= current_time) {
        memcpy(&ctx->statistics_interval, &ctx->statistics_temp, sizeof(rlr_statistics_t));
        ctx->statistics_temp = (rlr_statistics_t){0};
        ctx->statistics_timer += 1.0;
    }
    rlr_backend()->reset_statistics();

    glfwSwapBuffers(ctx->window);
    return true;
}

void rlr_free() {
    if(!ctx) {
        return;
    }

    //free pipelines
    rlr_pipeline_ui_deinit();
    rlr_pipeline_stencil_deinit();
    rlr_pipeline_model_deinit();

    //free resources
    rlr_res_texture_free(ctx->texture_white);

    //free ubos
    for(size_t i = 0; i < RLR_INTERNAL_UBO_COUNT; i++) {
        rlr_res_uniform_free(rlr()->ubos[i]);
    }

    //free backend API and window
    if(ctx->backend) {
        rlr_backend()->free_backend();
        free(ctx->backend);
    }
    if(ctx->window) {
        glfwDestroyWindow(ctx->window);
    }

    //free resource manager
    rlr_mem_man_deinit(&ctx->res_man);

    free(ctx);
    glfwTerminate();
    ctx = NULL;
}