#include <inttypes.h>
#include <stdint.h>
#include <string.h>
#include <stb_image.h>
#include <cgltf.h>
#include "../../internal/core/res_types.h"
#include "../../internal/impl.h"
#include "../rlr.h"
#include "texture.h"

static rlr_tex_t* _rlr_tex_load_mem_raw_impl(const void* color, uint32_t width, uint32_t height, uint32_t channels, uint32_t min_filter, uint32_t mag_filter, uint32_t wrap_s, uint32_t wrap_t, uint64_t flags) {
    rlr_tex_t* tex = NULL;

    if(width % 4 != 0 || height % 4 != 0) {
        rlr_log_error("texture's size(%dx%d) has a side that is not divisible by 4", width, height);
        goto err;
    }

    if(channels == 0 || channels > 4) {
        rlr_log_error("invalid channel count %" PRIu32, channels);
        goto err;
    }

    tex = malloc(sizeof(rlr_tex_t));
    if(!tex) {
        rlr_log_error("pointer to the texture handle for texture is null");
        goto err;
    }
    tex->texture = 0;
    tex->width = width;
    tex->height = height;

    bool compress = (flags & RLR_TEX_FLAG_COMPRESS) != 0;
    bool srgb = (flags & RLR_TEX_FLAG_SRGB) != 0;

    if(compress) {
        tex->texture = rlr_backend()->create_compressed_texture(color, width, height, channels, srgb, min_filter, mag_filter, wrap_s, wrap_t);
    } else {
        tex->texture = rlr_backend()->create_texture(color, width, height, channels, srgb, min_filter, mag_filter, wrap_s, wrap_t);
    }

    if(tex->texture == 0) {
        rlr_log_error("backend handle for texture \"%s\" is null");
        goto err;
    }
    
    return tex;
err:
    rlr_tex_free(tex);
    return NULL;
}

static rlr_tex_t* _rlr_tex_load_mem_raw_preset_filter_impl(const void* color, uint32_t width, uint32_t height, uint32_t channels, rlr_tex_filter_t filter, uint64_t flags) {
    uint32_t min_filter;
    uint32_t mag_filter;
    uint32_t wrap_s;
    uint32_t wrap_t;

    switch(filter) {
        case RLR_TEX_FILTER_LINEAR: {
            min_filter = RLR_BACKEND_FILTER_LINEAR;
            mag_filter = RLR_BACKEND_FILTER_LINEAR;
            wrap_s = RLR_BACKEND_FILTER_CLAMP_TO_EDGE;
            wrap_t = RLR_BACKEND_FILTER_CLAMP_TO_EDGE;
            break;
        }
        case RLR_TEX_FILTER_LINEAR_MIPMAP: {
            min_filter = RLR_BACKEND_FILTER_LINEAR_MIPMAP;
            mag_filter = RLR_BACKEND_FILTER_LINEAR;
            wrap_s = RLR_BACKEND_FILTER_CLAMP_TO_EDGE;
            wrap_t = RLR_BACKEND_FILTER_CLAMP_TO_EDGE;
            break;
        }
        case RLR_TEX_FILTER_NEAREST: {
            min_filter = RLR_BACKEND_FILTER_NEAREST;
            mag_filter = RLR_BACKEND_FILTER_NEAREST;
            wrap_s = RLR_BACKEND_FILTER_CLAMP_TO_EDGE;
            wrap_t = RLR_BACKEND_FILTER_CLAMP_TO_EDGE;
            break;
        }
        default:
            rlr_log_error("invalid filter id 0x%" PRIx32, filter);
            return NULL;
    }

    return _rlr_tex_load_mem_raw_impl(color, width, height, channels, min_filter, mag_filter, wrap_s, wrap_t, flags);
}

rlr_tex_t* rlr_tex_load(const char* file, rlr_tex_filter_t filter, uint64_t flags) {
    uint32_t width;
    uint32_t height;
    uint32_t channels;
    uint8_t* data = stbi_load(file, &width, &height, &channels, 0);
    rlr_tex_t* tex = NULL;
    if(data) {
        tex = _rlr_tex_load_mem_raw_preset_filter_impl(data, width, height, channels, filter, flags);
    }
    stbi_image_free(data);
    return tex;
}

rlr_tex_t* rlr_tex_load_mem(const void* color, size_t color_len, rlr_tex_filter_t filter, uint64_t flags) {
    uint32_t width;
    uint32_t height;
    uint32_t channels;
    uint8_t* data = stbi_load_from_memory(color, color_len, &width, &height, &channels, 0);
    rlr_tex_t* tex = NULL;
    if(data) {
        tex = _rlr_tex_load_mem_raw_preset_filter_impl(data, width, height, channels, filter, flags);
    }
    stbi_image_free(data);
    return tex;
}

rlr_tex_t* rlr_tex_load_raw(const char* file, uint32_t width, uint32_t height, uint32_t channels, rlr_tex_filter_t filter, uint64_t flags) {
    FILE* f = fopen(file, "rb");
    void* color = NULL;
    if(!f) {
        rlr_log_error("could not open the file \"%s\"", file);
        goto err;
    }

    size_t size = width * height * channels;
    color = malloc(size);
    if(!color) {
        goto err;
    }

    size_t read = fread(color, 1, size, f);
    if(read != size) {
        rlr_log_error("tried to read %zu bytes but only read %zu", size, read);
        goto err;
    }

    if(f) {
        fclose(f);
    }
    return _rlr_tex_load_mem_raw_preset_filter_impl(color, width, height, channels, filter, flags);
err:
    if(f) {
        fclose(f);
    }
    return NULL;
}

rlr_tex_t* rlr_tex_load_mem_raw(const void* color, uint32_t width, uint32_t height, uint32_t channels, rlr_tex_filter_t filter, uint64_t flags) {
    return _rlr_tex_load_mem_raw_preset_filter_impl(color, width, height, channels, filter, flags);
}

rlr_tex_t* rlr_tex_load_cgltf_albedo(cgltf_texture* tex) {
    if(!tex) {
        return NULL;
    }

    if(!tex || !tex->image || !tex->image->buffer_view) {
        rlr_log_error("glb texture isn't loaded");
        return NULL;
    }

    cgltf_buffer_view* view = tex->image->buffer_view;
    if(!view->buffer || !view->buffer->data) {
        rlr_log_error("glb texture isn't loaded");
        return NULL;
    }

    cgltf_sampler sampler;
    if(tex->sampler) {
        sampler = (*tex->sampler);
    } else {
        sampler = (cgltf_sampler) {
            .min_filter = cgltf_filter_type_linear_mipmap_nearest,
            .mag_filter = cgltf_filter_type_linear,
            .wrap_s = cgltf_wrap_mode_clamp_to_edge,
            .wrap_t = cgltf_wrap_mode_clamp_to_edge
        };
    }

    uint32_t width;
    uint32_t height;
    uint32_t channels;
    uint8_t* data = stbi_load_from_memory((uint8_t*)view->buffer->data + view->offset, view->size, &width, &height, &channels, 0);
    if(!data) {
        return NULL;
    }

    stbi_image_free(data);
    return _rlr_tex_load_mem_raw_impl(data, width, height, channels, sampler.min_filter, sampler.mag_filter, sampler.wrap_s, sampler.wrap_t, RLR_TEX_FLAG_COMPRESS | RLR_TEX_FLAG_SRGB);
}

void rlr_tex_bind(const rlr_tex_t* texture, uint8_t texture_slot) {
    rlr_backend()->bind_texture(texture->texture, RLR_BACKEND_TEXTURE_2D, texture_slot);
}

void rlr_tex_free(rlr_tex_t* texture) {
    if(!texture) {
        return;
    }
    rlr_backend()->free_texture(texture->texture);
    free(texture);
}