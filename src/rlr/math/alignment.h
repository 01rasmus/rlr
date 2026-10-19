#pragma once

typedef enum rlr_hori_algn_t {
    RLR_HORI_ALGN_LEFT = 0,
    RLR_HORI_ALGN_CENTER,
    RLR_HORI_ALGN_RIGHT,
    RLR_HORI_ALGN_JUSTIFIED,
} rlr_hori_algn_t;

typedef enum rlr_vert_algn_t {
    RLR_VERT_ALGN_TOP = 0,
    RLR_VERT_ALGN_CENTER,
    RLR_VERT_ALGN_BOTTOM,
} rlr_vert_algn_t;

float rlr_hori_start_pos(rlr_hori_algn_t alignment, float remaining_width);
float rlr_vert_start_pos(rlr_vert_algn_t alignment, float remaining_height);