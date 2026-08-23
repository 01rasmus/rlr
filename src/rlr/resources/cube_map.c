#include <stdlib.h>
#include <stb_image.h>
#include "../../internal/impl.h"
#include "../error.h"
#include "../rlr.h"
#include "cube_map.h"

#define FREE_TEXTURE_DATA(ARRAY) \
    for(int64_t i = 0; i < sizeof(ARRAY) / sizeof(ARRAY[0]); i++) \
        stbi_image_free(ARRAY[i]);

rlr_res_t rlr_res_cube_map_load(const char* right, const char* left, const char* top, const char* bottom, const char* front, const char* back) {
    rlr_res_t id = RLR_NULL;
    rlr_res_cube_map_t* cm = NULL;
    uint8_t* texture_data[6] = {0};
    const char* texture_locations[6] = {
        right,
        left,
        top,
        bottom,
        front,
        back
    };

    id = rlr_mem_man_allocate_res_cube_map(rlr_mem_man(), (rlr_res_cube_map_t){0});
    if(id == RLR_NULL) {
        rlr_log_error("could not allocate rlr handle for cube map");
        goto err;
    }
    
    cm = rlr_mem_man_get_res_cube_map(rlr_mem_man(), id);
    if(!cm) {
        rlr_log_error("pointer to the cube map handle is null");
        goto err;
    }

    int32_t width = 0;
    int32_t height = 0;

    for(int64_t i = 0; i < sizeof(texture_locations) / sizeof(texture_locations[0]); i++) {
        int32_t w;
        int32_t h;
        texture_data[i] = stbi_load(texture_locations[i], &w, &h, NULL, 3);
        if(!texture_data[i]) {
            rlr_log_error("the image \"%s\" did not load", texture_locations[i]);
            goto err;
        }

        if(i == 0) {
            width = w;
            height = h;
        } else if(width != w || height != h) {
            rlr_log_error("texture sizes are not the same");
            goto err;
        }
    }

    cm->texture = rlr_backend()->create_cube_map_texture(texture_data[0], texture_data[1], texture_data[2], texture_data[3], texture_data[4], texture_data[5], width, height, 3);
    if(!cm->texture) {
        rlr_log_error("backend texture handle is null");
        goto err;
    }

    FREE_TEXTURE_DATA(texture_data);
    rlr_log("loaded cube map\n\tright:\t%s\n\tleft:\t%s\n\ttop:\t%s\n\tbottom:\t%s\n\tfront:\t%s\n\tback:\t%s", right, left, top, bottom, front, back);
    return id;
err:
    FREE_TEXTURE_DATA(texture_data);
    rlr_res_cube_map_free(id);
    return RLR_NULL;
}

rlr_res_t rlr_res_cube_map_default() {
    rlr_res_t id = RLR_NULL;
    rlr_res_cube_map_t* cm = NULL;

    id = rlr_mem_man_allocate_res_cube_map(rlr_mem_man(), (rlr_res_cube_map_t){0});
    if(id == RLR_NULL) {
        rlr_log_error("could not allocate rlr handle for cube map");
        goto err;
    }
    
    cm = rlr_mem_man_get_res_cube_map(rlr_mem_man(), id);
    if(!cm) {
        rlr_log_error("pointer to the cube map handle is null");
        goto err;
    }

    const uint8_t white[3] = {255, 255, 255};
    cm->texture = rlr_backend()->create_cube_map_texture(white, white, white, white, white, white, 1, 1, 3);
    if(!cm->texture) {
        rlr_log_error("backend texture handle is null");
        goto err;
    }

    return id;
err:
    rlr_res_cube_map_free(id);
    return RLR_NULL;
}

void rlr_res_cube_map_bind(rlr_res_t cm, uint8_t texture_slot) {
    rlr_backend()->bind_texture(rlr_mem_man_get_res_cube_map(rlr_mem_man(), cm)->texture, RLR_BACKEND_TEXTURE_CUBE_MAP, texture_slot);
}

void rlr_res_cube_map_free(rlr_res_t id) {
    if(id == RLR_NULL) {
        return;
    }

    rlr_res_cube_map_t* cm = rlr_mem_man_get_res_cube_map(rlr_mem_man(), id);
    if(!cm) {
        return;
    }
    rlr_backend()->free_texture(cm->texture);
    rlr_mem_man_free_res_cube_map(rlr_mem_man(), id);
}