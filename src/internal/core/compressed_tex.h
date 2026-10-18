#pragma once
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define RLR_CMPRS_FLAG_SRGB    0x1
#define RLR_CMPRS_FLAG_ALPHA   0x2

typedef enum rlr_compressed_text_type_t {
    RLR_CMPRS_TYPE_BC1 = 0,
    RLR_CMPRS_TYPE_BC3,
    RLR_CMPRS_TYPE_BC4,
    RLR_CMPRS_TYPE_BC5,
    RLR_CMPRS_TYPE_BC7,
} rlr_compressed_text_type_t;

typedef struct rlr_compressed_tex_t {
    rlr_compressed_text_type_t type;      /* the compression encoding of the texture */
    uint64_t hash;              /* the hash of the original color data, type and flags */
    uint64_t flags;             /* flags of how the image is constructed */
    uint32_t mipmap_count;      /* the amount of mipmap levels */
    uint32_t width;             /* the original texture's width */
    uint32_t height;            /* the original texture's height */
    uint8_t** textures;         /* an array of the compressed color data. the length is equal to the mipmap count */
} rlr_compressed_tex_t;

rlr_compressed_tex_t* rlr_compressed_tex_create(uint8_t* color, uint32_t width, uint32_t height, uint64_t flags, rlr_compressed_text_type_t type);

int32_t rlr_compressed_tex_cache(rlr_compressed_tex_t* tex);
rlr_compressed_tex_t* rlr_compressed_tex_load(uint64_t hash);

uint64_t rlr_compressed_tex_hash(uint8_t* color, uint32_t width, uint32_t height, uint64_t flags, rlr_compressed_text_type_t type);
void rlr_compressed_tex_free(rlr_compressed_tex_t* tex);