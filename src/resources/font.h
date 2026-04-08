#pragma once
#include <stdint.h>
#include "texture.h"
#include "rlr.h"

/*
    the fonts used in rlr_render
    are the ones that can be 
    generated from Chlumsky's
    multi-channel signed distance
    field generator.

    https://github.com/Chlumsky/msdf-atlas-gen
*/

typedef enum rlr_font_type_t {
    RLR_FONT_TYPE_MTSDF,
} rlr_font_type_t;

typedef struct rlr_font_glyph_t {
    uint32_t key; //unicode, the hashmap key
    float advance;
    float left_bearing;
    float offset_y;
    float x;
    float y;
    float width;
    float height;
} rlr_font_glyph_t;

typedef struct rlr_font_t {
    rlr_font_type_t type;
    rlr_texture_t* texture;
    rlr_font_glyph_t* glyphs;
    float average_glyph_size;
} rlr_font_t;

rlr_font_t* rlr_font_load(rlr_t* rlr, const char* csv_path, const char* texture_atlas_path, rlr_font_type_t type);
bool rlr_font_generate_vertices(rlr_font_t* font, float x, float y, float size, const char* text);
int32_t rlr_font_glyph_count(rlr_font_t* font);
void rlr_font_free(rlr_t* rlr, rlr_font_t* font);