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
    rlr_anchor_t screen_anchor;
    rlr_anchor_t local_anchor;
} rlr_rect_t;

#define rlr_rect(X, Y, W, H)                                          ((rlr_rect_t){.x = (X), .y = (Y), .width = (W), .height = (H), .local_anchor = RLR_ANCH_TOP_LEFT, .screen_anchor = RLR_ANCH_TOP_LEFT})
#define rlr_rect_lcl(X, Y, W, H, LOCAL_ANCHOR)                        ((rlr_rect_t){.x = (X), .y = (Y), .width = (W), .height = (H), .local_anchor = (LOCAL_ANCHOR), .screen_anchor = RLR_ANCH_TOP_LEFT})
#define rlr_rect_lcl_scrn(X, Y, W, H, LOCAL_ANCHOR, SCREEN_ANCHOR)    ((rlr_rect_t){.x = (X), .y = (Y), .width = (W), .height = (H), .local_anchor = (LOCAL_ANCHOR), .screen_anchor = (SCREEN_ANCHOR)})

bool rlr_rect_contains_pos(const rlr_rect_t* rect, rlr_vec2_t pos, rlr_vec2_t screen_size);
rlr_rect_t rlr_rect_inset(rlr_rect_t rect, float amount);
rlr_rect_t rlr_rect_inset_vert(rlr_rect_t rect, float amount);
rlr_rect_t rlr_rect_inset_hori(rlr_rect_t rect, float amount);
rlr_rect_t rlr_rect_inset_top(rlr_rect_t rect, float amount);
rlr_rect_t rlr_rect_inset_bot(rlr_rect_t rect, float amount);
rlr_rect_t rlr_rect_inset_left(rlr_rect_t rect, float amount);
rlr_rect_t rlr_rect_inset_right(rlr_rect_t rect, float amount);
void rlr_rect_subdiv_vert(const rlr_rect_t rect, uint32_t divisions, rlr_rect_t* out_rects);
void rlr_rect_subdiv_hori(const rlr_rect_t rect, uint32_t divisions, rlr_rect_t* out_rects);
void rlr_rect_subdiv_matrix(const rlr_rect_t rect, uint32_t horizontal_divisions, uint32_t vertical_divisions, rlr_rect_t* out_rects);
void rlr_rect_split_hori(const rlr_rect_t rect, float ratio, rlr_rect_t out_rects[2]);
void rlr_rect_split_vert(const rlr_rect_t rect, float ratio, rlr_rect_t out_rects[2]);

static inline rlr_rect_t rlr_rect_outset(rlr_rect_t rect, float amount) {
    return rlr_rect_inset(rect, -amount);
}

static inline rlr_rect_t rlr_rect_take_top(rlr_rect_t rect, float amount) {
    return rlr_rect_inset_bot(rect, rect.height - amount);
}

static inline rlr_rect_t rlr_rect_take_bottom(rlr_rect_t rect, float amount) {
    return rlr_rect_inset_top(rect, rect.height - amount);
}

static inline rlr_rect_t rlr_rect_take_left(rlr_rect_t rect, float amount) {
    return rlr_rect_inset_right(rect, rect.width - amount);
}

static inline rlr_rect_t rlr_rect_take_right(rlr_rect_t rect, float amount) {
    return rlr_rect_inset_left(rect, rect.width - amount);
}