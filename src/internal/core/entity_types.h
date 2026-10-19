#pragma once
#include <stdbool.h>
#include <stdint.h>
#include "../../rlr/entities/nine_patch.h"
#include "../../external/rlpp.h"
#include "../../rlr/math/alignment.h"
#include "../../rlr/math/anchor.h"
#include "../../rlr/math/quat.h"
#include "../../rlr/math/rect.h"
#include "../../rlr/math/vec.h"
#include "res_types.h"

typedef struct rlr_label_instance_ctx_t {
    uint64_t cmd_id;
    uint64_t* instance_indices;
} rlr_label_instance_ctx_t;

/*
    Label entity
*/
typedef struct rlr_lbl_t {
    rlr_font_t* font;
    rlr_label_instance_ctx_t* instance_ctxs;
    size_t instance_ctxs_len;
    rlr_rect_t rect;
    rlr_hori_algn_t horizontal_alignment;
    rlr_vert_algn_t vertical_alignment;
    float size;
    bool visible;
} rlr_lbl_t;

/*
    Sprite entity
*/
typedef struct rlr_spr_t {
    rlr_rect_t rect;
    uint32_t layer;
    uint64_t cmd_id;
    uint64_t instance_index;
} rlr_spr_t;

/*
    animated model
*/
#define RLR_OBJ_ANIMATION_STATE_COUNT       2
#define RLR_OBJ_ANIMATION_PRIMARY           0
#define RLR_OBJ_ANIMATION_SECONDARY         1

typedef struct rlr_animation_state_t {
    int32_t animation_index;
    float animation_time;
    float animation_speed;
    bool animation_loop;
} rlr_animation_state_t;

/*
    Animated object entity (an instance of an animated model)
*/
typedef struct rlr_anm_obj_t {

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
} rlr_anm_obj_t;

/*
    Static object entity (an instance of a static model)
*/
typedef struct rlr_sta_obj_t {

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
} rlr_sta_obj_t;

/*
    Model occluder entity
*/
typedef struct rlr_mdl_occl_t {
    rlpp_ref_t instance_ref;
    rlr_rect_t rect;
    uint32_t index;
    bool visible;
} rlr_mdl_occl_t;

/*
    Mouse input area entity
*/
typedef struct rlr_ms_inpt_area_t {
    rlpp_id_t id;
} rlr_ms_inpt_area_t;

/*
    Nine patch entity
*/
typedef struct rlr_npatch_t {
    rlr_tex_t* texture;
    uint64_t cmd_id;
    uint64_t instances[9];
    rlr_npatch_desc_t nine_patch;
    rlr_rect_t rect;
    bool visible;
} rlr_npatch_t;