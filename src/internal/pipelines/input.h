#pragma once
#include <stdbool.h>
#include "../../rlr/math/anchor.h"
#include "../../rlr/math/rect.h"
#include "../../rlr/def.h"

typedef struct rlr_pipeline_input_mouse_area_t {
    uint64_t id;
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
    uint64_t current_hovered;
    uint64_t current_pressed;
} rlr_pipeline_input_t;

bool rlr_pipeline_input_init();
void rlr_pipeline_input_update(rlr_vec2_t mouse_position, rlr_vec2_t screen_size);
void rlr_pipeline_input_deinit();

uint64_t rlr_pipeline_input_add_area(rlr_pipeline_input_mouse_area_t input_area);
void rlr_pipeline_input_remove_area(uint64_t id);