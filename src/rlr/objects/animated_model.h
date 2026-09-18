#pragma once
#include <stdbool.h>
#include <stdint.h>
#include "../resources/animated_model.h"
#include "../math/matrix.h"
#include "../def.h"

typedef struct rlr_obj_animated_model_t rlr_obj_animated_model_t;

rlr_obj_animated_model_t* rlr_obj_animated_model_create(rlr_res_animated_model_t* model, rlr_vec3_t translation, rlr_quat_t rotation, rlr_vec3_t scale);
rlr_vec3_t rlr_obj_animated_model_get_translation(const rlr_obj_animated_model_t* model);
rlr_quat_t rlr_obj_animated_model_get_rotation(const rlr_obj_animated_model_t* model);
rlr_vec3_t rlr_obj_animated_model_get_scale(const rlr_obj_animated_model_t* model);
bool rlr_obj_animated_model_is_animating(rlr_obj_animated_model_t* model);

/*
    returns -1 if no animation is running
*/
int32_t rlr_obj_animated_model_get_current_animation(rlr_obj_animated_model_t* model);

void rlr_obj_animated_model_set_animation(rlr_obj_animated_model_t* model, int32_t animation_index, float speed, bool loop);
void rlr_obj_animated_model_set_animation_blended(rlr_obj_animated_model_t* model, int32_t animation_index, float speed, float transition_time, bool loop);

/*
    1.0 is the default
*/
void rlr_obj_animated_model_set_animation_speed(rlr_obj_animated_model_t* model, float speed);

/*
    only changes the components that arent null
*/
void rlr_obj_animated_model_set_trs(rlr_obj_animated_model_t* model, const rlr_vec3_t* translation, const rlr_quat_t* rotation, const rlr_vec3_t* scale);
void rlr_obj_animated_model_free(rlr_obj_animated_model_t* obj);