#pragma once
#include <stdbool.h>
#include <stdint.h>

typedef struct rlr_backend_t rlr_backend_t;
typedef struct GLFWwindow GLFWwindow;

typedef struct rlr_shader_t rlr_shader_t;
typedef struct rlr_texture_t rlr_texture_t;
typedef struct rlr_uniform_t rlr_uniform_t;
typedef struct rlr_cube_map_t rlr_cube_map_t;
typedef struct rlr_model_static_t rlr_model_static_t;
typedef struct rlr_obj_label_t rlr_obj_label_t;

typedef struct rlr_t {
    GLFWwindow* window;
    rlr_backend_t* backend;

    int32_t framebuffer_width;
    int32_t framebuffer_height;

    rlr_model_static_t* test;
    rlr_cube_map_t* test_cube_map;

    //built in resources
    rlr_texture_t* texture_white;
    rlr_shader_t* shader_text;
    rlr_shader_t* shader_model;
    rlr_uniform_t* ubo_ui;
    rlr_uniform_t* ubo_model;
    rlr_uniform_t* ubo_material;

    //objects
    rlr_obj_label_t* obj_labels;
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