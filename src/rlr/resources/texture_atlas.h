#pragma once
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "../def.h"
#include "../math/vec.h"
#include "texture.h"

#define rlr_res_texture_atlas_tile(FILEPATH, DEST) \
    ((rlr_res_texture_atlas_tile_desc_t){.destination = (DEST), .filepath = (FILEPATH)})

#define rlr_res_texture_atlas_tile_size_padding(FILEPATH, DEST, W, H, PADDING) \
    ((rlr_res_texture_atlas_tile_desc_t){.destination = (DEST), .filepath = (FILEPATH), .padding = (PADDING), .resize_width = (W), .resize_height = (H)})

#define rlr_res_texture_atlas_tile_offset(FILEPATH, DEST, DEST_OFFSET) \
    ((rlr_res_texture_atlas_tile_desc_t){.destination = (DEST), .destination_offset = (DEST_OFFSET), .filepath = (FILEPATH)})

#define rlr_res_texture_atlas_tile_size_padding_offset(FILEPATH, DEST, DEST_OFFSET, W, H, PADDING) \
    ((rlr_res_texture_atlas_tile_desc_t){.destination = (DEST), .destination_offset = (DEST_OFFSET), .filepath = (FILEPATH), .padding = (PADDING), .resize_width = (W), .resize_height = (H)})

typedef struct rlr_res_texture_atlas_t rlr_res_texture_atlas_t;
typedef struct rlr_res_texture_atlas_tile_t rlr_res_texture_atlas_tile_t;

typedef struct rlr_res_texture_atlas_tile_desc_t {
    const char* filepath;           //source file location
    uint32_t resize_width;          //if non-zero, the source image will be resized to this width
    uint32_t resize_height;         //if non-zero, the source image will be resized to this height
    uint32_t padding;               //padding around the image. the padded area will contain extruded pixels from the edge of the source image
    void* destination;              //destination pointer, where the rlr_res_texture_atlas_tile_t pointer will be placed
    size_t destination_offset;      //taken into considering when setting the output rlr_res_texture_atlas_tile_t. Effectively destination + destination_offset
} rlr_res_texture_atlas_tile_desc_t;

rlr_res_texture_atlas_t* rlr_res_texture_atlas_create_instant(rlr_res_texture_atlas_tile_desc_t* descriptions, size_t description_count, uint32_t channels, bool srgb, rlr_res_texture_filter_t filter);
rlr_res_texture_atlas_t* rlr_res_texture_atlas_create_stepped(rlr_res_texture_atlas_tile_desc_t* descriptions, size_t description_count, uint32_t channels, bool srgb, rlr_res_texture_filter_t filter);

/*
    functions used for a stepped texture atlas
*/
size_t rlr_res_texture_atlas_get_step_count(const rlr_res_texture_atlas_t* atlas);
void rlr_res_texture_atlas_step(rlr_res_texture_atlas_t* atlas);
bool rlr_res_texture_atlas_is_loaded(const rlr_res_texture_atlas_t* atlas);


void rlr_res_texture_atlas_free(rlr_res_texture_atlas_t* atlas);