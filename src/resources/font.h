#pragma once
#include <stdint.h>
#include "texture.h"

/*
    the fonts used in rlr_render
    are the ones that can be 
    generated from Chlumsky's
    multi-channel signed distance
    field generator.

    https://github.com/Chlumsky/msdf-atlas-gen
*/

typedef struct rlr_t rlr_t;

typedef enum rlr_font_type_t {
    RLR_FONT_TYPE_MTSDF,
} rlr_font_type_t;

typedef struct rlr_font_glyph_t {
    uint32_t key; //unicode, the hashmap key
    float advance;
    float plane_left;
    float plane_right;
    float plane_bottom;
    float plane_top;
    float atlas_left;
    float atlas_right;
    float atlas_bottom;
    float atlas_top;
} rlr_font_glyph_t;

typedef struct rlr_font_t {
    rlr_font_type_t type;
    rlr_texture_t* texture;
    rlr_font_glyph_t* glyphs;
} rlr_font_t;

rlr_font_t* rlr_font_create(rlr_t* rlr, const char* csv_path, const char* texture_atlas_path, rlr_font_type_t type);
rlr_font_glyph_t* rlr_font_glyph_get(rlr_font_t* font, uint32_t unicode);
int32_t rlr_font_glyph_count(rlr_font_t* font);
void rlr_font_free(rlr_t* rlr, rlr_font_t* font);