#include "rect.h"

static bool _rlr_rect_contains_pos(float x, float y, float w, float h, const rlr_vec2_t pos) {
    float x1 = x;
    float x2 = x + w;
    float y1 = y;
    float y2 = y + h;

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

bool rlr_rect_contains_pos(const rlr_rect_t* rect, rlr_vec2_t pos, rlr_vec2_t screen_size) {
    rlr_vec2_t screen_anchor_vec = rlr_vec2_mul(rlr_anchor_vec(rect->screen_anchor), screen_size);
    rlr_vec2_t rect_anchor_vec = rlr_vec2_mul(rlr_anchor_vec(rect->local_anchor), rlr_vec2(rect->width, rect->height));
    return _rlr_rect_contains_pos(
        rect->x + screen_anchor_vec.x - rect_anchor_vec.x,
        rect->y + screen_anchor_vec.y - rect_anchor_vec.y,
        rect->width,
        rect->height,
        pos
    );
}

rlr_rect_t rlr_rect_inset(rlr_rect_t rect, float amount) {
    rlr_vec2_t vec = rlr_anchor_vec(rect.local_anchor);
    rect.width -= amount * 2.0;
    rect.height -= amount * 2.0;
    rect.x += (1.0 - vec.x * 2.0) * amount;
    rect.y += (1.0 - vec.y * 2.0) * amount;
    return rect;
}

rlr_rect_t rlr_rect_inset_vert(rlr_rect_t rect, float amount) {
    rlr_vec2_t vec = rlr_anchor_vec(rect.local_anchor);
    rect.height -= amount * 2.0;
    rect.y += (1.0 - vec.y * 2.0) * amount;
    return rect;
}

rlr_rect_t rlr_rect_inset_hori(rlr_rect_t rect, float amount) {
    rlr_vec2_t vec = rlr_anchor_vec(rect.local_anchor);
    rect.width -= amount * 2.0;
    rect.x += (1.0 - vec.x * 2.0) * amount;
    return rect;
}

rlr_rect_t rlr_rect_inset_top(rlr_rect_t rect, float amount) {
    rect.y += (1.0 - rlr_anchor_y(rect.local_anchor)) * amount;
    rect.height -= amount;
    return rect;
}

rlr_rect_t rlr_rect_inset_bot(rlr_rect_t rect, float amount) {
    rect.y -= rlr_anchor_y(rect.local_anchor) * amount;
    rect.height -= amount;
    return rect;
}

rlr_rect_t rlr_rect_inset_left(rlr_rect_t rect, float amount) {
    rect.x += (1.0 - rlr_anchor_x(rect.local_anchor)) * amount;
    rect.width -= amount;
    return rect;
}

rlr_rect_t rlr_rect_inset_right(rlr_rect_t rect, float amount) {
    rect.x -= rlr_anchor_x(rect.local_anchor) * amount;
    rect.width -= amount;
    return rect;
}

void rlr_rect_subdiv_vert(const rlr_rect_t rect, uint32_t divisions, rlr_rect_t* out_rects) {
    if(divisions == 0) {
        return;
    }

    rlr_rect_t remaining = rect;
    float segment_size = rect.height / (float)divisions;

    for(uint32_t i = 0; i < divisions - 1; i++) {
        out_rects[i] = rlr_rect_take_top(remaining, segment_size);
        remaining = rlr_rect_inset_top(remaining, segment_size);
    }

    out_rects[divisions - 1] = rlr_rect_take_top(remaining, segment_size);
}

void rlr_rect_subdiv_hori(const rlr_rect_t rect, uint32_t divisions, rlr_rect_t* out_rects) {
    if(divisions == 0) {
        return;
    }

    rlr_rect_t remaining = rect;
    float segment_size = rect.width / (float)divisions;

    for(uint32_t i = 0; i < divisions - 1; i++) {
        out_rects[i] = rlr_rect_take_left(remaining, segment_size);
        remaining = rlr_rect_inset_left(remaining, segment_size);
    }

    out_rects[divisions - 1] = rlr_rect_take_left(remaining, segment_size);
}

void rlr_rect_subdiv_matrix(const rlr_rect_t rect, uint32_t horizontal_divisons, uint32_t vertical_divisions, rlr_rect_t* out_rects) {
    float seg_width = rect.width / (float)horizontal_divisons;
    float seg_height = rect.height / (float)vertical_divisions;

    rlr_rect_t remaining_vertically = rect;
    for(uint32_t y = 0; y < vertical_divisions; y++) {

        rlr_rect_t remaining_horizontal = rlr_rect_take_top(remaining_vertically, seg_height);
        remaining_vertically = rlr_rect_inset_top(remaining_vertically, seg_height);
        for(uint32_t x = 0; x < horizontal_divisons; x++) {
            out_rects[y * horizontal_divisons + x] = rlr_rect_take_left(remaining_horizontal, seg_width);
            remaining_horizontal = rlr_rect_inset_left(remaining_horizontal, seg_width);
        }
    }
}

void rlr_rect_split_hori(const rlr_rect_t rect, float ratio, rlr_rect_t out_rects[2]) {
    float take_left = rect.width * ratio;
    out_rects[0] = rlr_rect_take_left(rect, take_left);
    out_rects[1] = rlr_rect_inset_left(rect, take_left);
}

void rlr_rect_split_vert(const rlr_rect_t rect, float ratio, rlr_rect_t out_rects[2]) {
    float take_top = rect.height * ratio;
    out_rects[0] = rlr_rect_take_top(rect, take_top);
    out_rects[1] = rlr_rect_inset_top(rect, take_top);
}