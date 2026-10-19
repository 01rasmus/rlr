#pragma once
#include <stdbool.h>
#include <stdint.h>
#include "../math/rect.h"
#include "../math/anchor.h"
#include "../types/entity.h"
#include "../types/input.h"
#include "../types/res.h"

rlr_ms_inpt_area_t* rlr_ms_inpt_area_create(rlr_rect_t rect, int32_t layer, void* user, rlr_anchor_t local_anchor, rlr_anchor_t screen_anchor, rlr_input_area_on_enter_t on_enter, rlr_input_area_on_leave_t on_leave, rlr_input_area_on_pressed_t on_pressed, rlr_input_area_on_release_t on_release);
void rlr_ms_inpt_area_set_enabled(rlr_ms_inpt_area_t* mia, bool enabled);
void rlr_ms_inpt_area_free(rlr_ms_inpt_area_t* mia);