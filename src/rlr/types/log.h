#pragma once
#include <stdint.h>

#define RLR_LOG_LEVEL_INFO              0x0
#define RLR_LOG_LEVEL_WARNING           0x1
#define RLR_LOG_LEVEL_ERROR             0x2
#define RLR_LOG_LEVEL_DEBUG             0x3

typedef uint8_t rlr_log_level_t;

typedef void (*rlr_log_callback_t)(rlr_log_level_t log_level, const char* log, const char* file, uint64_t line, void* user);