#pragma once
#include "../def.h"
#include "../math/vec.h"
#include "texture.h"

typedef struct rlr_res_texture_atlas_t rlr_res_texture_atlas_t;
typedef struct rlr_res_texture_atlas_tile_t rlr_res_texture_atlas_tile_t;

/*
    creates a texture atlas with fixed sized tiles.
    all added tiles are resized into the given tile size
*/
rlr_res_texture_atlas_t* rlr_res_texture_atlas_create_fixed(rlr_vec2_t texture_size, rlr_vec2_t tile_size, bool use_srgb_color_space, rlr_res_texture_filter_t filter, uint32_t channels);

/*
    loads and resizes a texture and puts it into the
    texture atlas' texture. Returns a texture atlas tile
*/
rlr_res_texture_atlas_tile_t* rlr_res_texture_atlas_add(rlr_res_texture_atlas_t* texture_atlas, const char* filename);

/*
    frees a texture atlas and all of the added
    tiles and their handles will also become invalid
*/
void rlr_res_texture_atlas_free(rlr_res_texture_atlas_t* texture_atlas);