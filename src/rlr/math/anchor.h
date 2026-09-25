#pragma once
#include "vec.h"

typedef enum rlr_anchor_t {
    RLR_ANCHOR_TOP_LEFT = 0,
    RLR_ANCHOR_TOP_CENTER,
    RLR_ANCHOR_TOP_RIGHT,
    RLR_ANCHOR_CENTER_LEFT,
    RLR_ANCHOR_CENTER_CENTER,
    RLR_ANCHOR_CENTER_RIGHT,
    RLR_ANCHOR_BOTTOM_LEFT,
    RLR_ANCHOR_BOTTOM_CENTER,
    RLR_ANCHOR_BOTTOM_RIGHT,
} rlr_anchor_t;

rlr_vec2_t rlr_anchor_vec(rlr_anchor_t anchor);
float rlr_anchor_x(rlr_anchor_t anchor);
float rlr_anchor_y(rlr_anchor_t anchor);