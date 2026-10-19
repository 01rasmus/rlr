#pragma once
#include "../math/matrix.h"
#include "../types/entity.h"
#include "../types/res.h"

rlr_sta_obj_t* rlr_sta_obj_create(rlr_sta_mdl_t* model, rlr_vec3_t translation, rlr_quat_t rotation, rlr_vec3_t scale);
rlr_vec3_t rlr_sta_obj_get_trans(const rlr_sta_obj_t* model);
rlr_quat_t rlr_sta_obj_get_rot(const rlr_sta_obj_t* model);
rlr_vec3_t rlr_sta_obj_get_scale(const rlr_sta_obj_t* model);

/*
    only changes the components that arent null
*/
void rlr_sta_obj_set_trs(rlr_sta_obj_t* model, const rlr_vec3_t* translation, const rlr_quat_t* rotation, const rlr_vec3_t* scale);
void rlr_sta_obj_free(rlr_sta_obj_t* model);