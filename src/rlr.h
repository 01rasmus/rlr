#pragma once
#include <stdbool.h>
#include <stdint.h>
#include "resources/font.h"
#include "backends/backend.h"

typedef struct GLFWwindow GLFWwindow;

typedef struct rlr_pipeline_t {
    int32_t vao;
    int32_t vbo;
    int32_t shader;
} rlr_pipeline_t;

typedef struct rlr_t {
    GLFWwindow* window;
    rlr_backend_t* backend;

    //pipelines
    rlr_pipeline_t pipeline_text;
} rlr_t;

rlr_t* rlr_init(const char* title, uint32_t window_width, uint32_t window_height, uint64_t flags);
bool rlr_render(rlr_t* rlr);
void rlr_free(rlr_t* rlr);