#pragma once
#include <stdint.h>

typedef struct rlr_sampler_t {
    uint32_t sampler;
} rlr_sampler_t;

rlr_sampler_t* rlr_sampler_create(uint32_t min, uint32_t mag, uint32_t wrap_s, uint32_t wrap_t);
void rlr_sampler_free(rlr_sampler_t* sampler);