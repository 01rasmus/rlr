#pragma once
#include "backends/backend.h"

typedef struct rlr_shader_t rlr_shader_t;

typedef struct rlr_pipeline_stencil_t {
    rlr_shader_t* shader;
    rlr_handle_t vao;
    rlr_handle_t vbo;
    uint64_t vertex_count;
    bool is_dirty;
} rlr_pipeline_stencil_t;

bool rlr_pipeline_stencil_init();
void rlr_pipeline_stencil_draw();
void rlr_pipeline_stencil_free();