#pragma once
#include <stdint.h>

typedef enum rlr_storage_dir_t {
    RLR_STORAEG_DIR_CONFIG,
    RLR_STORAEG_DIR_CACHE,
    RLR_STORAEG_DIR_DATA
} rlr_storage_dir_t;

int32_t rlr_create_dir(rlr_storage_dir_t base_dir, const char* dir, ...);