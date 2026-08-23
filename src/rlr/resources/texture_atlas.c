#include <stb_image_resize2.h>
#include <stb_image.h>
#include "../../external/stb_ds.h"
#include "../../internal/impl.h"
#include "../error.h"
#include "texture_atlas.h"

rlr_res_t rlr_res_texture_atlas_create_fixed(rlr_vec2_t texture_size, rlr_vec2_t tile_size, bool use_srgb_color_space, rlr_res_texture_filter_t filter, uint32_t channels) {
    rlr_res_t id = RLR_NULL;
    rlr_res_texture_atlas_t* texture_atlas = NULL;

    id = rlr_mem_man_allocate_res_texture_atlas(rlr_mem_man(), (rlr_res_texture_atlas_t){0});
    if(id == RLR_NULL) {
        rlr_log_error("could not allocate rlr handle for texture atlas");
        goto err;
    }

    texture_atlas = rlr_mem_man_get_res_texture_atlas(rlr_mem_man(), id);
    if(!texture_atlas) {
        rlr_log_error("pointer to the texture atlas handle is null");
        goto err;
    }

    rlr_res_t texture_handle = rlr_res_texture_create_empty(texture_size, use_srgb_color_space, filter, channels);
    if(texture_handle == RLR_NULL) {
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

    return id;
err:
    rlr_res_texture_atlas_free(id);
    return RLR_NULL;
}

rlr_res_t rlr_res_texture_atlas_add(rlr_res_t texture_atlas_handle, const char* filename) {
    rlr_res_t id = RLR_NULL;
    rlr_res_texture_atlas_tile_t* tile = NULL;
    rlr_res_texture_atlas_t* atlas = NULL;
    uint8_t* tile_data = NULL;
    uint8_t* image_data = NULL;

    atlas = rlr_mem_man_get_res_texture_atlas(rlr_mem_man(), texture_atlas_handle);
    if(!atlas) {
        rlr_log_error("provided texture atlas handle is invalid");
        goto err;
    }

    id = rlr_mem_man_allocate_res_texture_atlas_tile(rlr_mem_man(), (rlr_res_texture_atlas_tile_t){0});
    if(id == RLR_NULL) {
        rlr_log_error("could not allocate rlr handle for texture atlas tile");
        goto err;
    }

    tile = rlr_mem_man_get_res_texture_atlas_tile(rlr_mem_man(), id);
    if(!tile) {
        rlr_log_error("pointer to the texture atlas tile handle is null");
        goto err;
    }

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

    *tile = (rlr_res_texture_atlas_tile_t){
        .size = atlas->tile_size,
        .texture = atlas->texture,
        .uv = rlr_vec2(pixel_x, pixel_y),
    };

    arrpush(atlas->tiles, id);
    free(tile_data);
    stbi_image_free(image_data);
    return id;
err:
    free(tile_data);
    stbi_image_free(image_data);
    if(id != RLR_NULL) {
        rlr_mem_man_free_res_texture_atlas_tile(rlr_mem_man(), id);
    }
    return RLR_NULL;
}

void rlr_res_texture_atlas_free(rlr_res_t texture_atlas_handle) {
    if(texture_atlas_handle == RLR_NULL) {
        return;
    }
    rlr_res_texture_atlas_t* texture_atlas = rlr_mem_man_get_res_texture_atlas(rlr_mem_man(), texture_atlas_handle);
    if(texture_atlas) {
        rlr_res_texture_free(texture_atlas->texture);

        //free all the tiles too
        for(size_t i = 0; i < arrlenu(texture_atlas->tiles); i++) {
            rlr_mem_man_free_res_texture_atlas_tile(rlr_mem_man(), texture_atlas->tiles[i]);
        } 
        arrfree(texture_atlas->tiles);
    }
    rlr_mem_man_free_res_texture_atlas(rlr_mem_man(), texture_atlas_handle);
}