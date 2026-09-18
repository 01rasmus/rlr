#pragma once
#include <stdint.h>
#include "pipelines/stencil.h"
#include "pipelines/model.h"
#include "pipelines/input.h"
#include "pipelines/ui.h"
#include "../rlr/math/matrix.h"
#include "../rlr/math/quat.h"
#include "../rlr/rlr.h"

typedef struct rlr_backend_t rlr_backend_t;
typedef struct GLFWwindow GLFWwindow;

typedef struct rlr_res_material_t rlr_res_material_t;

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

    //configuration
    int32_t framebuffer_width;
    int32_t framebuffer_height;
    rlr_vec2_t mouse_pos;
    rlr_vec3_t camera_pos;
    rlr_quat_t camera_rot;
    rlr_mat4x4_t view_projection;

    //callbacks
    void* input_user;
    void* log_user;
    rlr_input_key_callback_t callback_key_input;
    rlr_input_mouse_callback_t callback_mouse_input;
    rlr_log_callback_t callback_log;

    //timing
    double last_time;

    //pipelines
    rlr_pipeline_stencil_t pipeline_stencil;
    rlr_pipeline_model_t pipeline_model;
    rlr_pipeline_input_t pipeline_input;
    rlr_pipeline_ui_t pipeline_ui;

    //built in resources
    rlr_res_texture_t* texture_white;
    rlr_res_cube_map_t* cube_map_white;
    rlr_res_uniform_t* ubos[RLR_INTERNAL_UBO_COUNT];

    //statistics
    rlr_statistics_t statistics_total;
    rlr_statistics_t statistics_interval;
    rlr_statistics_t statistics_temp;
    double statistics_timer;
} rlr_t;

extern rlr_vec2_t rlr_quad_vertices[4];
extern uint8_t rlr_quad_indices[6];

#define rlr_log(...) \
    _rlr_log(__FILE__, __LINE__, RLR_LOG_LEVEL_INFO, __VA_ARGS__)

#define rlr_log_warning(...) \
    _rlr_log(__FILE__, __LINE__, RLR_LOG_LEVEL_WARNING, __VA_ARGS__)

#define rlr_log_error(...) \
    _rlr_log(__FILE__, __LINE__, RLR_LOG_LEVEL_ERROR, __VA_ARGS__)

#define rlr_log_debug(...) \
    _rlr_log(__FILE__, __LINE__, RLR_LOG_LEVEL_DEBUG, __VA_ARGS__)

rlr_t* rlr();
rlr_backend_t* rlr_backend();
void _rlr_log(const char* file, uint64_t line, rlr_log_level_t log_level, const char* format, ...);