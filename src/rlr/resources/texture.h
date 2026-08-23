#pragma once
#include <stdbool.h>
#include <stdint.h>
#include "../def.h"

typedef struct cgltf_texture cgltf_texture;

typedef enum rlr_res_texture_filter_t {
    RLR_RES_TEXTURE_FILTER_NEAREST = 0,
    RLR_RES_TEXTURE_FILTER_LINEAR = 1,
    RLR_RES_TEXTURE_FILTER_LINEAR_MIPMAP = 2 /*recommended if using linear sampling*/,
} rlr_res_texture_filter_t;

typedef struct rlr_res_texture_t {
    uint64_t texture;
    uint32_t channels;
    float width;
    float height;
} rlr_res_texture_t;

rlr_res_t rlr_res_texture_load(const char* texture_path, bool use_srgb_color_space, rlr_res_texture_filter_t filter);
rlr_res_t rlr_res_texture_create_empty(rlr_vec2_t size, bool use_srgb_color_space, rlr_res_texture_filter_t filter, uint32_t channels);
void rlr_res_texture_copy_subtex(rlr_res_t texture_handle, const uint8_t* texture_data, uint32_t u, uint32_t v, uint32_t width, uint32_t height);
rlr_res_t rlr_res_texture_default();
rlr_res_t rlr_res_texture_load_cgltf_base(cgltf_texture* tex);
void rlr_res_texture_bind(rlr_res_t texture, uint8_t texture_slot);
void rlr_res_texture_free(rlr_res_t texture);