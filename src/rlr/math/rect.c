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