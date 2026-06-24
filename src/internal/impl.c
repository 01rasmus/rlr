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

static rlr_t* ctx = NULL;

rlr_vec2_t rlr_quad_vertices[6] = {
    rlr_vec2(1, 1),
    rlr_vec2(1, 0),
    rlr_vec2(0, 0),
    rlr_vec2(0, 1),
    rlr_vec2(1, 1),
    rlr_vec2(0, 0)
};

void rlr_init(const char* title, uint32_t window_width, uint32_t window_height, uint64_t flags) {
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

    //setup default resources
    ctx->texture_white = rlr_res_texture_default();

    // todo: remove
    //_rlr->test_cube_map = rlr_res_cube_map_load("assets/skybox/right.jpg", "assets/skybox/left.jpg", "assets/skybox/top.jpg", "assets/skybox/bottom.jpg", "assets/skybox/front.jpg", "assets/skybox/back.jpg");
    ctx->test_cube_map = rlr_res_cube_map_load(
        "assets/s/px.png",
        "assets/s/nx.png",
        "assets/s/py.png",
        "assets/s/ny.png",
        "assets/s/pz.png",
        "assets/s/nz.png"
    );
    rlr_res_cube_map_bind(ctx->test_cube_map, 4);
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

rlr_statistics_t* rlr_get_total_statistics() {
    return &ctx->statistics_total;
}

rlr_statistics_t* rlr_get_statistics() {
    return &ctx->statistics_interval;
}

rlr_res_texture_t* rlr_internal_get_white_texture() {
    return ctx->texture_white;
}

const char* rlr_get_backend_implementation() {
    return rlr_backend()->get_implementation();
}

bool rlr_update() {
    glfwPollEvents();

    if(glfwWindowShouldClose(ctx->window)) {
        return false;
    }

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
        rlr_res_uniform_update(ctx->ubos[RLR_INTERNAL_UBO_UI], 0, &ui_uniform, sizeof(rlr_uniform_ui_t));
        rlr_backend()->set_viewport(0, 0, width, height);

        rlr_vec3_t cam_pos = rlr_vec3_mulf(rlr_vec3(-0.05, 0.1, -0.1), 7);
        rlr_vec3_t scene_center = rlr_vec3(0, 0, 0);
        rlr_vec3_t up = rlr_vec3(0, 1, 0);
        rlr_mat4_t projection = rlr_mat4_perspective(1, (float)width / (float)height, 0.001, 100.0);
        rlr_mat4_t view = rlr_mat4_look_at(&cam_pos, &scene_center, &up);
        rlr_uniform_model_t ubo_model = {
            .vp = rlr_mat4_mul(&projection, &view),
            .camera_pos = cam_pos,
        };
        rlr_res_uniform_update(ctx->ubos[RLR_INTERNAL_UBO_MODEL], 0, &ubo_model, sizeof(rlr_uniform_model_t));
    }

    rlr_backend()->set_clear_color(0.1, 0.2, 0.3, 1.0);
    rlr_backend()->set_clear_stencil(0);
    rlr_backend()->clear(RLR_BACKEND_CLEAR_BIT_COLOR | RLR_BACKEND_CLEAR_BIT_DEPTH | RLR_BACKEND_CLEAR_BIT_STENCIL);

    rlr_pipeline_stencil_draw();
    rlr_pipeline_model_draw();
    rlr_pipeline_ui_draw();

    //statistics
    double current_time = glfwGetTime();
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
    free(ctx);
    glfwTerminate();
    ctx = NULL;
}