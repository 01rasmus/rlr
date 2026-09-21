#include <stb_image_resize2.h>
#include <stb_image.h>
#include "../../internal/core/res_types.h"
#include "../../external/stb_ds.h"
#include "../../internal/impl.h"
#include "../error.h"
#include "texture_atlas.h"

#define STB_RECT_PACK_IMPLEMENTATION
#define STBRP_STATIC
#include <stb_rect_pack.h>

/*
    next power of two
*/
static inline uint32_t npo2(uint32_t x) {
    if(x <= 1) {
        return 1;
    }
    x--;
    x |= x >> 1;
    x |= x >> 2;
    x |= x >> 4;
    x |= x >> 8;
    x |= x >> 16;
    return x + 1;
}

static inline void reset_rects(stbrp_rect* rects, rlr_res_texture_atlas_tile_step_ctx_t* steps, size_t count) {
    for(size_t i = 0; i < count; i++) {
        rects[i].id = (int32_t)i;
        rects[i].x = 0;
        rects[i].y = 0;
        rects[i].w = steps[i].external_width;
        rects[i].h = steps[i].external_height;
    }
}

static inline void atlas_done_loading(rlr_res_texture_atlas_t* atlas) {
    //we are done, create gpu texture
    atlas->texture = rlr_res_texture_load_from_memory(atlas->temp_texture, atlas->texture_side_size, atlas->texture_side_size, atlas->channels, atlas->use_srgb_color_space, atlas->filter);
    if(!atlas->texture) {
        rlr_res_texture_atlas_free(atlas);
        return;
    }

    //fill texture on all the tiles
    for(size_t i = 0; i < arrlenu(atlas->tiles); i++) {
        rlr_res_texture_atlas_tile_t* tile = &atlas->tiles[i];
        tile->texture = atlas->texture;
    }

    //clenup step variables
    free(atlas->temp_texture);
    arrfree(atlas->steps);
    atlas->temp_texture = NULL;
    atlas->steps = NULL;
    atlas->is_loaded = true;
}

static inline void insert_texture_tile(const char* file, uint8_t* destination, uint32_t dest_x, uint32_t dest_y, uint32_t dest_width, uint32_t dest_height, uint32_t dest_channels, uint32_t side, bool srgb) {
    uint8_t* image_data = NULL;
    if(!file) {
        goto done;
    }

    int32_t width;
    int32_t height;
    image_data = stbi_load(file, &width, &height, NULL, dest_channels);
    if(!image_data) {
        rlr_log_error("could not load the image \"%s\"", file);
        goto done;
    }

    uint8_t* res = NULL;
    size_t offset = ((size_t)dest_y * side + dest_x) * dest_channels;
    uint8_t* dest_ptr = destination + offset;
    if(srgb) {
        res = stbir_resize_uint8_srgb(image_data, width, height, 0, dest_ptr, dest_width, dest_height, side * dest_channels, dest_channels);
    } else {
        res = stbir_resize_uint8_linear(image_data, width, height, 0, dest_ptr, dest_width, dest_height, side * dest_channels, dest_channels);
    }

    if(!res) {
        rlr_log_warning("could not resize image \"%s\"", file);
        goto done;
    }

done:
    stbi_image_free(image_data);
}

static void extrude_texture_tile(uint8_t* data, uint32_t x, uint32_t y, uint32_t width, uint32_t height, uint32_t padding, uint32_t channels, uint32_t side) {
    size_t stride = (size_t)side * channels;

    //horizontal extrusion
    for(uint32_t row = 0; row < height; row++) {
        uint8_t* row_ptr = data + (((size_t)(y + row) * side + x) * channels);
        uint8_t* left_pixel = row_ptr;
        uint8_t* right_pixel = row_ptr + ((size_t)(width - 1) * channels);

        for(uint32_t p = 1; p <= padding; p++) {
            memcpy(left_pixel - ((size_t)p * channels), left_pixel, channels);
            memcpy(right_pixel + ((size_t)p * channels), right_pixel, channels);
        }
    }

    size_t padded_width = (size_t)width + (padding * 2);
    size_t row_bytes = padded_width * channels;

    uint8_t* first_row = data + (((size_t)y * side + (x - padding)) * channels);
    uint8_t* last_row = data + (((size_t)(y + height - 1) * side + (x - padding)) * channels);
    for (uint32_t p = 1; p <= padding; p++) {
        memcpy(first_row - ((size_t)p * stride), first_row, row_bytes);
        memcpy(last_row + ((size_t)p * stride), last_row, row_bytes);
    }
}

rlr_res_texture_atlas_t* rlr_res_texture_atlas_create_instant(rlr_res_texture_atlas_tile_desc_t* descriptions, size_t description_count, uint32_t channels, bool srgb, rlr_res_texture_filter_t filter) {
    rlr_res_texture_atlas_t* atlas = rlr_res_texture_atlas_create_instant(descriptions, description_count, channels, srgb, filter);
    if(!atlas) {
        return NULL;
    }

    while(!rlr_res_texture_atlas_is_loaded(atlas)) {
        rlr_res_texture_atlas_step(atlas);
    }

    return atlas;
}

rlr_res_texture_atlas_t* rlr_res_texture_atlas_create_stepped(rlr_res_texture_atlas_tile_desc_t* descriptions, size_t description_count, uint32_t channels, bool srgb, rlr_res_texture_filter_t filter) {
    stbrp_rect* rects = NULL;
    stbrp_node* nodes = NULL;
    rlr_res_texture_atlas_t* atlas = malloc(sizeof(rlr_res_texture_atlas_t));
    if(!atlas) {
        goto err;
    }

    rects = malloc(sizeof(stbrp_rect) * description_count);
    if(!rects) {
        goto err;
    }

    *atlas = (rlr_res_texture_atlas_t){
        .texture = NULL,
        .channels = channels,
        .use_srgb_color_space = srgb,
        .filter = filter,
        .tiles = NULL,
        .temp_texture = NULL,
        .steps = NULL,
        .step_count = description_count,
        .current_step = 0,
        .is_loaded = false,
        .texture_side_size = 0
    };

    //get area and sizes of the tiles that are to be loaded
    double area = 0.0;
    for(size_t i = 0; i < description_count; i++) {
        rlr_res_texture_atlas_tile_desc_t* desc = &descriptions[i];

        //get the size of the image
        uint32_t rw = desc->resize_width;
        uint32_t rh = desc->resize_height;
        bool loaded = true;
        if(rw == 0 || rh == 0) {
            uint32_t original_w = 1;
            uint32_t original_h = 1;
            if(!stbi_info(desc->filepath, &original_w, &original_h, NULL)) {
                rlr_log_error("could not load atlas tile info from \"%s\"", desc->filepath);
                loaded = false;

                //it failed, but we are gonna default to a deafult texture
                rw = 1;
                rh = 1;
            } else {
                if(rw == 0) {
                    rw = original_w;
                }
                if(rh == 0) {
                    rh = original_h;
                }
            }
        }

        //set padding automatically if it's not set
        uint32_t padding = desc->padding;
        if(atlas->filter != RLR_RES_TEXTURE_FILTER_NEAREST && padding == 0) {
            uint32_t size = rw < rh ? rw : rh;
            if(size <= 16) {
                padding = 2;
            } else if(size <= 64) {
                padding = 4;
            } else if(size <= 128) {
                padding = 8;
            } else {
                padding = 16;
            }
        }

        //add to the area
        uint32_t ew = rw + padding * 2;
        uint32_t eh = rh + padding * 2;
        area += (double)ew + (double)eh;

        //add step entry
        rlr_res_texture_atlas_tile_step_ctx_t ctx = {
            .filepath = loaded ? desc->filepath : NULL,
            .dest = desc->destination,
            .dest_offset = desc->destination_offset,
            .external_width = ew,
            .external_height = eh,
            .internal_width = rw,
            .internal_height = rh
        };
        arrpush(atlas->steps, ctx);
        atlas->step_count++;
    }

    //calculate the atlas size
    uint32_t side = npo2((uint32_t)ceil(sqrt(area)));
    const uint32_t max_side_size = 4096;
    if(side > max_side_size) {
        rlr_log_error("the guessed atlas side of %u is larger than the maximum of %u", side, max_side_size);
        goto err;
    }

    //find the best size
    bool packed = false;
    while(side <= max_side_size) {

        //reset rectangles
        reset_rects(rects, atlas->steps, description_count);

        //allocate and pack
        stbrp_context context = {0};
        nodes = malloc(sizeof(stbrp_node) * side);
        if(!nodes) {
            rlr_log_error("could not allocate memory for rect pack nodes");
            goto err;
        }
        
        stbrp_init_target(&context, side, side, nodes, side);
        stbrp_pack_rects(&context, rects, (int32_t)description_count);

        //check if it was successful
        packed = true;
        for(size_t i = 0; i < description_count; i++) {
            if(!rects[i].was_packed) {
                packed = false;
                break;
            }
        }

        free(nodes);
        nodes = NULL;
        if(!packed) {
            side *= 2;
        } else {
            break;
        }
    }

    if(!packed) {
        rlr_log_error("could not find a size to make atlas");
        goto err;
    }

    //fill step data
    atlas->texture_side_size = side;
    atlas->temp_texture = malloc(sizeof(uint8_t) * channels * side * side);
    if(!atlas->temp_texture) {
        rlr_log_error("could not allocate memory for temporary texture buffer for texture atlas");
        goto err;
    }

    //fill with magenta
    for(size_t i = 0; i < ((size_t)side * side); i++) {
        if(channels > 0) {
            atlas->temp_texture[i * channels + 0] = 255;
        }
        if(channels > 1) {
            atlas->temp_texture[i * channels + 1] = 0;
        }
        if(channels > 2) {
            atlas->temp_texture[i * channels + 2] = 255;
        }
        if(channels > 3) {
            atlas->temp_texture[i * channels + 3] = 255;
        }
    }

    //fill uv data
    for(size_t i = 0; i < description_count; i++) {
        stbrp_rect* rect = &rects[i];
        rlr_res_texture_atlas_tile_step_ctx_t* step = &atlas->steps[rect->id];
        uint32_t padding = step->external_width - step->internal_width;
        step->u = rect->x + padding;
        step->v = rect->y + padding;
    }

    free(rects);
    free(nodes);
    return atlas;
err:
    free(rects);
    free(nodes);
    rlr_res_texture_atlas_free(atlas);
    return NULL;
}

size_t rlr_res_texture_atlas_get_step_count(const rlr_res_texture_atlas_t* atlas) {
    return atlas->step_count;
}

void rlr_res_texture_atlas_step(rlr_res_texture_atlas_t* atlas) {
    if(atlas->is_loaded) {
        return;
    }
    
    //set atlas tile
    rlr_res_texture_atlas_tile_step_ctx_t* step = &atlas->steps[atlas->current_step++];
    rlr_res_texture_atlas_tile_t** tile = step->dest + step->dest_offset;

    arrpush(atlas->tiles, ((rlr_res_texture_atlas_tile_t){
        .texture = NULL,
        .size = rlr_vec2(step->internal_width, step->internal_height),
        .uv = rlr_vec2(step->u, step->v)
    }));
    *tile = &arrlast(atlas->tiles);

    insert_texture_tile(step->filepath, atlas->temp_texture, step->u, step->v, step->internal_width, step->internal_height, atlas->channels, atlas->texture_side_size, atlas->use_srgb_color_space);

    uint32_t padding = (step->external_width - step->internal_width) / 2;
    if(padding != 0) {
        extrude_texture_tile(atlas->temp_texture, step->u, step->v, step->internal_width, step->internal_height, padding, atlas->channels, atlas->texture_side_size);
    }

    //check if its done
    if(atlas->current_step == arrlen(atlas->steps)) {
        atlas_done_loading(atlas);
        return;
    }
}

bool rlr_res_texture_atlas_is_loaded(const rlr_res_texture_atlas_t* atlas) {
    return atlas->is_loaded;
}

void rlr_res_texture_atlas_free(rlr_res_texture_atlas_t* atlas) {
    if(!atlas) {
        return;
    }

    arrfree(atlas->steps);
    arrfree(atlas->tiles);
    rlr_res_texture_free(atlas->texture);
    free(atlas->temp_texture);
    free(atlas);
}