#pragma once
#include <stdbool.h>
#include <stdint.h>
#include "../resources/animated_model.h"
#include "../math/matrix.h"
#include "../types/entity.h"
#include "../types/res.h"

rlr_anm_obj_t* rlr_anm_obj_create(rlr_anm_mdl_t* model, rlr_vec3_t translation, rlr_quat_t rotation, rlr_vec3_t scale);
rlr_vec3_t rlr_anm_obj_get_trans(const rlr_anm_obj_t* model);
rlr_quat_t rlr_anm_obj_get_rot(const rlr_anm_obj_t* model);
rlr_vec3_t rlr_anm_obj_get_scale(const rlr_anm_obj_t* model);

/*
    returns -1 if no animation is running
*/
int32_t rlr_anm_obj_get_curr_anm(rlr_anm_obj_t* model);

void rlr_anm_obj_set_anm(rlr_anm_obj_t* model, int32_t animation_index, float speed, bool loop);
void rlr_anm_obj_set_anm_blnd(rlr_anm_obj_t* model, int32_t animation_index, float speed, float transition_time, bool loop);

/*
    1.0 is the default
*/
void rlr_anm_obj_set_anm_spd(rlr_anm_obj_t* model, float speed);

/*
    only changes the components that arent null
*/
void rlr_anm_obj_set_trs(rlr_anm_obj_t* model, const rlr_vec3_t* translation, const rlr_quat_t* rotation, const rlr_vec3_t* scale);
void rlr_anm_obj_free(rlr_anm_obj_t* obj);