#pragma once
#include <stdbool.h>
#include <stdint.h>
#include "rlr/math/matrix.h"
#include "rlr/def.h"

typedef struct rlr_pipeline_static_model_instance_t {
    rlr_affine_mat4x3_t matrix;
} rlr_pipeline_static_model_instance_t;

typedef struct rlr_pipeline_animated_model_instance_t {
    rlr_affine_mat4x3_t matrix;
    uint32_t pose_a_offset_primary;
    uint32_t pose_b_offset_primary;
    uint32_t pose_a_offset_secondary;
    uint32_t pose_b_offset_secondary;
    uint16_t transition_lerp;
    uint8_t lerp_primary;
    uint8_t lerp_secondary;
} rlr_pipeline_animated_model_instance_t;

typedef struct rlr_pipeline_static_model_draw_command_t {
    rlr_res_t model;
    rlr_res_t shader;
    uint64_t* mesh_vaos;
    uint64_t instance_vbo; //shared between all vaos
    uint32_t generation;
    uint32_t index;
    rlr_pipeline_static_model_instance_t* instances;
    bool dirty;
} rlr_pipeline_static_model_draw_command_t;

typedef struct rlr_pipeline_animated_model_draw_command_t {
    rlr_res_t model;
    rlr_res_t shader;
    uint64_t* mesh_vaos;
    uint64_t instance_vbo; //shared between all vaos
    uint32_t generation;
    uint32_t index;
    rlr_pipeline_animated_model_instance_t* instances;
    bool dirty;
} rlr_pipeline_animated_model_draw_command_t;

typedef struct rlr_pipeline_model_t {
    rlr_res_t shader_opaque_static_model;
    rlr_res_t shader_opaque_animated_model;

    uint32_t generation_counter;

    rlr_pipeline_static_model_draw_command_t* opaque_static_model_commands;
    rlr_pipeline_animated_model_draw_command_t* opaque_animated_model_commands;
} rlr_pipeline_model_t;

bool rlr_pipeline_model_init();
void rlr_pipeline_model_draw(double delta_time);
void rlr_pipeline_model_deinit();

/*
    tries to find a draw command list where the specific
    configuration belongs to. If there is no command,
    a new one will be created.
*/
rlr_pipeline_static_model_draw_command_t* rlr_pipeline_model_find_static_model_draw_command(rlr_res_t model, rlr_res_t shader);
rlr_pipeline_animated_model_draw_command_t* rlr_pipeline_model_find_animated_model_draw_command(rlr_res_t model, rlr_res_t shader);

/*
    returns the index to the instance buffer.
    returns -1 on failure
*/
uint32_t rlr_pipeline_model_add_static_model_instance(rlr_pipeline_static_model_draw_command_t* command, rlr_pipeline_static_model_instance_t data);
void rlr_pipeline_model_update_static_model_instance(uint32_t cmd_index, uint32_t cmd_generation, uint32_t instance_index, rlr_pipeline_static_model_instance_t data);
uint32_t rlr_pipeline_model_add_animated_model_instance(rlr_pipeline_animated_model_draw_command_t* command, rlr_pipeline_animated_model_instance_t data);
void rlr_pipeline_model_update_animated_model_instance(uint32_t cmd_index, uint32_t cmd_generation, uint32_t instance_index, rlr_pipeline_animated_model_instance_t data);
void rlr_pipeline_model_swap_animation_states(uint32_t cmd_index, uint32_t cmd_generation, uint32_t instance_index);