#include <stdlib.h>
#include <stb_ds.h>
#include "../io/csv.h"
#include "../io/str.h"
#include "../error.h"
#include "font.h"

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
        rlr_error_setf(RLR_ERR_PARSE_FLOAT, "at row %u, column 10, \"%s\" was not a valid float", row, columns[9]);
        return;
    }

    rlr_font_glyph_t glyph = (rlr_font_glyph_t) {
        .key = unicode,
        .advance = advance,
        .left_bearing = plane_bound_left,
        .offset_y = plane_bound_top,
        .x = atlas_bound_left,
        .y = font->texture->height - atlas_bound_top,
        .width = atlas_bound_right - atlas_bound_left,
        .height = atlas_bound_top - atlas_bound_bottom,
    };

    font->average_glyph_size += glyph.width + glyph.height;
    hmputs(font->glyphs, glyph);
}

rlr_font_t* rlr_font_create(rlr_t* rlr, const char* csv_path, const char* texture_atlas_path, rlr_font_type_t type) {
    rlr_font_t* font = malloc(sizeof(rlr_font_t));
    if(!font) {
        rlr_error_set(RLR_ERR_NO_MEMORY);
        goto err;
    }

    char* csv = str_from_file(csv_path);
    if(!csv) {
        goto err;
    }

    font->type = type;
    font->texture = NULL;
    font->glyphs = NULL;
    font->average_glyph_size = 0;

    font->texture = rlr_texture_create(rlr, texture_atlas_path, false);
    if(!font->texture) {
        goto err;
    }

    if(!csv_parse(_rlr_font_csv_callback, csv, 10, ",", font)) {
        goto err;
    }

    //normalize the sizes
    font->average_glyph_size /= (hmlen(font->glyphs) * 2.0);
    for(int32_t i = 0; i < hmlen(font->glyphs); i++) {
        rlr_font_glyph_t glyph = font->glyphs[i];
        glyph.advance *= font->average_glyph_size;
        glyph.left_bearing *= font->average_glyph_size;
        glyph.offset_y = (glyph.offset_y * font->average_glyph_size) - font->average_glyph_size;
    }

    free(csv);
    return font;
err:
    free(csv);
    rlr_font_free(rlr, font);
    return NULL;
}

int32_t rlr_font_glyph_count(rlr_font_t* font) {
    return hmlen(font->glyphs);
}

void rlr_font_free(rlr_t* rlr, rlr_font_t* font) {
    if(font) {
        rlr_texture_free(rlr, font->texture);
        hmfree(font->glyphs);
    }
    free(font);
}