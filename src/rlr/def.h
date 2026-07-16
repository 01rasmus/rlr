#pragma once
#include <stdint.h>
#include "../external/rlpp.h"

#define RLR_DEBUG   1
#define RLR_NULL    ((uint64_t)0u)

typedef rlpp_id_t rlr_res_t;
typedef rlpp_id_t rlr_obj_t;
typedef uint64_t rlr_handle_t;

typedef enum rlr_init_flags_t {
    RLR_INIT_FLAG_FULLSCREEN = 0x1,
    RLR_INIT_FLAG_VSYNC = 0x2,
} rlr_init_flags_t;