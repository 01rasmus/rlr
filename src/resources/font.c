#include <stdlib.h>
#include <stb_ds.h>
#include "../io/csv.h"
#include "../io/str.h"
#include "../error.h"
#include "font.h"
#include "rlr.h"

void _rlr_font_csv_callback(uint32_t row, const char** columns, size_t count, void* user) {
    rlr_font_t* font = (rlr_font_t*)user;

    char* end = NULL;
    uint32_t unicode = strtoul(columns[0], &end, 10);
    if(*end != 0) {
        rlr_error_setf(RLR_ERR_PARSE_UNSIGNED_INT, "at row %u, column 1, \"%s\" was not a valid unsigned integer", row, columns[0]);
        return;
    }
    float advance = strtof(columns[1], &end);
    if(*end != 0) {
        rlr_error_setf(RLR_ERR_PARSE_FLOAT, "at row %u, column 2, \"%s\" was not a valid float", row, columns[1]);
        return;
    }
    float plane_bound_left = strtof(columns[2], &end);
    if(*end != 0) {
        rlr_error_setf(RLR_ERR_PARSE_FLOAT, "at row %u, column 3, \"%s\" was not a valid float", row, columns[2]);
        return;
    }
    float plane_bound_bottom = strtof(columns[3], &end);
    if(*end != 0) {
        rlr_error_setf(RLR_ERR_PARSE_FLOAT, "at row %u, column 4, \"%s\" was not a valid float", row, columns[3]);
        return;
    }
    float plane_bound_right = strtof(columns[4], &end);
    if(*end != 0) {
        rlr_error_setf(RLR_ERR_PARSE_FLOAT, "at row %u, column 5, \"%s\" was not a valid float", row, columns[4]);
        return;
    }
    float plane_bound_top = strtof(columns[5], &end);
    if(*end != 0) {
        rlr_error_setf(RLR_ERR_PARSE_FLOAT, "at row %u, column 6, \"%s\" was not a valid float", row, columns[5]);
        return;
    }
    float atlas_bound_left = strtof(columns[6], &end);
    if(*end != 0) {
        rlr_error_setf(RLR_ERR_PARSE_FLOAT, "at row %u, column 7, \"%s\" was not a valid float", row, columns[6]);
        return;
    }
    float atlas_bound_bottom = strtof(columns[7], &end);
    if(*end != 0) {
        rlr_error_setf(RLR_ERR_PARSE_FLOAT, "at row %u, column 8, \"%s\" was not a valid float", row, columns[7]);
        return;
    }
    float atlas_bound_right = strtof(columns[8], &end);
    if(*end != 0) {
        rlr_error_setf(RLR_ERR_PARSE_FLOAT, "at row %u, column 9, \"%s\" was not a valid float", row, columns[8]);
        return;
    }
    float atlas_bound_top = strtof(columns[9], &end);
    if(*end != 0) {
        printf("LASD %f %s\n", atlas_bound_top, columns[9]);
        rlr_error_setf(RLR_ERR_PARSE_FLOAT, "at row %u, column 10, \"%s\" was not a valid float", row, columns[9]);
        return;
    }

    rlr_font_glyph_t glyph = (rlr_font_glyph_t) {
        .key = unicode,
        .advance = advance,
        .plane_bottom = plane_bound_bottom,
        .plane_left = plane_bound_left,
        .plane_top = plane_bound_top,
        .plane_right = plane_bound_right,
        .atlas_left = atlas_bound_left / font->texture->width,
        .atlas_right = atlas_bound_right / font->texture->width,
        .atlas_top = (font->texture->height - atlas_bound_top) / font->texture->height,
        .atlas_bottom = (font->texture->height - atlas_bound_bottom) / font->texture->height,
    };

    hmputs(font->glyphs, glyph);
}

rlr_font_t* rlr_font_create(const char* csv_path, const char* texture_atlas_path, float px_range) {
    rlr_font_t* font = malloc(sizeof(rlr_font_t));
    if(!font) {
        rlr_error_set(RLR_ERR_NO_MEMORY);
        goto err;
    }

    char* csv = str_from_file(csv_path);
    if(!csv) {
        goto err;
    }

    font->px_range = px_range;
    font->texture = NULL;
    font->glyphs = NULL;

    //msdf textures need to use normal linear filtering (dont use mipmap filtering)
    font->texture = rlr_texture_load(texture_atlas_path, false, RLR_TEXTURE_FILTER_LINEAR);
    if(!font->texture) {
        font->texture = _rlr_raw()->texture_white;
    }

    if(!csv_parse(_rlr_font_csv_callback, csv, 10, ",", font)) {
        goto err;
    }

    free(csv);
    return font;
err:
    free(csv);
    rlr_font_free(font);
    return NULL;
}

rlr_font_glyph_t* rlr_font_glyph_get(rlr_font_t* font, uint32_t unicode) {
    return hmgetp_null(font->glyphs, unicode);
}

int32_t rlr_font_glyph_count(rlr_font_t* font) {
    return hmlen(font->glyphs);
}

void rlr_font_free(rlr_font_t* font) {
    if(font) {
        rlr_texture_free(font->texture);
        hmfree(font->glyphs);
    }
    free(font);
}