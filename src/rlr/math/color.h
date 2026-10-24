#pragma once
#include <stdint.h>

typedef struct rlr_col_t {
    uint8_t r;
    uint8_t g;
    uint8_t b;
    uint8_t a;
} rlr_col_t;

#define RLR_COL_WHITE \
    rlr_col_rgb(255, 255, 255)

#define RLR_COL_BLACK \
    rlr_col_rgb(0, 0, 0)

rlr_col_t rlr_col_hex(const char hex[8]);
uint32_t rlr_col_as_u32(rlr_col_t color);

static inline rlr_col_t rlr_col_gray(uint8_t gray) {
    return (rlr_col_t){
        .r = gray,
        .g = gray,
        .b = gray,
        .a = 255
    };
}

static inline rlr_col_t rlr_col_rgb(uint8_t r, uint8_t g, uint8_t b) {
    return (rlr_col_t){
        .r = r,
        .g = g,
        .b = b,
        .a = 255,
    };
}

static inline rlr_col_t rlr_col_rgba(uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
    return (rlr_col_t){
        .r = r,
        .g = g,
        .b = b,
        .a = a,
    };
}