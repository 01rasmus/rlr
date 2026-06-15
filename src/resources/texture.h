#pragma once
#include <stdbool.h>
#include <stdint.h>
#include <cgltf.h>
#include "backends/backend.h"

typedef struct rlr_texture_t {
    rlr_handle_t texture;
    float width;
    float height;
} rlr_texture_t;

rlr_texture_t* rlr_texture_load(const char* texture_path, bool generate_mipmaps, bool use_srgb_color_space);
rlr_texture_t* rlr_texture_default();
rlr_texture_t* rlr_texture_load_cgltf_base(cgltf_texture* tex);
void rlr_texture_bind(rlr_texture_t* texture, uint8_t texture_slot);
void rlr_texture_free(rlr_texture_t* texture);