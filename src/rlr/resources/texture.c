#include <inttypes.h>
#include <stdint.h>
#include <string.h>
#include <stb_image.h>
#include <cgltf.h>
#include "../../internal/core/res_types.h"
#include "../../internal/impl.h"
#include "../rlr.h"
#include "texture.h"

rlr_tex_t* rlr_tex_load(const char* file, rlr_tex_filter_t filter, uint64_t flags) {
    uint32_t width;
    uint32_t height;
    uint32_t channels;
    uint8_t* data = stbi_load(file, &width, &height, &channels, 0);
    rlr_tex_t* tex = NULL;
    if(data) {
        tex = rlr_tex_load_mem_raw_preset_filter_impl(data, width, height, channels, filter, flags);
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
        tex = rlr_tex_load_mem_raw_preset_filter_impl(data, width, height, channels, filter, flags);
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
    return rlr_tex_load_mem_raw_preset_filter_impl(color, width, height, channels, filter, flags);
err:
    if(f) {
        fclose(f);
    }
    return NULL;
}

rlr_tex_t* rlr_tex_load_mem_raw(const void* color, uint32_t width, uint32_t height, uint32_t channels, rlr_tex_filter_t filter, uint64_t flags) {
    return rlr_tex_load_mem_raw_preset_filter_impl(color, width, height, channels, filter, flags);
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