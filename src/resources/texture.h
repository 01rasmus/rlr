#pragma once
#include <stdbool.h>
#include <stdint.h>

typedef struct rlr_texture_t {
    uint64_t texture;
    float width;
    float height;
} rlr_texture_t;

rlr_texture_t* rlr_texture_create(const char* texture_path, bool generate_mipmaps);
void rlr_texture_use(rlr_texture_t* texture);
void rlr_texture_free(rlr_texture_t* texture);