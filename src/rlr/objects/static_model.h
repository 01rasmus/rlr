#pragma once
#include <stdbool.h>
#include <stdint.h>
#include "rlr/math/matrix.h"

typedef struct rlr_res_static_model_t rlr_res_static_model_t;
typedef struct rlr_res_shader_t rlr_res_shader_t;

typedef struct rlr_obj_static_model_t {
    rlr_res_static_model_t* model;
    rlr_res_shader_t* shader;
    uint32_t index;

    //trs
    rlr_mat4x4_t matrix;
    rlr_vec3_t translation;
    rlr_quat_t rotation;
    rlr_vec3_t scale;

    //transparency
    bool opaque;
    float alpha;
} rlr_obj_static_model_t;

rlr_obj_static_model_t* rlr_obj_static_model_create(rlr_res_static_model_t* model, rlr_vec3_t translation, rlr_quat_t rotation, rlr_vec3_t scale);
void rlr_obj_static_model_free(rlr_obj_static_model_t* obj);