#pragma once
#include <stdint.h>

static inline float rlr_clampf(float value, float min, float max) {
    if(value > max) {
        return max;
    }
    if(value < min) {
        return min;
    }
    return value;
}

static inline int64_t rlr_clamp(int64_t value, int64_t min, int64_t max) {
    if(value > max) {
        return max;
    }
    if(value < min) {
        return min;
    }
    return value;
}

static inline float rlr_smoothstep(float t) {
    return t * t * (3.0 - 2.0 * t);
}