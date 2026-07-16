#pragma once
#include <stdbool.h>
#include <stdint.h>
#include "math/vec.h"
#include "def.h"

typedef struct rlr_statistics_t {
    uint64_t draw_call_count;
    uint64_t frame_count;
    double time;
} rlr_statistics_t;

void rlr_init(const char* title, uint32_t window_width, uint32_t window_height, rlr_init_flags_t flags);
const char* rlr_get_backend_implementation();
const char* rlr_get_backend_context();
const char* rlr_get_gpu_name();
rlr_vec2_t rlr_get_framebuffer_size();
rlr_statistics_t* rlr_get_total_statistics();
rlr_statistics_t* rlr_get_statistics();
bool rlr_update();
void rlr_free();