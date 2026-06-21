#pragma once
#include "backends/backend.h"

typedef struct rlr_shader_t rlr_shader_t;
typedef struct rlr_obj_model_occluder_t rlr_obj_model_occluder_t;

typedef struct rlr_pipeline_stencil_t {
    rlr_shader_t* shader;
    rlr_handle_t vao;
    rlr_handle_t vbo;
    rlr_handle_t instance_vbo;
    uint64_t instance_count;
    rlr_obj_model_occluder_t* obj_model_occluders;
    bool is_dirty;
} rlr_pipeline_stencil_t;

bool rlr_pipeline_stencil_init();
void rlr_pipeline_stencil_draw();
void rlr_pipeline_stencil_free();

rlr_obj_model_occluder_t* rlr_pipeline_stencil_alloc_model_occluder();
void rlr_pipeline_stencil_free_model_occluder(rlr_obj_model_occluder_t* mo);