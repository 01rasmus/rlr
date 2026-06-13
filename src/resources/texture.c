#include <stdint.h>
#include <stb_image.h>
#include "../error.h"
#include "texture.h"
#include "rlr.h"

rlr_texture_t* rlr_texture_create(const char* texture_path, bool generate_mipmaps) {
    rlr_texture_t* texture = malloc(sizeof(rlr_texture_t));
    if(!texture) {
        rlr_error_set(RLR_ERR_NO_MEMORY);
        goto err;
    }

    texture->texture = 0;
    texture->height = 0;
    texture->width = 0;

    int32_t w = 0;
    int32_t h = 0;
    uint8_t* data = stbi_load(texture_path, &w, &h, NULL, 4);
    if(!data) {
        rlr_error_setf(RLR_ERR_IMAGE_NOT_LOADED, "file \"%s\"", texture_path);
        goto err;
    }

    texture->width = w;
    texture->height = h;

    texture->texture = _rlr_raw()->backend->texture_create(data, w, h, generate_mipmaps);
    if(texture->texture == 0) {
        rlr_error_setf(RLR_ERR_BACKEND_NULL_HANDLE, "file \"%s\"", texture_path);
        goto err;
    }

    stbi_image_free(data);
    return texture;
err:
    stbi_image_free(data);
    rlr_texture_free(texture);
    return NULL;
}

void rlr_texture_bind(rlr_texture_t* texture, uint8_t texture_slot) {
    _rlr_raw()->backend->texture_bind(texture->texture, texture_slot);
}

void rlr_texture_free(rlr_texture_t* texture) {
    if(texture) {
        _rlr_raw()->backend->texture_free(texture->texture);
    }
    free(texture);
}