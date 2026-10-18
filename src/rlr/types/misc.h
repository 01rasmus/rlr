#pragma once
#include <stdint.h>
#include "log.h"

#define RLR_NULL    ((uint64_t)0u)

typedef uint64_t rlr_handle_t;
typedef struct rlr_t rlr_t;

typedef struct rlr_statistics_t {
    uint64_t draw_call_count;
    uint64_t frame_count;
    double time;
} rlr_statistics_t;

typedef enum rlr_setting_animation_evaluation_t {
    RLR_SETTING_ANIMATION_EVALUATION_AUTOMATIC = 0,
    RLR_SETTING_ANIMATION_EVALUATION_GPU,
    RLR_SETTING_ANIMATION_EVALUATION_CPU,
} rlr_setting_animation_evaluation_t;

typedef enum rlr_init_flag_t {
    RLR_INIT_FLAG_FULLSCREEN    = 0x1,
    RLR_INIT_FLAG_VSYNC         = 0x2,
} rlr_init_flag_t;