#pragma once
#include <stdbool.h>
#include <stdint.h>
#include "../../external/rlpp.h"
#include "../../rlr/math/alignment.h"
#include "../../rlr/math/anchor.h"
#include "../../rlr/math/quat.h"
#include "../../rlr/math/rect.h"
#include "../../rlr/math/vec.h"
#include "res_types.h"

/*
    label
*/
typedef struct rlr_obj_label_t {
    rlr_res_font_t* font;
    uint64_t cmd_id;
    uint64_t* instance_indices;
    rlr_rect_t rectangle;
    rlr_anchor_t screen_anchor;
    rlr_anchor_t local_anchor;
    rlr_horizontal_alignment_t horizontal_alignment;
    rlr_vertical_alignment_t vertical_alignment;
    float size;
} rlr_obj_label_t;

/*
    sprite
*/
typedef struct rlr_obj_sprite_t {
    rlr_rect_t rect;
    rlr_anchor_t local_anchor;
    uint64_t cmd_id;
    uint64_t instance_index;
} rlr_obj_sprite_t;

/*
    animated model
*/
#define RLR_OBJ_ANIMATION_STATE_COUNT       2
#define RLR_OBJ_ANIMATION_PRIMARY           0
#define RLR_OBJ_ANIMATION_SECONDARY         1

typedef struct rlr_obj_animation_state_t {
    int32_t animation_index;
    float animation_time;
    float animation_speed;
    bool animation_loop;
} rlr_obj_animation_state_t;

typedef struct rlr_obj_animated_model_t {

    //trs
    rlr_vec3_t translation;
    rlr_quat_t rotation;
    rlr_vec3_t scale;

    //animation
    rlpp_ref_t animation_state_ref;

    //transparency
    bool opaque;

    //internal pipeline context
    uint32_t cmd_index;
    uint32_t cmd_generation;
    uint32_t object_index;
    uint32_t instance_index;
} rlr_obj_animated_model_t;

/*
    static model
*/
typedef struct rlr_obj_static_model_t {

    //trs
    rlr_vec3_t translation;
    rlr_quat_t rotation;
    rlr_vec3_t scale;

    //transparency
    bool opaque;

    //internal pipeline context
    uint32_t cmd_index;
    uint32_t cmd_generation;
    uint32_t object_index;
    uint32_t instance_index;
} rlr_obj_static_model_t;

/*
    model occluder
*/
typedef struct rlr_obj_model_occluder_t {
    rlpp_ref_t instance_ref;
    rlr_anchor_t screen_anchor;
    rlr_anchor_t local_anchor;
    rlr_rect_t rectangle;
    uint32_t index;
    bool visible;
} rlr_obj_model_occluder_t;

/*
    mouse input area
*/
typedef struct rlr_obj_mouse_input_area_t {
    rlpp_id_t id;
} rlr_obj_mouse_input_area_t;