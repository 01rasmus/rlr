#include "rect.h"

bool rlr_rect_contains_position(const rlr_rect_t* rect, const rlr_vec2_t pos) {
    float x1 = rect->x;
    float x2 = rect->x + rect->width;
    float y1 = rect->y;
    float y2 = rect->y + rect->height;

    if(pos.x > x2) {
        return false;
    }
    if(pos.x < x1) {
        return false;
    }
    if(pos.y > y2) {
        return false;
    }
    if(pos.y < y1) {
        return false;
    }
    return true;
}

rlr_anchored_rect_t rlr_anchored_rect_inset(rlr_anchored_rect_t ar, float amount) {
    rlr_vec2_t anchor = rlr_anchor_vec(ar.anchor);
    ar.rect.x += amount * (1.0 - 2.0 * anchor.x);
    ar.rect.y += amount * (1.0 - 2.0 * anchor.y);
    ar.rect.width -= amount * 2.0f;
    ar.rect.height -= amount * 2.0f;
    return ar;
}

rlr_anchored_rect_t rlr_anchored_rect_inset_top(rlr_anchored_rect_t ar, float amount) {
    rlr_vec2_t anchor = rlr_anchor_vec(ar.anchor);
    ar.rect.y += amount * (1.0 - anchor.y);
    ar.rect.height -= amount;
    return ar;
}

rlr_anchored_rect_t rlr_anchored_rect_inset_bottom(rlr_anchored_rect_t ar, float amount) {
    rlr_vec2_t anchor = rlr_anchor_vec(ar.anchor);
    ar.rect.y -= amount * anchor.y;
    ar.rect.height -= amount;
    return ar;
}

rlr_anchored_rect_t rlr_anchored_rect_inset_left(rlr_anchored_rect_t ar, float amount) {
    rlr_vec2_t anchor = rlr_anchor_vec(ar.anchor);
    ar.rect.x += amount * (1.0 - anchor.x);
    ar.rect.width -= amount;
    return ar;
}

rlr_anchored_rect_t rlr_anchored_rect_inset_right(rlr_anchored_rect_t ar, float amount) {
    rlr_vec2_t anchor = rlr_anchor_vec(ar.anchor);
    ar.rect.x -= amount * anchor.x;
    ar.rect.width -= amount;
    return ar;
}