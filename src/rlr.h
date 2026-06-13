#pragma once
#include <stdbool.h>
#include <stdint.h>

typedef struct rlr_backend_t rlr_backend_t;
typedef struct GLFWwindow GLFWwindow;

typedef struct rlr_shader_t rlr_shader_t;
typedef struct rlr_uniform_t rlr_uniform_t;
typedef struct rlr_obj_label_t rlr_obj_label_t;

typedef struct rlr_t {
    GLFWwindow* window;
    rlr_backend_t* backend;

    int32_t framebuffer_width;
    int32_t framebuffer_height;

    //resources
    rlr_shader_t* shader_text;
    rlr_uniform_t* ubo_ui;

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