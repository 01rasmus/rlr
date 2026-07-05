#include <stdint.h>
#include <stb_image.h>
#include "internal/impl.h"
#include "rlr/error.h"
#include "rlr/rlr.h"
#include "texture.h"

rlr_res_texture_t* rlr_res_texture_load(const char* texture_path, bool use_srgb_color_space, rlr_res_texture_filter_t filter) {
    rlr_res_texture_t* texture = malloc(sizeof(rlr_res_texture_t));
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
        case RLR_RES_TEXTURE_FILTER_NEAREST: {
            texture->texture = rlr_backend()->create_nearest_texture(data, w, h, channels, use_srgb_color_space);
            break;
        }
        case RLR_RES_TEXTURE_FILTER_LINEAR: {
            texture->texture = rlr_backend()->create_linear_texture(data, w, h, channels, use_srgb_color_space);
            break;
        }
        default:
        case RLR_RES_TEXTURE_FILTER_LINEAR_MIPMAP: {
            texture->texture = rlr_backend()->create_linear_mipmap_texture(data, w, h, channels, use_srgb_color_space);
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
    rlr_res_texture_free(texture);
    return NULL;
}

rlr_res_texture_t* rlr_res_texture_default() {
    rlr_res_texture_t* texture = malloc(sizeof(rlr_res_texture_t));
    if(!texture) {
        rlr_error_set(RLR_ERR_NO_MEMORY);
        goto err;
    }

    texture->texture = 0;
    texture->height = 1;
    texture->width = 1;

    uint8_t data[3] = {255, 255, 255};
    texture->texture = rlr_backend()->create_nearest_texture(data, 1, 1, 3, false);
    if(texture->texture == 0) {
        rlr_error_set(RLR_ERR_BACKEND_NULL_HANDLE);
        goto err;
    }

    return texture;
err:
    rlr_res_texture_free(texture);
    return NULL;
}

rlr_res_texture_t* rlr_res_texture_load_cgltf_base(cgltf_texture* tex) {
    if(!tex) {
        return NULL;
    }

    rlr_res_texture_t* texture = malloc(sizeof(rlr_res_texture_t));
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

    texture->texture = rlr_backend()->create_texture(data, w, h, channels, true, sampler.min_filter, sampler.mag_filter, sampler.wrap_s, sampler.wrap_t);
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
    rlr_res_texture_free(texture);
    return NULL;
}

void rlr_res_texture_bind(rlr_res_texture_t* texture, uint8_t texture_slot) {
    rlr_backend()->bind_texture(texture->texture, RLR_BACKEND_TEXTURE_2D, texture_slot);
}

void rlr_res_texture_free(rlr_res_texture_t* texture) {
    if(texture) {
        rlr_backend()->free_texture(texture->texture);
    }
    free(texture);
}