#include <stdint.h>
#include <stb_image.h>
#include <cgltf.h>
#include "../../internal/core/res_types.h"
#include "../../internal/impl.h"
#include "../error.h"
#include "../rlr.h"
#include "texture.h"

rlr_res_texture_t* rlr_res_texture_load(const char* texture_path, bool use_srgb_color_space, rlr_res_texture_filter_t filter) {
    uint8_t* data = NULL;
    rlr_res_texture_t* texture = NULL;

    texture = malloc(sizeof(rlr_res_texture_t));
    if(!texture) {
        rlr_log_error("pointer to the texture handle is null");
        goto err;
    }

    texture->texture = 0;
    texture->height = 0;
    texture->width = 0;

    int32_t w = 0;
    int32_t h = 0;
    int32_t channels = 0;
    data = stbi_load(texture_path, &w, &h, &channels, 0);
    if(!data) {
        rlr_log_error("failed to load file \"%s\"", texture_path);
        goto err;
    }

    texture->width = w;
    texture->height = h;
    texture->channels = channels;
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
        rlr_log_error("backend handle for the texture \"%s\" is null", texture_path);
        goto err;
    }

    stbi_image_free(data);
    rlr_log("loaded texture %s", texture_path);
    return texture;
err:
    stbi_image_free(data);
    rlr_res_texture_free(texture);
    return NULL;
}

rlr_res_texture_t* rlr_res_texture_create_empty(rlr_vec2_t size, bool use_srgb_color_space, rlr_res_texture_filter_t filter, uint32_t channels) {
    rlr_res_texture_t* texture = NULL;
    uint64_t texture_handle = 0;
    switch(filter) {
        case RLR_RES_TEXTURE_FILTER_NEAREST: {
            texture_handle = rlr_backend()->create_nearest_texture(NULL, size.x, size.y, channels, use_srgb_color_space);
            break;
        }
        case RLR_RES_TEXTURE_FILTER_LINEAR: {
            texture_handle = rlr_backend()->create_linear_texture(NULL, size.x, size.y, channels, use_srgb_color_space);
            break;
        }
        case RLR_RES_TEXTURE_FILTER_LINEAR_MIPMAP: {
            texture_handle = rlr_backend()->create_linear_mipmap_texture(NULL, size.x, size.y, channels, use_srgb_color_space);
            break;
        }
    }
    if(texture_handle == 0) {
        goto err;
    }
    texture = malloc(sizeof(rlr_res_texture_t));
    if(!texture) {
        goto err;
    }
    *texture = (rlr_res_texture_t){
        .height = size.x,
        .width = size.y,
        .channels = channels,
        .texture = texture_handle
    };
    return texture;
err:
    rlr_res_texture_free(texture);
    return NULL;
}

void rlr_res_texture_copy_subtex(rlr_res_texture_t* texture, const uint8_t* texture_data, uint32_t u, uint32_t v, uint32_t width, uint32_t height) {
    rlr_backend()->copy_sub_texture(texture->texture, u, v, width, height, texture->channels, texture_data);
}

rlr_res_texture_t* rlr_res_texture_default() {
    rlr_res_texture_t* texture = NULL;
    uint8_t data[3] = {255, 255, 255};
    uint64_t texture_handle = rlr_backend()->create_nearest_texture(data, 1, 1, 3, false);
    if(texture_handle == 0) {
        goto err;
    }
    texture = malloc(sizeof(rlr_res_texture_t));
    if(!texture) {
        goto err;
    }
    *texture = (rlr_res_texture_t){
        .height = 1,
        .width = 1,
        .channels = 3,
        .texture = texture_handle
    };
    return texture;
err:
    rlr_res_texture_free(texture);
    return NULL;
}

rlr_res_texture_t* rlr_res_texture_load_cgltf_base(cgltf_texture* tex) {
    if(!tex) {
        return NULL;
    }

    uint8_t* data = NULL;
    rlr_res_texture_t* texture = NULL;

    texture = malloc(sizeof(rlr_res_texture_t));
    if(!texture) {
        rlr_log_error("pointer to the texture handle is null");
        goto err;
    }

    if(!tex || !tex->image || !tex->image->buffer_view) {
        rlr_log_error("glb texture isn't loaded");
        goto err;
    }

    cgltf_buffer_view* view = tex->image->buffer_view;
    if(!view->buffer || !view->buffer->data) {
        rlr_log_error("glb texture isn't loaded");
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
    data = stbi_load_from_memory(texture_data, size, &w, &h, &channels, 0);
    if(!data) {
        rlr_log_error("failed to parse glb texture");
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
        rlr_log_error("the backend texture handle is null");
        goto err;
    }

    texture->width = w;
    texture->height = h;
    texture->channels = channels;

    stbi_image_free(data);
    return texture;
err:
    stbi_image_free(data);
    rlr_res_texture_free(texture);
    return NULL;
}

void rlr_res_texture_bind(const rlr_res_texture_t* texture, uint8_t texture_slot) {
    rlr_backend()->bind_texture(texture->texture, RLR_BACKEND_TEXTURE_2D, texture_slot);
}

void rlr_res_texture_free(rlr_res_texture_t* texture) {
    if(!texture) {
        return;
    }
    rlr_backend()->free_texture(texture->texture);
    free(texture);
}