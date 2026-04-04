#pragma once
#include <stdbool.h>
#include <stdint.h>
#include "resources/font.h"

typedef struct RGFW_window RGFW_window;

typedef struct rlr_pipeline_t {
    int32_t vao;
    int32_t vbo;
    int32_t shader;
} rlr_pipeline_t;

typedef struct rlr_t {
    RGFW_window* window;

    //pipelines
    rlr_pipeline_t pipeline_text;
} rlr_t;

bool rlr_init(const char* title, uint32_t window_width, uint32_t window_height, uint64_t flags);
bool rlr_render();
void rlr_deinit();