#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "color.h"

rlr_color_t rlr_color_hex(const char hex[8]) {
    rlr_color_t default_color = RLR_COLOR_WHITE;
    if(hex[0] != '#' || hex[7] != '\0') {
        return default_color;
    }

    char* end;
    unsigned long long val = strtoull(hex + 1, &end, 16);
    if(errno == ERANGE || val > UINT32_MAX || *end != '\0') {
        return default_color;
    }

    return (rlr_color_t){
        .r = (uint8_t)(val >> 16),
        .g = (uint8_t)(val >> 8),
        .b = (uint8_t)val,
        .a = 255
    };
}

rlr_color_t rlr_color_rgb(uint8_t r, uint8_t g, uint8_t b) {
    return (rlr_color_t){
        .r = r,
        .g = g,
        .b = b,
        .a = 255,
    };
}

rlr_color_t rlr_color_rgba(uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
    return (rlr_color_t){
        .r = r,
        .g = g,
        .b = b,
        .a = a,
    };
}

uint32_t rlr_color_as_uint(rlr_color_t color) {
    uint32_t as_uint;
    memcpy(&as_uint, &color, sizeof(uint32_t));
    return as_uint;
}