#include <stdint.h>
#include <stb_image.h>
#include "../error.h"
#include "texture.h"
#include "rlr.h"

rlr_texture_t* rlr_texture_load(const char* texture_path, bool use_srgb_color_space, rlr_texture_filter_t filter) {
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
    int32_t channels = 0;
    uint8_t* data = stbi_load(texture_path, &w, &h, &channels, 0);
    if(!data) {
        rlr_error_setf(RLR_ERR_IMAGE_NOT_LOADED, "file \"%s\"", texture_path);
        goto err;
    }

    texture->width = w;
    texture->height = h;
    switch(filter) {
        case RLR_TEXTURE_FILTER_NEAREST: {
            texture->texture = rlr_backend()->texture_create_nearest(data, w, h, channels, use_srgb_color_space);
            break;
        }
        case RLR_TEXTURE_FILTER_LINEAR: {
            texture->texture = rlr_backend()->texture_create_linear(data, w, h, channels, use_srgb_color_space);
            break;
        }
        default:
        case RLR_TEXTURE_FILTER_LINEAR_MIPMAP: {
            texture->texture = rlr_backend()->texture_create_linear_mipmap(data, w, h, channels, use_srgb_color_space);
            break;
        }
    }

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

rlr_texture_t* rlr_texture_default() {
    rlr_texture_t* texture = malloc(sizeof(rlr_texture_t));
    if(!texture) {
        rlr_error_set(RLR_ERR_NO_MEMORY);
        goto err;
    }

    texture->texture = 0;
    texture->height = 1;
    texture->width = 1;

    uint8_t data[1] = {255};
    texture->texture = rlr_backend()->texture_create_nearest(data, 1, 1, 1, false);
    if(texture->texture == 0) {
        rlr_error_set(RLR_ERR_BACKEND_NULL_HANDLE);
        goto err;
    }

    return texture;
err:
    rlr_texture_free(texture);
    return NULL;
}

rlr_texture_t* rlr_texture_load_cgltf_base(cgltf_texture* tex) {
    rlr_texture_t* texture = malloc(sizeof(rlr_texture_t));
    if(!texture) {
        rlr_error_set(RLR_ERR_NO_MEMORY);
        goto err;
    }

    if(!tex || !tex->image || !tex->image->buffer_view) {
        rlr_error_set(RLR_ERR_IMAGE_NOT_LOADED);
        goto err;
    }

    cgltf_buffer_view* view = tex->image->buffer_view;
    if(!view->buffer || !view->buffer->data) {
        rlr_error_set(RLR_ERR_IMAGE_NOT_LOADED);
        goto err;
    }

    uint8_t* texture_data = (uint8_t*)view->buffer->data + view->offset;
    size_t size = view->size;

    texture->texture = 0;
    texture->height = 0;
    texture->width = 0;

    int32_t w = 0;
    int32_t h = 0;
    int32_t channels = 0;
    uint8_t* data = stbi_load_from_memory(texture_data, size, &w, &h, &channels, 0);
    if(!data) {
        rlr_error_set(RLR_ERR_IMAGE_NOT_LOADED);
        goto err;
    }

    cgltf_sampler sampler;
    if(tex->sampler) {
        sampler = (*tex->sampler);
    } else {
        sampler = (cgltf_sampler) {
            .mag_filter = cgltf_filter_type_linear,
            .min_filter = cgltf_filter_type_linear,
            .wrap_s = cgltf_wrap_mode_clamp_to_edge,
            .wrap_t = cgltf_wrap_mode_clamp_to_edge
        };
    }

    texture->texture = rlr_backend()->texture_create(data, w, h, channels, true, sampler.min_filter, sampler.mag_filter, sampler.wrap_s, sampler.wrap_t);
    if(texture->texture == 0) {
        rlr_error_set(RLR_ERR_BACKEND_NULL_HANDLE);
        goto err;
    }

    texture->width = w;
    texture->height = h;

    stbi_image_free(data);
    return texture;
err:
    stbi_image_free(data);
    rlr_texture_free(texture);
    return NULL;
}

void rlr_texture_bind(rlr_texture_t* texture, uint8_t texture_slot) {
    _rlr_raw()->backend->texture_bind(texture->texture, RLR_BACKEND_TEXTURE_2D, texture_slot);
}

void rlr_texture_free(rlr_texture_t* texture) {
    if(texture) {
        _rlr_raw()->backend->texture_free(texture->texture);
    }
    free(texture);
}