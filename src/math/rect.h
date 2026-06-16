#pragma once

typedef struct rlr_rect_t {
    float x;
    float y;
    float width;
    float height;
} rlr_rect_t;

#define rlr_rect(X, Y, WIDTH, HEIGHT)           ((rlr_rect_t){.x = (X), .y = (Y), .width = (WIDTH), .height = (HEIGHT)})