#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "color.h"

rlr_col_t rlr_col_hex(const char hex[8]) {
    rlr_col_t default_color = RLR_COL_WHITE;
    if(hex[0] != '#' || hex[7] != '\0') {
        return default_color;
    }

    char* end;
    unsigned long long val = strtoull(hex + 1, &end, 16);
    if(errno == ERANGE || val > UINT32_MAX || *end != '\0') {
        return default_color;
    }

    return (rlr_col_t){
        .r = (uint8_t)(val >> 16),
        .g = (uint8_t)(val >> 8),
        .b = (uint8_t)val,
        .a = 255
    };
}

uint32_t rlr_col_as_u32(rlr_col_t color) {
    uint32_t as_uint;
    memcpy(&as_uint, &color, sizeof(uint32_t));
    return as_uint;
}