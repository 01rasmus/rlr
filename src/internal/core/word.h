#pragma once
#include <stdbool.h>
#include <stdint.h>
#include "../../rlr/resources/font.h"
#include "../../rlr/math/alignment.h"
#include "../../rlr/math/rect.h"
#include "../../rlr/math/anchor.h"
#include "../../rlr/math/alignment.h"
#include "../../rlr/math/vec.h"

#define WORD_EPSILON    (1e-6)

typedef void (*rlr_word_generate_callback_t)(void* user, uint32_t color, rlr_vec2_t pos, rlr_vec2_t size, rlr_anchor_t screen_anchor, rlr_vec2_t uv, rlr_vec2_t uv_size);

typedef struct rlr_measured_glyph_t {
    uint32_t row;
    uint32_t color;
    uint32_t word_id;
    rlr_res_font_glyph_t* glyph;
} rlr_measured_glyph_t;

typedef struct word_measure_context_t {
    rlr_measured_glyph_t* glyphs;
    uint32_t row_count;
} word_measure_context_t;

bool rlr_word_unicode_is_space(int32_t u);
bool rlr_word_unicode_is_new_line(int32_t u);

word_measure_context_t* rlr_word_measure(rlr_res_font_glyph_t* unknown_glyph, rlr_res_t font, float font_size, float space_width, float height, float width, const char* text);
bool rlr_word_generate(rlr_res_t font, rlr_vec2_t* out_measured_size, float text_size, rlr_rect_t rectangle, rlr_horizontal_alignment_t horizontal_alignment, rlr_vertical_alignment_t vertical_alignment, rlr_anchor_t local_anchor, rlr_anchor_t screen_anchor, rlr_word_generate_callback_t gen_callback, const char* text, void* user);
float rlr_word_calculate_remaining_row_width(word_measure_context_t* ctx, float width, uint32_t glyphs_offset, rlr_horizontal_alignment_t alignment, float font_size, float space_width, uint32_t* row_word_count);
float rlr_word_calculate_space_width(float remaining_row_width, float real_space_width, uint32_t row_word_count, rlr_horizontal_alignment_t alignment);