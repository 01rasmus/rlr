#pragma once
#include <stdbool.h>
#include <stdint.h>

typedef uint64_t rlr_res_t;

typedef struct rlr_statistics_t {
    uint64_t draw_call_count;
    uint64_t frame_count;
    double time;
} rlr_statistics_t;

typedef enum rlr_init_flags_t {
    RLR_INIT_FLAG_FULLSCREEN = 0x1,
    RLR_INIT_FLAG_VSYNC = 0x2,
} rlr_init_flags_t;

void rlr_init(const char* title, uint32_t window_width, uint32_t window_height, uint64_t flags);
const char* rlr_get_backend_implementation();
rlr_statistics_t* rlr_get_total_statistics();
rlr_statistics_t* rlr_get_statistics();
bool rlr_update();
void rlr_free();