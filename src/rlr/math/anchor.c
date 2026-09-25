#include "anchor.h"

rlr_vec2_t rlr_anchor_vec(rlr_anchor_t anchor) {
    switch(anchor) {
        case RLR_ANCHOR_TOP_CENTER:
            return rlr_vec2(0.5, 0.0);
        case RLR_ANCHOR_TOP_RIGHT:
            return rlr_vec2(1.0, 0.0);
        case RLR_ANCHOR_CENTER_LEFT:
            return rlr_vec2(0.0, 0.5);
        case RLR_ANCHOR_CENTER_CENTER:
            return rlr_vec2(0.5, 0.5);
        case RLR_ANCHOR_CENTER_RIGHT:
            return rlr_vec2(1.0, 0.5);
        case RLR_ANCHOR_BOTTOM_LEFT:
            return rlr_vec2(0.0, 1.0);
        case RLR_ANCHOR_BOTTOM_CENTER:
            return rlr_vec2(0.5, 1.0);
        case RLR_ANCHOR_BOTTOM_RIGHT:
            return rlr_vec2(1.0, 1.0);
        default:
        case RLR_ANCHOR_TOP_LEFT: 
            return rlr_vec2(0, 0);
    }
}

float rlr_anchor_x(rlr_anchor_t anchor) {
    switch(anchor) {
        case RLR_ANCHOR_TOP_RIGHT:
        case RLR_ANCHOR_CENTER_RIGHT:
        case RLR_ANCHOR_BOTTOM_RIGHT:
            return 1.0;
        case RLR_ANCHOR_TOP_CENTER:
        case RLR_ANCHOR_CENTER_CENTER:
        case RLR_ANCHOR_BOTTOM_CENTER:
            return 0.5;
        case RLR_ANCHOR_CENTER_LEFT:
        case RLR_ANCHOR_BOTTOM_LEFT:
        case RLR_ANCHOR_TOP_LEFT:
        default:
            return 0.0;
    }
}

float rlr_anchor_y(rlr_anchor_t anchor) {
    switch(anchor) {
        case RLR_ANCHOR_BOTTOM_LEFT:
        case RLR_ANCHOR_BOTTOM_CENTER:
        case RLR_ANCHOR_BOTTOM_RIGHT:
            return 1.0;
        case RLR_ANCHOR_CENTER_LEFT:
        case RLR_ANCHOR_CENTER_CENTER:
        case RLR_ANCHOR_CENTER_RIGHT:
            return 0.5;
        case RLR_ANCHOR_TOP_CENTER:
        case RLR_ANCHOR_TOP_RIGHT:
        case RLR_ANCHOR_TOP_LEFT:
        default:
            return 0.0;
    }
}