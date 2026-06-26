#pragma once
#include <stdbool.h>
#include <stdint.h>
#include "rlr/math/matrix.h"

#define RLR_PIPELINE_MODEL    (&rlr()->pipeline_model)

typedef struct rlr_res_shader_t rlr_res_shader_t;
typedef struct rlr_res_static_mesh_t rlr_res_static_mesh_t;
typedef struct rlr_obj_static_model_t rlr_obj_static_model_t;

typedef struct vec4_t {
    float x;
    float y;
    float z;
    float w;
} vec4_t;

typedef struct rlr_pipeline_model_instance_t {
    rlr_affine_mat4x3_t matrix;
    float alpha;
} rlr_pipeline_model_instance_t;

typedef struct rlr_pipeline_model_draw_command_t {
    rlr_res_shader_t* shader;
    rlr_res_static_mesh_t* mesh;
    uint64_t vao;
    uint64_t instance_vbo;
    bool dirty;
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

/*
    tries to find a draw command list where the specific
    configuration belongs to. If there is no command,
    a new one will be created.
*/
rlr_pipeline_model_draw_command_t* rlr_pipeline_model_find_draw_command(rlr_res_static_mesh_t* mesh, rlr_res_shader_t* shader);

rlr_obj_static_model_t* rlr_pipeline_model_alloc_static_model();
void rlr_pipeline_model_free_static_model(rlr_obj_static_model_t* sm);