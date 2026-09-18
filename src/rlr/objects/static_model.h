#pragma once
#include <stdbool.h>
#include <stdint.h>
#include "../math/matrix.h"
#include "../def.h"

typedef struct rlr_obj_static_model_t rlr_obj_static_model_t;
typedef struct rlr_res_static_model_t rlr_res_static_model_t;

rlr_obj_static_model_t* rlr_obj_static_model_create(rlr_res_static_model_t* model, rlr_vec3_t translation, rlr_quat_t rotation, rlr_vec3_t scale);
rlr_vec3_t rlr_obj_static_model_get_translation(const rlr_obj_static_model_t* model);
rlr_quat_t rlr_obj_static_model_get_rotation(const rlr_obj_static_model_t* model);
rlr_vec3_t rlr_obj_static_model_get_scale(const rlr_obj_static_model_t* model);

/*
    only changes the components that arent null
*/
void rlr_obj_static_model_set_trs(rlr_obj_static_model_t* model, const rlr_vec3_t* translation, const rlr_quat_t* rotation, const rlr_vec3_t* scale);
void rlr_obj_static_model_free(rlr_obj_static_model_t* model);