#include <stdlib.h>
#include <stb_image.h>
#include "internal/rlr.h"
#include "rlr/error.h"
#include "rlr/rlr.h"
#include "cube_map.h"

#define FREE_TEXTURE_DATA(ARRAY) \
    for(int64_t i = 0; i < sizeof(ARRAY) / sizeof(ARRAY[0]); i++) \
        stbi_image_free(ARRAY[i]);

rlr_res_cube_map_t* rlr_res_cube_map_load(const char* right, const char* left, const char* top, const char* bottom, const char* front, const char* back) {
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

    cm = malloc(sizeof(rlr_res_cube_map_t));
    if(!cm) {
        rlr_error_set(RLR_ERR_NO_MEMORY);
        goto err;
    }

    int32_t width = 0;
    int32_t height = 0;

    for(int64_t i = 0; i < sizeof(texture_locations) / sizeof(texture_locations[0]); i++) {
        int32_t w;
        int32_t h;
        texture_data[i] = stbi_load(texture_locations[i], &w, &h, NULL, 3);
        if(!texture_data[i]) {
            rlr_error_setf(RLR_ERR_IMAGE_NOT_LOADED, "file \"%s\"", texture_locations[i]);
            goto err;
        }

        if(i == 0) {
            width = w;
            height = h;
        } else if(width != w || height != h) {
            rlr_error_set(RLR_ERR_BACKEND_TEXTURE_SIZES_ARE_DIFFERENT);
            goto err;
        }
    }

    cm->texture = rlr_backend()->create_cube_map_texture(texture_data[0], texture_data[1], texture_data[2], texture_data[3], texture_data[4], texture_data[5], width, height, 3);
    if(!cm->texture) {
        rlr_error_set(RLR_ERR_BACKEND_NULL_HANDLE);
        goto err;
    }

    FREE_TEXTURE_DATA(texture_data);
    return cm;
err:
    FREE_TEXTURE_DATA(texture_data);
    rlr_res_cube_map_free(cm);
    return NULL;
}

void rlr_res_cube_map_bind(rlr_res_cube_map_t* cm, uint8_t texture_slot) {
    rlr_backend()->bind_texture(cm->texture, RLR_BACKEND_TEXTURE_CUBE_MAP, texture_slot);
}

void rlr_res_cube_map_free(rlr_res_cube_map_t* cm) {
    if(!cm) {
        return;
    }
    rlr_backend()->free_texture(cm->texture);
    free(cm);
}