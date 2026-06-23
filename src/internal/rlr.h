#pragma once
#include <stdint.h>
#include "pipelines/stencil.h"
#include "pipelines/ui.h"
#include "rlr/rlr.h"

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

    //statistics
    rlr_statistics_t statistics_total;
    rlr_statistics_t statistics_interval;
    rlr_statistics_t statistics_temp;
    double statistics_timer;

    //pipelines
    rlr_pipeline_stencil_t pipeline_stencil;
    rlr_pipeline_ui_t pipeline_ui;

    //built in resources
    rlr_res_texture_t* texture_white;
    rlr_res_shader_t* shader_model;
    rlr_res_uniform_t* ubo_model;
    rlr_res_uniform_t* ubo_material;
} rlr_t;

rlr_backend_t* rlr_backend();
rlr_res_texture_t* rlr_internal_get_white_texture();
rlr_pipeline_stencil_t* rlr_internal_get_stencil_pipeline();
rlr_pipeline_ui_t* rlr_internal_get_ui_pipeline();