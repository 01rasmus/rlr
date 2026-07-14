#pragma once
#include <stdbool.h>
#include <stdint.h>
#include "rlr/math/matrix.h"
#include "rlr/def.h"

typedef struct rlr_pipeline_static_model_draw_command_t rlr_pipeline_static_model_draw_command_t;
typedef struct rlr_res_animated_model_t rlr_res_animated_model_t;
typedef struct rlr_res_shader_t rlr_res_shader_t;

typedef struct rlr_obj_animated_model_t {

    //trs
    rlr_vec3_t translation;
    rlr_quat_t rotation;
    rlr_vec3_t scale;

    //animation info
    int32_t current_animation_index;
    float animation_time;
    float animation_speed;
    bool animation_loop;

    //transparency
    bool opaque;

    //internal pipeline context
    uint32_t cmd_index;
    uint32_t cmd_generation;
    uint32_t object_index;
    uint32_t instance_index;
} rlr_obj_animated_model_t;

rlr_obj_t rlr_obj_animated_model_create(rlr_res_animated_model_t* model, rlr_vec3_t translation, rlr_quat_t rotation, rlr_vec3_t scale);
rlr_vec3_t rlr_obj_animated_model_get_translation(rlr_obj_t model);
rlr_quat_t rlr_obj_animated_model_get_rotation(rlr_obj_t model);
rlr_vec3_t rlr_obj_animated_model_get_scale(rlr_obj_t model);
bool rlr_obj_animated_model_is_animating(rlr_obj_t model);

/*
    returns -1 if no animation is running
*/
int32_t rlr_obj_animated_model_get_current_animation(rlr_obj_t model);

void rlr_obj_animated_model_set_animation(rlr_obj_t model, int32_t animation_index, float speed, bool loop);

/*
    1.0 is the default
*/
void rlr_obj_animated_model_set_animation_speed(rlr_obj_t model, float speed);

/*
    only changes the components that arent null
*/
void rlr_obj_animated_model_set_trs(rlr_obj_t model, const rlr_vec3_t* translation, const rlr_quat_t* rotation, const rlr_vec3_t* scale);
void rlr_obj_animated_model_free(rlr_obj_t obj);