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
    rlr_vec2_t vec = rlr_anchor_vec(ar.anchor);
    ar.rect.width -= amount * 2.0;
    ar.rect.height -= amount * 2.0;
    ar.rect.x += (1.0 - vec.x * 2.0) * amount;
    ar.rect.y += (1.0 - vec.y * 2.0) * amount;
    return ar;
}

rlr_anchored_rect_t rlr_anchored_rect_inset_top(rlr_anchored_rect_t ar, float amount) {
    ar.rect.y += (1.0 - rlr_anchor_y(ar.anchor)) * amount;
    ar.rect.height -= amount;
    return ar;
}

rlr_anchored_rect_t rlr_anchored_rect_inset_bottom(rlr_anchored_rect_t ar, float amount) {
    ar.rect.y -= rlr_anchor_y(ar.anchor) * amount;
    ar.rect.height -= amount;
    return ar;
}

rlr_anchored_rect_t rlr_anchored_rect_inset_left(rlr_anchored_rect_t ar, float amount) {
    ar.rect.x += (1.0 - rlr_anchor_x(ar.anchor)) * amount;
    ar.rect.width -= amount;
    return ar;
}

rlr_anchored_rect_t rlr_anchored_rect_inset_right(rlr_anchored_rect_t ar, float amount) {
    ar.rect.x -= rlr_anchor_x(ar.anchor) * amount;
    ar.rect.width -= amount;
    return ar;
}

void rlr_anchored_rect_subdivide_vertically(const rlr_anchored_rect_t ar, uint32_t divisions, rlr_anchored_rect_t* out_anchors) {
    if(divisions == 0) {
        return;
    }

    rlr_anchored_rect_t remaining = ar;
    float segment_size = ar.rect.height / (float)divisions;

    for(uint32_t i = 0; i < divisions - 1; i++) {
        out_anchors[i] = rlr_anchored_rect_take_top(remaining, segment_size);
        remaining = rlr_anchored_rect_inset_top(remaining, segment_size);
    }

    out_anchors[divisions - 1] = rlr_anchored_rect_take_top(remaining, segment_size);
}

void rlr_anchored_rect_subdivide_horizontally(const rlr_anchored_rect_t ar, uint32_t divisions, rlr_anchored_rect_t* out_anchors) {
    if(divisions == 0) {
        return;
    }

    rlr_anchored_rect_t remaining = ar;
    float segment_size = ar.rect.width / (float)divisions;

    for(uint32_t i = 0; i < divisions - 1; i++) {
        out_anchors[i] = rlr_anchored_rect_take_left(remaining, segment_size);
        remaining = rlr_anchored_rect_inset_left(remaining, segment_size);
    }

    out_anchors[divisions - 1] = rlr_anchored_rect_take_left(remaining, segment_size);
}

void rlr_anchored_rect_subdivide_matrix(const rlr_anchored_rect_t ar, uint32_t horizontal_divisons, uint32_t vertical_divisions, rlr_anchored_rect_t* out_anchors) {
    float seg_width = ar.rect.width / (float)horizontal_divisons;
    float seg_height = ar.rect.height / (float)vertical_divisions;

    rlr_anchored_rect_t remaining_vertically = ar;
    for(uint32_t y = 0; y < vertical_divisions; y++) {

        rlr_anchored_rect_t remaining_horizontal = rlr_anchored_rect_take_top(remaining_vertically, seg_height);
        remaining_vertically = rlr_anchored_rect_inset_top(remaining_vertically, seg_height);
        for(uint32_t x = 0; x < horizontal_divisons; x++) {
            out_anchors[y * horizontal_divisons + x] = rlr_anchored_rect_take_left(remaining_horizontal, seg_width);
            remaining_horizontal = rlr_anchored_rect_inset_left(remaining_horizontal, seg_width);
        }
    }
}