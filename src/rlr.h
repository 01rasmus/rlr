#pragma once
#include <stdbool.h>
#include <stdint.h>
#include "resources/font.h"
#include "backends/backend.h"
#include "objects/label.h"

typedef struct GLFWwindow GLFWwindow;

typedef struct rlr_t {
    GLFWwindow* window;
    rlr_backend_t* backend;

    //resources
    rlr_shader_t* shader_text;

    //objects
    rlr_obj_label_t* obj_labels;
} rlr_t;

rlr_t* rlr_init(const char* title, uint32_t window_width, uint32_t window_height, uint64_t flags);
double rlr_time();
bool rlr_draw(rlr_t* rlr);
void rlr_free(rlr_t* rlr);