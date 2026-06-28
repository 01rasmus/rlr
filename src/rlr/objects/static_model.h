#pragma once
#include <stdbool.h>
#include <stdint.h>
#include "rlr/io/sparse_gen_allocator.h"
#include "rlr/math/matrix.h"

typedef struct rlr_pipeline_static_model_draw_command_t rlr_pipeline_static_model_draw_command_t;
typedef struct rlr_res_static_model_t rlr_res_static_model_t;
typedef struct rlr_res_shader_t rlr_res_shader_t;

typedef rlr_sparse_gen_allocator_handle_t rlr_obj_static_model_handle_t;

typedef struct rlr_obj_static_model_t {

    //trs
    rlr_vec3_t translation;
    rlr_quat_t rotation;
    rlr_vec3_t scale;

    //transparency
    float alpha;
    bool opaque;

    //internal pipeline context
    uint32_t cmd_index;
    uint32_t cmd_generation;
    uint32_t object_index;
    uint32_t instance_index;
} rlr_obj_static_model_t;

rlr_obj_static_model_handle_t rlr_obj_static_model_create(rlr_res_static_model_t* model, rlr_vec3_t translation, rlr_quat_t rotation, rlr_vec3_t scale);
rlr_vec3_t rlr_obj_static_model_get_translation(rlr_obj_static_model_handle_t model);
rlr_quat_t rlr_obj_static_model_get_rotation(rlr_obj_static_model_handle_t model);
rlr_vec3_t rlr_obj_static_model_get_scale(rlr_obj_static_model_handle_t model);

/*
    only changes the components that arent null
*/
void rlr_obj_static_model_set_trs(rlr_obj_static_model_handle_t model, const rlr_vec3_t* translation, const rlr_quat_t* rotation, const rlr_vec3_t* scale);
void rlr_obj_static_model_free(rlr_obj_static_model_handle_t obj);