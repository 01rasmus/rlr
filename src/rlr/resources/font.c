#include <stdlib.h>
#include "../../internal/core/res_types.h"
#include "../../internal/core/word.h"
#include "../../external/stb_ds.h"
#include "../../internal/impl.h"
#include "../io/csv.h"
#include "../io/str.h"
#include "../error.h"
#include "../rlr.h"
#include "font.h"

typedef struct rlr_res_font_load_context_t {
    rlr_res_font_t* font;
    rlr_res_texture_t* tex;
} rlr_res_font_load_context_t;

void _rlr_font_csv_callback(uint32_t row, const char** columns, size_t count, void* user) {
    rlr_res_font_load_context_t* ctx = (rlr_res_font_load_context_t*)user;

    char* end = NULL;
    uint32_t unicode = strtoul(columns[0], &end, 10);
    if(*end != 0) {
        rlr_log_error("failed to parse uint at row %u, column 1, \"%s\" was not a valid unsigned integer", row, columns[0]);
        return;
    }
    float advance = strtof(columns[1], &end);
    if(*end != 0) {
        rlr_log_error("failed to parse float at row %u, column 2, \"%s\" was not a valid float", row, columns[1]);
        return;
    }
    float plane_bound_left = strtof(columns[2], &end);
    if(*end != 0) {
        rlr_log_error("failed to parse float at row %u, column 3, \"%s\" was not a valid float", row, columns[2]);
        return;
    }
    float plane_bound_bottom = strtof(columns[3], &end);
    if(*end != 0) {
        rlr_log_error("failed to parse float at row %u, column 4, \"%s\" was not a valid float", row, columns[3]);
        return;
    }
    float plane_bound_right = strtof(columns[4], &end);
    if(*end != 0) {
        rlr_log_error("failed to parse float at row %u, column 5, \"%s\" was not a valid float", row, columns[4]);
        return;
    }
    float plane_bound_top = strtof(columns[5], &end);
    if(*end != 0) {
        rlr_log_error("failed to parse float at row %u, column 6, \"%s\" was not a valid float", row, columns[5]);
        return;
    }
    float atlas_bound_left = strtof(columns[6], &end);
    if(*end != 0) {
        rlr_log_error("failed to parse float at row %u, column 7, \"%s\" was not a valid float", row, columns[6]);
        return;
    }
    float atlas_bound_bottom = strtof(columns[7], &end);
    if(*end != 0) {
        rlr_log_error("failed to parse float at row %u, column 8, \"%s\" was not a valid float", row, columns[7]);
        return;
    }
    float atlas_bound_right = strtof(columns[8], &end);
    if(*end != 0) {
        rlr_log_error("failed to parse float at row %u, column 9, \"%s\" was not a valid float", row, columns[8]);
        return;
    }
    float atlas_bound_top = strtof(columns[9], &end);
    if(*end != 0) {
        rlr_log_error("failed to parse float at row %u, column 10, \"%s\" was not a valid float", row, columns[9]);
        return;
    }

    const rlr_res_font_glyph_t glyph = (rlr_res_font_glyph_t) {
        .key = unicode,
        .advance = advance,
        .plane_bottom = plane_bound_bottom,
        .plane_left = plane_bound_left,
        .plane_top = plane_bound_top,
        .plane_right = plane_bound_right,
        .atlas_left = atlas_bound_left / ctx->tex->width,
        .atlas_right = atlas_bound_right / ctx->tex->width,
        .atlas_top = (ctx->tex->height - atlas_bound_top) / ctx->tex->height,
        .atlas_bottom = (ctx->tex->height - atlas_bound_bottom) / ctx->tex->height,
    };
    hmputs(ctx->font->glyphs, glyph);
}

rlr_res_font_t* rlr_res_font_load(const char* csv_path, const char* texture_atlas_path, float px_range) {
    rlr_res_font_t* font = NULL;

    font = malloc(sizeof(rlr_res_font_t));
    if(!font) {
        rlr_log_error("pointer to the font handle is null");
        goto err;
    }

    char* csv = rlr_io_str_load_from_file(csv_path);
    if(!csv) {
        goto err;
    }

    font->px_range = px_range;
    font->texture = RLR_NULL;
    font->glyphs = NULL;

    //msdf textures need to use normal linear filtering (dont use mipmap filtering)
    font->texture = rlr_res_texture_load(texture_atlas_path, false, RLR_RES_TEXTURE_FILTER_LINEAR);
    if(!font->texture) {
        font->texture = rlr_default_texture();
    }

    rlr_res_font_load_context_t ctx = {
        .font = font,
        .tex = font->texture,
    };
    if(!rlr_io_csv_parse(_rlr_font_csv_callback, csv, 10, ",", &ctx)) {
        goto err;
    }

    free(csv);
    rlr_log("loaded mtsdf font\n\tmeta:\t%s\n\tatlas:\t%s", csv_path, texture_atlas_path);
    return font;
err:
    free(csv);
    rlr_res_font_free(font);
    return NULL;
}

rlr_vec2_t rlr_res_font_measure(const rlr_res_font_t* font, rlr_rect_t rectangle, float text_size, const char* format, ...) {
    rlr_vec2_t size = {0};
    if(!rlr_word_generate(font, &size, text_size, rectangle, RLR_HORIZONTAL_ALIGNMENT_LEFT, RLR_VERTICAL_ALIGNMENT_TOP, RLR_ANCHOR_TOP_LEFT, RLR_ANCHOR_TOP_LEFT, NULL, format, NULL)) {
        return (rlr_vec2_t){0};
    }
    return size;
}

const rlr_res_font_glyph_t* rlr_res_font_get_glyph(rlr_res_font_t* font, uint32_t unicode) {
    return hmgetp_null(font->glyphs, unicode);
}

int32_t rlr_res_font_get_glyph_count(const rlr_res_font_t* font) {
    return hmlen(font->glyphs);
}

void rlr_res_font_free(rlr_res_font_t* font) {
    if(!font) {
        return;
    }
    rlr_res_texture_free(font->texture);
    hmfree(font->glyphs);
    free(font);
}