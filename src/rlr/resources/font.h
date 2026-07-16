#pragma once
#include <stdint.h>
#include "rlr/def.h"
#include "texture.h"

/*
    the fonts used in rlr_render
    is the *mtsdf* type that can be
    generated from Chlumsky's
    multi-channel signed distance
    field generator.

    https://github.com/Chlumsky/msdf-atlas-gen
*/

typedef struct rlr_res_font_glyph_t {
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
} rlr_res_font_glyph_t;

typedef struct rlr_res_font_t {
    rlr_res_t texture;
    rlr_res_font_glyph_t* glyphs;
    float px_range;
} rlr_res_font_t;

rlr_res_t rlr_res_font_load(const char* csv_path, const char* texture_atlas_path, float px_range);
rlr_res_font_glyph_t* rlr_res_font_get_glyph(rlr_res_t font, uint32_t unicode);
int32_t rlr_res_font_get_glyph_count(rlr_res_t font);
void rlr_res_font_free(rlr_res_t font);