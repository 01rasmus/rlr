#pragma once
#include <stdint.h>
#include "pipelines/stencil.h"
#include "pipelines/model.h"
#include "pipelines/ui.h"
#include "core/memory_manager.h"
#include "rlr/math/matrix.h"
#include "rlr/rlr.h"

typedef struct rlr_backend_t rlr_backend_t;
typedef struct GLFWwindow GLFWwindow;

typedef struct rlr_res_shader_t rlr_res_shader_t;
typedef struct rlr_res_texture_t rlr_res_texture_t;
typedef struct rlr_res_uniform_t rlr_res_uniform_t;
typedef struct rlr_res_cube_map_t rlr_res_cube_map_t;
typedef struct rlr_res_material_t rlr_res_material_t;
typedef struct rlr_res_static_model_t rlr_res_static_model_t;

typedef bool (*rlr_pipeline_init_function_t)();

#define RLR_INTERNAL_UBO_COUNT                  4
#define RLR_INTERNAL_UBO_MODEL                  0x0
#define RLR_INTERNAL_UBO_MATERIAL               0x1
#define RLR_INTERNAL_UBO_ENVIRONMENT            0x2
#define RLR_INTERNAL_UBO_UI                     0x3

typedef struct rlr_uniform_model_t {
    rlr_mat4x4_t vp;
    rlr_vec3_t camera_pos;
} rlr_uniform_model_t;

typedef struct rlr_uniform_environment_t {
    rlr_vec3_t light_direction;
    rlr_vec3_t ambient_light_color;
    float ambient_light_strength;
} rlr_uniform_environment_t;

typedef struct rlr_uniform_ui_t {
    float inv_x;
    float inv_y;
    float screen_width;
    float screen_height;
} rlr_uniform_ui_t;

typedef rlr_res_material_t rlr_uniform_material_t;

typedef struct rlr_t {
    GLFWwindow* window;
    rlr_backend_t* backend;
    rlr_mem_man_t res_man;

    //configuration
    int32_t framebuffer_width;
    int32_t framebuffer_height;

    //pipelines
    rlr_pipeline_stencil_t pipeline_stencil;
    rlr_pipeline_model_t pipeline_model;
    rlr_pipeline_ui_t pipeline_ui;

    //built in resources
    rlr_res_texture_t* texture_white;
    rlr_res_uniform_t* ubos[RLR_INTERNAL_UBO_COUNT];

    //statistics
    rlr_statistics_t statistics_total;
    rlr_statistics_t statistics_interval;
    rlr_statistics_t statistics_temp;
    double statistics_timer;

    //misc (todo: remove and add a new model pipeline)
    rlr_res_cube_map_t* test_cube_map;
} rlr_t;

extern rlr_vec2_t rlr_quad_vertices[4];
extern uint8_t rlr_quad_indices[6];

rlr_t* rlr();
rlr_backend_t* rlr_backend();
rlr_mem_man_t* rlr_mem_man();
rlr_res_texture_t* rlr_internal_get_white_texture();