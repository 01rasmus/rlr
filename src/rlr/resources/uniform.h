#pragma once
#include <stdint.h>
#include <stdbool.h>
#include "internal/backends/backend.h"

typedef struct rlr_res_uniform_t {
    rlr_handle_t buffer;
    uint64_t max_size;
    bool is_static;
    uint8_t slot;
} rlr_res_uniform_t;

rlr_res_uniform_t* rlr_res_uniform_create_dynamic(uint64_t size);
rlr_res_uniform_t* rlr_res_uniform_create_static(void* data, uint64_t size);

/*
    if the uniform is static, this function does nothing
*/
void rlr_res_uniform_update(rlr_res_uniform_t* ubo, uint64_t offset, void* data, uint64_t size);

void rlr_res_uniform_bind(rlr_res_uniform_t* ubo, uint8_t ubo_slot);
void rlr_res_uniform_free(rlr_res_uniform_t* ubo);