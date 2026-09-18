#pragma once
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

rlr_res_font_t* rlr_res_font_load(const char* csv_path, const char* texture_atlas_path, float px_range);
rlr_vec2_t rlr_res_font_measure(rlr_res_font_t* font, rlr_rect_t rectangle, float text_size, const char* format, ...);
const rlr_res_font_glyph_t* rlr_res_font_get_glyph(rlr_res_font_t* font, uint32_t unicode);
int32_t rlr_res_font_get_glyph_count(const rlr_res_font_t* font);
void rlr_res_font_free(rlr_res_font_t* font);