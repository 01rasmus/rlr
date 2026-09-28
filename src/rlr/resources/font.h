#pragma once
#include <stddef.h>
#include <stdint.h>
#include "../math/rect.h"
#include "../def.h"
#include "texture.h"

/*
    the fonts used in rlr_render
    is the *mtsdf* type that can be
    generated from Chlumsky's
    multi-channel signed distance
    field generator.

    https://github.com/Chlumsky/msdf-atlas-gen
*/

typedef struct rlr_res_font_t rlr_res_font_t;
typedef struct rlr_res_font_glyph_t rlr_res_font_glyph_t;

rlr_res_font_t* rlr_res_font_load(const char* csv_path, const char* texture_atlas_path, float px_range, rlr_res_font_t** fallback_fonts, size_t fallback_font_count);
void rlr_res_font_set_vertical_offset(rlr_res_font_t* font, float vertical_offset_fraction);
void rlr_res_font_set_size_scale(rlr_res_font_t* font, float scale);
rlr_vec2_t rlr_res_font_measure(rlr_res_font_t* font, rlr_rect_t rectangle, float text_size, const char* format, ...);

/*
    returns the glyph.

    if the font does not have the unicode character,
    it will go through all of the fallback fonts and
    return one that has it.

    if no font has it, NULL is returned.

    out_font_index will be set to the index of the font
    that had the glyph.

    non-zero means that it was a fallback font that had it,
    a zero value means that the main font had it (it will set it to 0
    if the function returns NULL too, since then the unknown glyph
    is usually in the main font)
*/
const rlr_res_font_glyph_t* rlr_res_font_get_glyph(rlr_res_font_t* font, uint32_t unicode, uint32_t* out_font_index);
int32_t rlr_res_font_get_glyph_count(const rlr_res_font_t* font);
void rlr_res_font_free(rlr_res_font_t* font);