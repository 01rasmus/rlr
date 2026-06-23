#pragma once
#include "internal/backends/backend.h"

#define RLR_PIPELINE_STENCIL    (&rlr()->pipeline_stencil)

typedef struct rlr_res_shader_t rlr_res_shader_t;
typedef struct rlr_obj_model_occluder_t rlr_obj_model_occluder_t;

typedef struct rlr_pipeline_stencil_t {
    rlr_res_shader_t* shader;
    uint64_t vao;
    uint64_t vbo;
    uint64_t instance_vbo;
    uint64_t instance_count;
    rlr_obj_model_occluder_t* obj_model_occluders;
    bool is_dirty;
} rlr_pipeline_stencil_t;

bool rlr_pipeline_stencil_init();
void rlr_pipeline_stencil_draw();
void rlr_pipeline_stencil_deinit();

rlr_obj_model_occluder_t* rlr_pipeline_stencil_alloc_model_occluder();
void rlr_pipeline_stencil_free_model_occluder(rlr_obj_model_occluder_t* mo);