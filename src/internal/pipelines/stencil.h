#pragma once
#include "../../external/rlpp.h"
#include "../backends/backend.h"
#include "../core/res_types.h"

#define RLR_PIPELINE_STENCIL    (&rlr()->pipeline_stencil)

typedef struct rlr_pipeline_stencil_occluder_instance_t {
    rlr_vec2_t pos;
    rlr_vec2_t size;
    rlr_vec2_t screen_anchor;
    uint8_t visible;
} rlr_pipeline_stencil_occluder_instance_t;

typedef struct rlr_pipeline_stencil_t {
    rlr_res_shader_t* shader;
    uint64_t vao;
    uint64_t vbo;
    uint64_t ebo;
    uint64_t instance_vbo;
    bool is_dirty;
    rlr_pipeline_stencil_occluder_instance_t* occluder_instances;
    uint32_t visible_occluder_instances;
} rlr_pipeline_stencil_t;

bool rlr_pipeline_stencil_init();
void rlr_pipeline_stencil_draw();
void rlr_pipeline_stencil_deinit();
bool rlr_pipeline_stencil_add_model_occluder_instance(rlr_pipeline_stencil_occluder_instance_t data, rlpp_ref_t* out_ref);
void rlr_pipeline_stencil_set_model_occluder_instance_visability(rlpp_ref_t ref, bool visability);
rlr_pipeline_stencil_occluder_instance_t* rlr_pipeline_stencil_get_and_dirty_model_occluder_instance(rlpp_ref_t ref);
void rlr_pipeline_stencil_remove_model_occluder_instance(rlpp_ref_t ref);