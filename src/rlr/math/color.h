#pragma once
#include <stdint.h>

typedef struct rlr_color_t {
    uint8_t r;
    uint8_t g;
    uint8_t b;
    uint8_t a;
} rlr_color_t;

#define RLR_COLOR_WHITE \
    rlr_color_rgb(255, 255, 255)

#define RLR_COLOR_BLACK \
    rlr_color_rgb(0, 0, 0)

rlr_color_t rlr_color_hex(const char hex[8]);
rlr_color_t rlr_color_rgb(uint8_t r, uint8_t g, uint8_t b);
rlr_color_t rlr_color_rgba(uint8_t r, uint8_t g, uint8_t b, uint8_t a);
uint32_t rlr_color_as_uint(rlr_color_t color);