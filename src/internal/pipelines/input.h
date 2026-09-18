#pragma once
#include <stdbool.h>
#include "../../external/rlpp.h"
#include "../../rlr/math/anchor.h"
#include "../../rlr/math/rect.h"
#include "../../rlr/def.h"

typedef struct rlr_pipeline_input_mouse_area_t {
    rlpp_id_t id;
    rlr_rect_t rect;
    void* user;
    int64_t layer;
    bool enabled;
    rlr_anchor_t local_anchor;
    rlr_anchor_t screen_anchor;

    rlr_input_area_on_enter_t on_enter;
    rlr_input_area_on_leave_t on_leave;
    rlr_input_area_on_pressed_t on_pressed;
    rlr_input_area_on_release_t on_release;
} rlr_pipeline_input_mouse_area_t;

typedef struct rlr_pipeline_input_t {
    rlr_pipeline_input_mouse_area_t* mouse_areas;

    //states
    rlpp_id_t current_hovered;
    rlpp_id_t current_pressed;
} rlr_pipeline_input_t;

bool rlr_pipeline_input_init();
void rlr_pipeline_input_update(rlr_vec2_t mouse_position, rlr_vec2_t screen_size);
void rlr_pipeline_input_deinit();

rlpp_id_t rlr_pipeline_input_add_area(rlr_pipeline_input_mouse_area_t input_area);
void rlr_pipeline_input_remove_area(rlpp_id_t id);