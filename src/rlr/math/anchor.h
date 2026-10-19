#pragma once
#include "vec.h"

typedef enum rlr_anchor_t {
    RLR_ANCH_TOP_LEFT = 0,
    RLR_ANCH_TOP_CENTER,
    RLR_ANCH_TOP_RIGHT,
    RLR_ANCH_CENTER_LEFT,
    RLR_ANCH_CENTER_CENTER,
    RLR_ANCH_CENTER_RIGHT,
    RLR_ANCH_BOTTOM_LEFT,
    RLR_ANCH_BOTTOM_CENTER,
    RLR_ANCH_BOTTOM_RIGHT,
} rlr_anchor_t;

rlr_vec2_t rlr_anchor_vec(rlr_anchor_t anchor);
float rlr_anchor_x(rlr_anchor_t anchor);
float rlr_anchor_y(rlr_anchor_t anchor);