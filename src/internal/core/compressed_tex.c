#include <inttypes.h>
#include <stdio.h>
#include "../../external/stb_ds.h"
#include "compressed_tex.h"

static uint32_t rlr_compressed_tex_block_size(rlr_compressed_text_type_t compression_type) {
    switch(compression_type) {
        case RLR_CMPRS_TYPE_BC1:
        case RLR_CMPRS_TYPE_BC4:
            return 8;
        case RLR_CMPRS_TYPE_BC3:
        case RLR_CMPRS_TYPE_BC5:
        case RLR_CMPRS_TYPE_BC7:
        default:
            return 16;
            break;
    }
}

static void rlr_compressed_tex_mipmap_size(uint32_t original_width, uint32_t original_height, uint32_t level, uint32_t* out_width, uint32_t* out_height) {
    if(level == 0) {
        *out_width = original_width;
        *out_height = original_height;
        return;
    }

    uint32_t divisor = (2 << level) / 2;
    uint32_t w = original_width / divisor;
    uint32_t h = original_height / divisor;
    if(w < 1) {
        w = 1;
    }
    if(h < 1) {
        h = 1;
    }

    *out_width = w;
    *out_height = h;
}

static uint32_t rlr_compressed_tex_calculate_mipmap_count(uint32_t original_width, uint32_t original_height) {

    //a texture where one or both of the sides are 0 is invalid
    if(original_height == 0 || original_width == 0) {
        return 0;
    }

    //if the size is 1 no mip maps can be calculated
    if(original_height == 1 && original_width == 1) {
        return 1;
    }

    uint32_t level = 0;
    uint32_t w = original_width;
    uint32_t h = original_height;
    for(; w + h != 2; level++) {
        rlr_compressed_tex_mipmap_size(original_width, original_height, level, &w, &h);
    }

    return level;
}

static uint32_t rlr_compressed_tex_channel_count(uint64_t flags, rlr_compressed_text_type_t type) {
    bool alpha = flags & RLR_CMPRS_FLAG_ALPHA;
    switch(type) {
        case RLR_CMPRS_TYPE_BC1: {
            return 3;
        }
        case RLR_CMPRS_TYPE_BC3: {
            return 4;
        }
        case RLR_CMPRS_TYPE_BC4: {
            return 1;
        }
        case RLR_CMPRS_TYPE_BC5: {
            return 2;
        }
        case RLR_CMPRS_TYPE_BC7: {
            return alpha ? 4 : 3;
        }
        default:
            return 0;
    }
}

static void rlr_compressed_tex_file_name(uint64_t hash, char out[64]) {
    snprintf(out, 64, "%" PRIx64 ".dds", hash);
}

uint64_t rlr_compressed_tex_hash(uint8_t* color, uint32_t width, uint32_t height, uint64_t flags, rlr_compressed_text_type_t type) {
    uint32_t channel_count = rlr_compressed_tex_channel_count(flags, type);
    size_t hashed_flags = stbds_hash_bytes(&flags, sizeof(flags), 0);
    size_t hashed_type = stbds_hash_bytes(&type, sizeof(type), hashed_flags);
    size_t hashed_image = stbds_hash_bytes(color, width * height * channel_count, hashed_type);
    return (uint64_t)hashed_image;
}

int32_t rlr_compressed_tex_cache(rlr_compressed_tex_t* tex) {
    char filename[64];
    rlr_compressed_tex_file_name(tex->hash, filename);
    FILE* file = fopen(filename, "wb");

    char* data = NULL;
    size_t size = 0;
    FILE* test = open_memstream(&data, &size);

}

rlr_compressed_tex_t* rlr_compressed_tex_load(uint64_t hash) {
    char filename[64];
    rlr_compressed_tex_file_name(hash, filename);
    FILE* file = fopen(filename, "rb");

}

rlr_compressed_tex_t* rlr_compressed_tex_create(uint8_t* color, uint32_t width, uint32_t height, uint64_t flags, rlr_compressed_text_type_t type) {
    rlr_compressed_tex_t* tex = NULL;
    bool alpha = flags & RLR_CMPRS_FLAG_ALPHA;
    bool srgb = flags & RLR_CMPRS_FLAG_SRGB;

    tex = malloc(sizeof(rlr_compressed_tex_t));
    if(!tex) {
        goto err;
    }

    tex->type = type;
    tex->flags = flags;
    tex->hash = rlr_compressed_tex_hash(color, width, height, flags, type);
    tex->width = width;
    tex->height = height;
    tex->mipmap_count = rlr_compressed_tex_calculate_mipmap_count(width, height);
    tex->textures = malloc(sizeof(uint8_t*) * tex->mipmap_count);
    if(!tex->textures || tex->mipmap_count == 0) {
        goto err;
    }

    for(size_t i = 0; i< tex->mipmap_count; i++) {
        uint32_t w;
        uint32_t h;
        rlr_compressed_tex_mipmap_size(width, height, i, &w, &h);
    }

    return tex;
err:
    rlr_compressed_tex_free(tex);
    return NULL;
}

void rlr_compressed_tex_free(rlr_compressed_tex_t* tex) {
    if(!tex) {
        return;
    }

    if(tex->textures) {
        for(size_t i = 0; i < tex->mipmap_count; i++) {
            free(tex->textures[i]);
        }
        free(tex->textures);
    }
    free(tex);
}