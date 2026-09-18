#include <stb_image_resize2.h>
#include <stb_image.h>
#include "../../internal/core/res_types.h"
#include "../../external/stb_ds.h"
#include "../../internal/impl.h"
#include "../error.h"
#include "texture_atlas.h"

rlr_res_texture_atlas_t* rlr_res_texture_atlas_create_fixed(rlr_vec2_t texture_size, rlr_vec2_t tile_size, bool use_srgb_color_space, rlr_res_texture_filter_t filter, uint32_t channels) {
    rlr_res_texture_atlas_t* texture_atlas = NULL;

    texture_atlas = malloc(sizeof(rlr_res_texture_atlas_t));
    if(!texture_atlas) {
        rlr_log_error("pointer to the texture atlas handle is null");
        goto err;
    }

    rlr_res_texture_t* texture_handle = rlr_res_texture_create_empty(texture_size, use_srgb_color_space, filter, channels);
    if(texture_handle == NULL) {
        rlr_log_error("unable to create new empty texture");
        goto err;
    }

    *texture_atlas = (rlr_res_texture_atlas_t){
        .filter = filter,
        .use_srgb_color_space = use_srgb_color_space,
        .tile_size = tile_size,
        .texture_size = texture_size,
        .tiles = NULL,
        .texture = texture_handle,
        .channels = channels,
    };

    return texture_atlas;
err:
    rlr_res_texture_atlas_free(texture_atlas);
    return NULL;
}

rlr_res_texture_atlas_tile_t* rlr_res_texture_atlas_add(rlr_res_texture_atlas_t* atlas, const char* filename) {
    uint8_t* tile_data = NULL;
    uint8_t* image_data = NULL;

    size_t columns = atlas->texture_size.x / atlas->tile_size.x;
    size_t rows = atlas->texture_size.y / atlas->tile_size.y;
    size_t tile_index = arrlenu(atlas->tiles);
    if (tile_index >= columns * rows) {
        rlr_log_error("cannot add tile because the texture atlas is full");
        goto err;
    }

    size_t pixel_x = (tile_index % columns) * atlas->tile_size.x;
    size_t pixel_y = (tile_index / columns) * atlas->tile_size.y;

    tile_data = malloc(atlas->tile_size.x * atlas->tile_size.y * atlas->channels);
    if(!tile_data) {
        rlr_log_error("could not allocate a buffer to resize a texture atlas tile");
        goto err;
    }

    int32_t width;
    int32_t height;
    image_data = stbi_load(filename, &width, &height, NULL, atlas->channels);
    if(!image_data) {
        rlr_log_error("could not load the image \"%s\"", filename);
        goto err;
    }

    uint8_t* res = NULL;
    if(atlas->use_srgb_color_space) {
        res = stbir_resize_uint8_srgb(image_data, width, height, 0, tile_data, atlas->tile_size.x, atlas->tile_size.y, 0, atlas->channels);
    } else {
        res = stbir_resize_uint8_linear(image_data, width, height, 0, tile_data, atlas->tile_size.x, atlas->tile_size.y, 0, atlas->channels);
    }

    if(!res) {
        rlr_log_warning("could not resize image");
        goto err;
    }

    //copy image
    rlr_res_texture_copy_subtex(atlas->texture, tile_data, pixel_x, pixel_y, atlas->tile_size.x, atlas->tile_size.y);

    rlr_res_texture_atlas_tile_t tile = {
        .size = atlas->tile_size,
        .texture = atlas->texture,
        .uv = rlr_vec2(pixel_x, pixel_y),
    };
    arrpush(atlas->tiles, tile);
    rlr_res_texture_atlas_tile_t* tile_ptr = &arrlast(atlas->tiles);
    if(!tile_ptr) {
        goto err;
    }
    
    free(tile_data);
    stbi_image_free(image_data);
    return tile_ptr;
err:
    free(tile_data);
    stbi_image_free(image_data);
    return NULL;
}

void rlr_res_texture_atlas_free(rlr_res_texture_atlas_t* texture_atlas) {
    if(!texture_atlas) {
        return;
    }
    rlr_res_texture_free(texture_atlas->texture);
    arrfree(texture_atlas->tiles);
    free(texture_atlas);
}