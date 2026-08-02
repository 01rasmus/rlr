#pragma once

typedef enum rlr_horizontal_alignment_t {
    RLR_HORIZONTAL_ALIGNMENT_LEFT = 0,
    RLR_HORIZONTAL_ALIGNMENT_CENTER,
    RLR_HORIZONTAL_ALIGNMENT_RIGHT,
    RLR_HORIZONTAL_ALIGNMENT_JUSTIFIED,
} rlr_horizontal_alignment_t;

typedef enum rlr_vertical_alignment_t {
    RLR_VERTICAL_ALIGNMENT_TOP = 0,
    RLR_VERTICAL_ALIGNMENT_CENTER,
    RLR_VERTICAL_ALIGNMENT_BOTTOM,
} rlr_vertical_alignment_t;

float rlr_horizontal_start_position(rlr_horizontal_alignment_t alignment, float remaining_width);
float rlr_vertical_start_position(rlr_vertical_alignment_t alignment, float remaining_height);