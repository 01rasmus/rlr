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

typedef struct rlr_anch_rect_t {
    rlr_rect_t rect;
    rlr_anchor_t screen_anchor;
    rlr_anchor_t local_anchor;
} rlr_anch_rect_t;

#define rlr_rect(X, Y, WIDTH, HEIGHT)                                       ((rlr_rect_t){.x = (X), .y = (Y), .width = (WIDTH), .height = (HEIGHT)})
#define rlr_anch_rect(RECT, LOCAL_ANCHOR)                                   ((rlr_anch_rect_t){.rect = (RECT), .local_anchor = (LOCAL_ANCHOR), .screen_anchor = RLR_ANCH_TOP_LEFT})
#define rlr_anch_rect_scrn(RECT, LOCAL_ANCHOR, SCREEN_ANCHOR)               ((rlr_anch_rect_t){.rect = (RECT), .local_anchor = (LOCAL_ANCHOR), .screen_anchor = (SCREEN_ANCHOR)})
#define rlr_anch_rect_xywh(X, Y, W, H, LOCAL_ANCHOR)                        ((rlr_anch_rect_t){.rect = {.x = (X), .y = (Y), .width = (W), .height = (H)}, .local_anchor = (LOCAL_ANCHOR), .screen_anchor = RLR_ANCH_TOP_LEFT})
#define rlr_anch_rect_xywh_scrn(X, Y, W, H, LOCAL_ANCHOR, SCREEN_ANCHOR)    ((rlr_anch_rect_t){.rect = {.x = (X), .y = (Y), .width = (W), .height = (H)}, .local_anchor = (LOCAL_ANCHOR), .screen_anchor = (SCREEN_ANCHOR)})

bool rlr_rect_contains_pos(const rlr_rect_t* rect, const rlr_vec2_t pos);
rlr_anch_rect_t rlr_anch_rect_inset(rlr_anch_rect_t ar, float amount);
rlr_anch_rect_t rlr_anch_rect_inset_vert(rlr_anch_rect_t ar, float amount);
rlr_anch_rect_t rlr_anch_rect_inset_hori(rlr_anch_rect_t ar, float amount);
rlr_anch_rect_t rlr_anch_rect_inset_top(rlr_anch_rect_t ar, float amount);
rlr_anch_rect_t rlr_anch_rect_inset_bot(rlr_anch_rect_t ar, float amount);
rlr_anch_rect_t rlr_anch_rect_inset_left(rlr_anch_rect_t ar, float amount);
rlr_anch_rect_t rlr_anch_rect_inset_right(rlr_anch_rect_t ar, float amount);
void rlr_anch_rect_subdiv_vert(const rlr_anch_rect_t ar, uint32_t divisions, rlr_anch_rect_t* out_anchors);
void rlr_anch_rect_subdiv_hori(const rlr_anch_rect_t ar, uint32_t divisions, rlr_anch_rect_t* out_anchors);
void rlr_anch_rect_subdiv_matrix(const rlr_anch_rect_t ar, uint32_t horizontal_divisons, uint32_t vertical_divisions, rlr_anch_rect_t* out_anchors);

inline rlr_anch_rect_t rlr_anch_rect_outset(rlr_anch_rect_t ar, float amount) {
    return rlr_anch_rect_inset(ar, -amount);
}

inline rlr_anch_rect_t rlr_anch_rect_take_top(rlr_anch_rect_t ar, float amount) {
    return rlr_anch_rect_inset_bot(ar, ar.rect.height - amount);
}

inline rlr_anch_rect_t rlr_anch_rect_take_bottom(rlr_anch_rect_t ar, float amount) {
    return rlr_anch_rect_inset_top(ar, ar.rect.height - amount);
}

inline rlr_anch_rect_t rlr_anch_rect_take_left(rlr_anch_rect_t ar, float amount) {
    return rlr_anch_rect_inset_right(ar, ar.rect.width - amount);
}

inline rlr_anch_rect_t rlr_anch_rect_take_right(rlr_anch_rect_t ar, float amount) {
    return rlr_anch_rect_inset_left(ar, ar.rect.width - amount);
}