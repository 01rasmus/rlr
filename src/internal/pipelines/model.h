#pragma once
#include <stdbool.h>
#include <stdint.h>
#include "rlr/math/matrix.h"

#define RLR_PIPELINE_MODEL    (&rlr()->pipeline_model)

typedef struct rlr_res_shader_t rlr_res_shader_t;
typedef struct rlr_res_static_mesh_t rlr_res_static_mesh_t;
typedef struct rlr_obj_static_model_t rlr_obj_static_model_t;

typedef struct rlr_pipeline_model_instance_t {
    rlr_mat4_t matrix;
    float alpha;
    float _pad[3];
} rlr_pipeline_model_instance_t;

typedef struct rlr_pipeline_model_draw_command_t {
    rlr_res_shader_t* shader;
    rlr_res_static_mesh_t* mesh;
    uint64_t vao;
    uint64_t instance_vbo;
    rlr_pipeline_model_instance_t* instances;
} rlr_pipeline_model_draw_command_t;

typedef struct rlr_pipeline_model_t {
    rlr_obj_static_model_t* obj_models;
    rlr_res_shader_t* shader_opaque;
    rlr_res_shader_t* shader_transparent;
    rlr_pipeline_model_draw_command_t* commands;
} rlr_pipeline_model_t;

bool rlr_pipeline_model_init();
void rlr_pipeline_model_draw();
void rlr_pipeline_model_deinit();

rlr_obj_static_model_t* rlr_pipeline_model_alloc_static_model();
void rlr_pipeline_model_free_static_model(rlr_obj_static_model_t* sm);