#pragma once
#include <stdbool.h>
#include <stdint.h>

typedef struct rlr_backend_t rlr_backend_t;

typedef struct rlr_statistics_t {
    uint64_t draw_call_count;
    uint64_t frame_count;
    double time;
} rlr_statistics_t;

typedef enum rlr_init_flags_t {
    RLR_INIT_FLAG_FULLSCREEN = 0x1,
} rlr_init_flags_t;

void rlr_init(const char* title, uint32_t window_width, uint32_t window_height, uint64_t flags);
rlr_statistics_t* rlr_get_statistics_total();
rlr_statistics_t* rlr_get_statistics();
bool rlr_draw();
void rlr_free();