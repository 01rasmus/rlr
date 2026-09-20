#include <string.h>
#include <utf8.h>
#include "../../external/stb_ds.h"
#include "../impl.h"
#include "word.h"

static _Thread_local word_measure_context_t ctx = {
    .glyphs = NULL,
    .row_count = 1,
};

bool rlr_word_unicode_is_space(int32_t u) {
    return u == '\n' || u == '\t' || u == ' ';
}

bool rlr_word_unicode_is_new_line(int32_t u) {
    return u == '\n';
}

static int32_t hex_value(char c) {
    if(c >= '0' && c <= '9') {
        return c - '0';
    }
    if(c >= 'a' && c <= 'f') {
        return c - 'a' + 10;
    }
    if(c >= 'A' && c <= 'F') {
        return c - 'A' + 10;
    }
    return -1;
}

static bool try_parse_color(const char* p, const char** end, uint32_t* color) {
    if(p[0] != '[' || p[1] != 'c' || p[2] != '=' || p[3] != '#') {
        return false;
    }

    union {
        struct {
            uint8_t r;
            uint8_t g;
            uint8_t b;
            uint8_t a;
        } comps;
        uint32_t uint;
    } col = {0};

    int32_t values[6];
    for(int32_t i = 0; i < 6; i++) {
        if(p[i + 4] == '\0') {
            return false;
        }
        int32_t h = hex_value(p[i + 4]);
        if(h < 0) {
            return false;
        }
        values[i] = h;
    }

    if(p[10] != ']') {
        return false;
    }

    col.comps.r = (uint8_t)((values[0] << 4) | values[1]);
    col.comps.g = (uint8_t)((values[2] << 4) | values[3]);
    col.comps.b = (uint8_t)((values[4] << 4) | values[5]);
    col.comps.a = 255;

    *color = col.uint;
    *end = p + 11;
    return true;
}

static bool try_parse_color_reset(const char* p, const char** end) {
    if(p[0] != '[' || p[1] != '/' || p[2] != 'c' || p[3] != ']') {
        return false;
    }
    *end = p + 4;
    return true;
}

static bool try_parse_italic_on(const char* p, const char** end) {
    if(p[0] != '[' || p[1] != 'i' || p[2] != ']') {
        return false;
    }
    *end = p + 3;
    return true;
}

static bool try_parse_italic_off(const char* p, const char** end) {
    if(p[0] != '[' || p[1] != '/' || p[2] != 'i' || p[3] != ']') {
        return false;
    }
    *end = p + 4;
    return true;
}

static bool try_parse_turn_off_rich_text(const char* p, const char** end) {
    if(p[0] != '[' || p[1] != 'o' || p[2] != 'f' || p[3] != 'f' || p[4] != ']') {
        return false;
    }
    *end = p + 5;
    return true;
}

word_measure_context_t* rlr_word_measure(const rlr_res_font_glyph_t* unknown_glyph, rlr_res_font_t* font, float font_size, float space_width, float height, float width, const char* text) {
    
    //reset
    arrsetlen(ctx.glyphs, 0);
    ctx.row_count = 1;

    float x = 0.0;
    float y = 0.0;
    uint32_t current_row = 0;
    float current_word_width = 0.0;
    uint32_t current_word_glyph_count = 0;
    uint32_t current_offset = 0;
    uint32_t current_word_id = 0;
    uint32_t current_color = UINT32_MAX;
    bool current_italic = false;
    bool do_rich_text = true;

    const void* p = text;
    utf8_int32_t unicode;
    while(p && *(const char*)p) {

        const char* start = p;
        if(*start == '[' && do_rich_text) {
            const char* end;
            uint32_t new_color = current_color;
            
            if(try_parse_color(start, &end, &new_color)) {
                current_color = new_color;
                p = end;
                continue;
            } else if(try_parse_italic_on(start, &end)) {
                current_italic = true;
                p = end;
                continue;
            } else if(try_parse_italic_off(start, &end)) {
                current_italic = false;
                p = end;
                continue;
            } else if(try_parse_color_reset(start, &end)) {
                current_color = UINT32_MAX;
                p = end;
                continue;
            } else if(try_parse_turn_off_rich_text(start, &end)) {
                do_rich_text = false;
                p = end;
                continue;
            }
        }

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

        const rlr_res_font_glyph_t* glyph = rlr_res_font_get_glyph(font, unicode);
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
            .color = current_color,
            .italic = current_italic,
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

bool rlr_word_generate(rlr_res_font_t* font, rlr_vec2_t* out_measured_size, float text_size, rlr_rect_t rectangle, rlr_horizontal_alignment_t horizontal_alignment, rlr_vertical_alignment_t vertical_alignment, rlr_anchor_t local_anchor, rlr_anchor_t screen_anchor, rlr_word_generate_callback_t gen_callback, const char* text, void* user) {
    if(!font) {
        goto err;
    }
    if(!font->texture) {
        goto err;
    }

    rlr_vec2_t local_anchor_vec = rlr_anchor_vec(local_anchor);
    const rlr_res_font_glyph_t* space_glyph = rlr_res_font_get_glyph(font, ' ');
    const rlr_res_font_glyph_t* unknown_glyph = rlr_res_font_get_glyph(font, '?');
    float x = rectangle.x - (rectangle.width * local_anchor_vec.x);
    float start_y = (rectangle.y + text_size) - (rectangle.height * local_anchor_vec.y) - (font->vertical_offset * text_size);
    float space_width = space_glyph ? ((space_glyph->advance * text_size)) : text_size;
    float start_x = x;

    float max_width = 0.0;
    float current_width = 0.0;

    word_measure_context_t* ctx = rlr_word_measure(unknown_glyph, font, text_size, space_width, rectangle.height, rectangle.width, text);
    start_y += rlr_vertical_start_position(vertical_alignment, rectangle.height - ctx->row_count * text_size);

    uint32_t current_word = 0;
    uint32_t current_row = 0;

    if(ctx->glyphs == NULL) {
        return true;
    }

    uint32_t row_word_count = 0;

    float rem_width = rlr_word_calculate_remaining_row_width(ctx, rectangle.width, 0, horizontal_alignment, text_size, space_width, &row_word_count);
    float sw = rlr_word_calculate_space_width(rem_width, space_width, row_word_count, horizontal_alignment);

    float start_pos = rlr_horizontal_start_position(horizontal_alignment, rem_width);
    x += start_pos;
    current_width += start_pos;

    for(uint32_t i = 0; i < arrlenu(ctx->glyphs); i++) {
        rlr_measured_glyph_t* g = &ctx->glyphs[i];
        float y = start_y + text_size * g->row;

        if(current_word != g->word_id) {
            x += sw;
            current_width += sw;
            current_word = g->word_id;
        }

        if(current_row != g->row) {
            rem_width = rlr_word_calculate_remaining_row_width(ctx, rectangle.width, i, horizontal_alignment, text_size, space_width, &row_word_count);
            sw = rlr_word_calculate_space_width(rem_width, space_width, row_word_count, horizontal_alignment);
            current_row = g->row;

            if(current_width > max_width) {
                max_width = current_width;
            }

            float start_pos = rlr_horizontal_start_position(horizontal_alignment, rem_width);
            x = start_x + start_pos;
            current_width = start_pos;
        }

        const rlr_res_font_glyph_t* glyph = g->glyph;
        float draw_x1 = x + glyph->plane_left * text_size;
        float draw_y1 = y - glyph->plane_top * text_size;
        float draw_x2 = x + glyph->plane_right * text_size;
        float draw_y2 = y - glyph->plane_bottom * text_size;

        float u1 = glyph->atlas_left;
        float v2 = glyph->atlas_bottom;
        float u2 = glyph->atlas_right;
        float v1 = glyph->atlas_top;

        //calculate the screen px range
        float texture_width = (u2 - u1) * font->texture->width;
        float quad_width = draw_x2 - draw_x1;
        float screen_px_range = quad_width / texture_width * (font->px_range);
        if(screen_px_range < 1.0) {
            screen_px_range = 1.0;
        }

        float sheer_amount = 0.25;
        float half_sheer_amount = sheer_amount * 0.5;
        float height = draw_y2 - draw_y1;
        float width = draw_x2 - draw_x1;
        float italic_sheer = g->italic == true ? (height * sheer_amount) : 0.0; 
        float italic_sheer_offset = g->italic == true ? (-text_size * half_sheer_amount) : 0.0;

        if(gen_callback) {
            gen_callback(
                user,
                g->color,
                italic_sheer,
                rlr_vec2(draw_x1 + italic_sheer_offset, draw_y1),
                rlr_vec2(width, height),
                screen_anchor,
                rlr_vec2(u1, v1),
                rlr_vec2(u2 - u1, v2 - v1),
                screen_px_range
            );
        }

        float word_width = glyph->advance * text_size;
        current_width += word_width;
        x += word_width;
    }

    if(current_width > max_width) {
        max_width = current_width;
    }
    
    if(out_measured_size) {
        const float epsilon_size = 1.0e-4;
        *out_measured_size = (rlr_vec2_t){
            .x = max_width + epsilon_size,
            .y = (current_row + 1) * text_size + epsilon_size
        };
    }
    return true;
err:
    return false;
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