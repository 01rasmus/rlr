#pragma once
#include <stdbool.h>
#include <stdint.h>
#include "anchor.h"
#include "vec.h"

typedef struct rlr_rect_t {
    float x;
    float y;
    float width;
    float height;
} rlr_rect_t;

typedef struct rlr_anchored_rect_t {
    rlr_rect_t rect;
    rlr_anchor_t anchor;
} rlr_anchored_rect_t;

#define rlr_rect(X, Y, WIDTH, HEIGHT)           ((rlr_rect_t){.x = (X), .y = (Y), .width = (WIDTH), .height = (HEIGHT)})
#define rlr_anchored_rect(RECT, ANCHOR)         ((rlr_anchored_rect_t){.rect = (RECT), .anchor = (ANCHOR)})

bool rlr_rect_contains_position(const rlr_rect_t* rect, const rlr_vec2_t pos);
rlr_anchored_rect_t rlr_anchored_rect_inset(rlr_anchored_rect_t ar, float amount);
rlr_anchored_rect_t rlr_anchored_rect_inset_top(rlr_anchored_rect_t ar, float amount);
rlr_anchored_rect_t rlr_anchored_rect_inset_bottom(rlr_anchored_rect_t ar, float amount);
rlr_anchored_rect_t rlr_anchored_rect_inset_left(rlr_anchored_rect_t ar, float amount);
rlr_anchored_rect_t rlr_anchored_rect_inset_right(rlr_anchored_rect_t ar, float amount);
void rlr_anchored_rect_subdivide_vertically(const rlr_anchored_rect_t ar, uint32_t divisions, rlr_anchored_rect_t* out_anchors);
void rlr_anchored_rect_subdivide_horizontally(const rlr_anchored_rect_t ar, uint32_t divisions, rlr_anchored_rect_t* out_anchors);
void rlr_anchored_rect_subdivide_matrix(const rlr_anchored_rect_t ar, uint32_t horizontal_divisons, uint32_t vertical_divisions, rlr_anchored_rect_t* out_anchors);

inline rlr_anchored_rect_t rlr_anchored_rect_outset(rlr_anchored_rect_t ar, float amount) {
    return rlr_anchored_rect_inset(ar, -amount);
}

inline rlr_anchored_rect_t rlr_anchored_rect_take_top(rlr_anchored_rect_t ar, float amount) {
    return rlr_anchored_rect_inset_bottom(ar, ar.rect.height - amount);
}

inline rlr_anchored_rect_t rlr_anchored_rect_take_bottom(rlr_anchored_rect_t ar, float amount) {
    return rlr_anchored_rect_inset_top(ar, ar.rect.height - amount);
}

inline rlr_anchored_rect_t rlr_anchored_rect_take_left(rlr_anchored_rect_t ar, float amount) {
    return rlr_anchored_rect_inset_right(ar, ar.rect.width - amount);
}

inline rlr_anchored_rect_t rlr_anchored_rect_take_right(rlr_anchored_rect_t ar, float amount) {
    return rlr_anchored_rect_inset_left(ar, ar.rect.width - amount);
}