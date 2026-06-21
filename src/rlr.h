#pragma once
#include <stdbool.h>
#include <stdint.h>
#include "pipelines/stencil.h"
#include "pipelines/ui.h"

typedef struct rlr_backend_t rlr_backend_t;
typedef struct GLFWwindow GLFWwindow;

typedef struct rlr_res_shader_t rlr_res_shader_t;
typedef struct rlr_res_texture_t rlr_res_texture_t;
typedef struct rlr_res_uniform_t rlr_res_uniform_t;
typedef struct rlr_res_cube_map_t rlr_res_cube_map_t;
typedef struct rlr_res_static_model_t rlr_res_static_model_t;

typedef struct rlr_t {
    GLFWwindow* window;
    rlr_backend_t* backend;

    int32_t framebuffer_width;
    int32_t framebuffer_height;

    rlr_res_static_model_t* test;
    rlr_res_cube_map_t* test_cube_map;

    //pipelines
    rlr_pipeline_stencil_t pipeline_stencil;
    rlr_pipeline_ui_t pipeline_ui;

    //built in resources
    rlr_res_texture_t* texture_white;
    rlr_res_shader_t* shader_model;
    rlr_res_uniform_t* ubo_model;
    rlr_res_uniform_t* ubo_material;
} rlr_t;

void rlr_init(const char* title, uint32_t window_width, uint32_t window_height, uint64_t flags);
double rlr_time();
rlr_backend_t* rlr_backend();
bool rlr_draw();
void rlr_free();

/*
    internal context fetching function
*/
rlr_t* _rlr_raw();