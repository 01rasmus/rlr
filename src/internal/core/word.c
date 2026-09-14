#include <string.h>
#include <utf8.h>
#include "../../external/stb_ds.h"
#include "word.h"

static _Thread_local word_measure_context_t ctx = {
    .glyphs = NULL,
    .row_count = 0,
};

bool rlr_word_unicode_is_space(int32_t u) {
    return u == '\n' || u == '\t' || u == ' ';
}

bool rlr_word_unicode_is_new_line(int32_t u) {
    return u == '\n';
}

word_measure_context_t* rlr_word_measure(rlr_res_font_glyph_t* unknown_glyph, rlr_res_t font, float font_size, float space_width, float height, float width, const char* text) {
    
    //reset
    arrsetlen(ctx.glyphs, 0);
    ctx.row_count = 0;

    float x = 0.0;
    float y = 0.0;
    uint32_t current_row = 0;
    float current_word_width = 0.0;
    uint32_t current_word_glyph_count = 0;
    uint32_t current_offset = 0;
    uint32_t current_word_id = 0;

    const void* p = text;
    utf8_int32_t unicode;
    while(p && *(const char*)p) {
        p = utf8codepoint(p, &unicode);

        if(y + font_size > height) {
            return &ctx;
        }

        //this is the end of a word
        if(rlr_word_unicode_is_space(unicode)) {

            float word_width = (x <= WORD_EPSILON) ? current_word_width : (space_width + current_word_width);
            bool too_large = word_width + x > width;
            if(too_large) {
                x = 0.0;
                y += font_size;
                current_row++;
                ctx.row_count++;
            }

            for(uint32_t i = current_offset; i < (current_offset + current_word_glyph_count); i++) {
                ctx.glyphs[i].row = current_row;
            }
            current_offset += current_word_glyph_count;
            current_word_glyph_count = 0;
            current_word_width = 0.0;
            current_word_id++;
            x += word_width;

            //force new row on newline if the word fits
            if(rlr_word_unicode_is_new_line(unicode) && !too_large) {
                x = 0.0;
                y += font_size;
                current_row++;
                ctx.row_count++;
            }
            continue;
        }

        rlr_res_font_glyph_t* glyph = rlr_res_font_get_glyph(font, unicode);
        if(!glyph) {
            if(!unknown_glyph) {
                continue;
            }
            glyph = unknown_glyph;
        }

        float glyph_width = glyph->advance * font_size;
        current_word_width += glyph_width;
        current_word_glyph_count++;

        rlr_measured_glyph_t g = (rlr_measured_glyph_t){
            .color = UINT32_MAX,
            .word_id = current_word_id,
            .glyph = glyph,
        };
        arrpush(ctx.glyphs, g);
    }

    if(arrlenu(ctx.glyphs) != current_offset) {
        float word_width = (x <= WORD_EPSILON) ? current_word_width : (space_width + current_word_width);
        if(word_width + x > width) {
            current_row++;
            ctx.row_count++;
        }
        for(uint32_t i = current_offset; i < (current_offset + current_word_glyph_count); i++) {
            ctx.glyphs[i].row = current_row;
        }
    }
    return &ctx;
}

float rlr_word_calculate_remaining_row_width(word_measure_context_t* c, float width, uint32_t glyphs_offset, rlr_horizontal_alignment_t alignment, float font_size, float space_width, uint32_t* row_word_count) {
    uint32_t words = 1;
    float words_width = 0.0;
    uint32_t row = c->glyphs[glyphs_offset].row;
    uint32_t current_word = c->glyphs[glyphs_offset].word_id;

    for(uint32_t i = glyphs_offset; i < arrlenu(c->glyphs); i++) {
        rlr_measured_glyph_t* g = &c->glyphs[i];
        if(g->row != row) {
            break;
        }

        if(current_word != g->word_id) {
            words++;
            current_word = g->word_id;
        }

        words_width += g->glyph->advance * font_size;
    }

    if(alignment != RLR_HORIZONTAL_ALIGNMENT_JUSTIFIED) {
        words_width += space_width * (float)(words - 1);
    }

    *row_word_count = words;
    return width - words_width;
}

float rlr_word_calculate_space_width(float remaining_row_width, float real_space_width, uint32_t row_word_count, rlr_horizontal_alignment_t alignment) {
    switch(alignment) {
        case RLR_HORIZONTAL_ALIGNMENT_LEFT:
        case RLR_HORIZONTAL_ALIGNMENT_CENTER:
        case RLR_HORIZONTAL_ALIGNMENT_RIGHT:
            return real_space_width;
        case RLR_HORIZONTAL_ALIGNMENT_JUSTIFIED:
            return row_word_count > 1 ? remaining_row_width / (float)(row_word_count - 1) : remaining_row_width;
    }
}