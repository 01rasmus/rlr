#include <stb_ds.h>
#include <utf8.h>
#include "resources/font.h"
#include "math/vec.h"
#include "rlr.h"
#include "label.h"

typedef struct rlr_obj_label_vertex_t {
    rlr_vec3_t color;
    rlr_vec2_t pos;
    rlr_vec2_t uv;
    float screen_px_range;
} rlr_obj_label_vertex_t;

static void rlr_obj_label_upload_vertices(rlr_obj_label_t* label, const char* text) {
    rlr_obj_label_vertex_t* vertices = NULL;
    rlr_res_font_glyph_t* space_glyph = rlr_res_font_glyph_get(label->font, ' ');
    rlr_res_font_glyph_t* unknown_glyph = rlr_res_font_glyph_get(label->font, '?');
    float x = label->x;
    float y = label->y + label->size;
    float space_width = space_glyph ? ((space_glyph->advance * label->size)) : label->size;
    float tab_width = space_width * 8.0;
    float start_x = x;

    const void* p = text;
    utf8_int32_t unicode;
    while(p && *(const char*)p) {
        p = utf8codepoint(p, &unicode);

        if(unicode == ' ') {
            x += space_width;
            continue;
        }

        if (unicode == '\t') {
            float local_x = x - start_x;
            float next_tab = floorf(local_x / tab_width + 1.0f) * tab_width;
            x = start_x + next_tab;
            continue;
        }

        if(unicode == '\n') {
            y += label->size;
            x = start_x;
            continue;
        }

        rlr_res_font_glyph_t* glyph = rlr_res_font_glyph_get(label->font, unicode);
        if(!glyph) {
            if(!unknown_glyph) {
                continue;
            }
            glyph = unknown_glyph;
        }

        float draw_x1 = x + glyph->plane_left * label->size;
        float draw_y1 = y - glyph->plane_top * label->size;
        float draw_x2 = x + glyph->plane_right * label->size;
        float draw_y2 = y - glyph->plane_bottom * label->size;

        float u1 = glyph->atlas_left;
        float v2 = glyph->atlas_bottom;
        float u2 = glyph->atlas_right;
        float v1 = glyph->atlas_top;

        //calculate the screen px range
        float texture_width = (u2 - u1) * label->font->texture->width;
        float quad_width = draw_x2 - draw_x1;
        float screen_px_range = quad_width / texture_width * (label->font->px_range);
        if(screen_px_range < 1.0) {
            screen_px_range = 1.0;
        }

        rlr_obj_label_vertex_t vert1 = (rlr_obj_label_vertex_t){.pos = rlr_vec2(draw_x1, draw_y1), .uv = rlr_vec2(u1, v1), .color = label->color, .screen_px_range = screen_px_range};
        rlr_obj_label_vertex_t vert2 = (rlr_obj_label_vertex_t){.pos = rlr_vec2(draw_x2, draw_y1), .uv = rlr_vec2(u2, v1), .color = label->color, .screen_px_range = screen_px_range};
        rlr_obj_label_vertex_t vert3 = (rlr_obj_label_vertex_t){.pos = rlr_vec2(draw_x1, draw_y2), .uv = rlr_vec2(u1, v2), .color = label->color, .screen_px_range = screen_px_range};
        rlr_obj_label_vertex_t vert4 = (rlr_obj_label_vertex_t){.pos = rlr_vec2(draw_x2, draw_y2), .uv = rlr_vec2(u2, v2), .color = label->color, .screen_px_range = screen_px_range};

        arrpush(vertices, vert3);
        arrpush(vertices, vert2);
        arrpush(vertices, vert1);
        arrpush(vertices, vert3);
        arrpush(vertices, vert4);
        arrpush(vertices, vert2);

        x += glyph->advance * label->size;
    }

    _rlr_raw()->backend->buffer_bind(label->vbo, RLR_BACKEND_BUFFER_ARRAY);
    _rlr_raw()->backend->buffer_update(RLR_BACKEND_BUFFER_ARRAY, arrlenu(vertices) * sizeof(rlr_obj_label_vertex_t), vertices, RLR_BACKEND_BUFFER_USAGE_DYNAMIC);
    label->vertex_count = arrlenu(vertices);
    arrfree(vertices);
}

rlr_obj_label_t* rlr_obj_label_create(float x, float y, float size, const char* text, rlr_res_font_t* font) {
    rlr_obj_label_t* label = rlr_pipeline_ui_alloc_label();

    label->font = font;
    label->size = size;
    label->x = x;
    label->y = y;
    label->color = rlr_vec3(1.0, 0.9, 1.0);
    label->visible = true;
    label->vao = _rlr_raw()->backend->vertex_array_create();
    label->vbo = _rlr_raw()->backend->buffer_create();

    if(!label->vao || !label->vbo) {
        goto err;
    }
    
    _rlr_raw()->backend->vertex_array_bind(label->vao);
    _rlr_raw()->backend->buffer_bind(label->vbo, RLR_BACKEND_BUFFER_ARRAY);
    _rlr_raw()->backend->vertex_array_attrib_set(RLR_BACKEND_VERTEX_ARRAY_ATTRIB_PER_VERTEX, 0, 3, RLR_BACKEND_BUFFER_TYPE_FLOAT, false, sizeof(rlr_obj_label_vertex_t), offsetof(rlr_obj_label_vertex_t, color));
    _rlr_raw()->backend->vertex_array_attrib_set(RLR_BACKEND_VERTEX_ARRAY_ATTRIB_PER_VERTEX, 1, 2, RLR_BACKEND_BUFFER_TYPE_FLOAT, false, sizeof(rlr_obj_label_vertex_t), offsetof(rlr_obj_label_vertex_t, pos));
    _rlr_raw()->backend->vertex_array_attrib_set(RLR_BACKEND_VERTEX_ARRAY_ATTRIB_PER_VERTEX, 2, 2, RLR_BACKEND_BUFFER_TYPE_FLOAT, false, sizeof(rlr_obj_label_vertex_t), offsetof(rlr_obj_label_vertex_t, uv));
    _rlr_raw()->backend->vertex_array_attrib_set(RLR_BACKEND_VERTEX_ARRAY_ATTRIB_PER_VERTEX, 3, 1, RLR_BACKEND_BUFFER_TYPE_FLOAT, false, sizeof(rlr_obj_label_vertex_t), offsetof(rlr_obj_label_vertex_t, screen_px_range));
    
    rlr_obj_label_upload_vertices(label, text);
    return label;
err:
    rlr_obj_label_free(label);
    return NULL;
}

void rlr_obj_label_visible_set(rlr_obj_label_t* label, bool visible) {
    label->visible = visible;
}

void rlr_obj_label_text_set(rlr_obj_label_t* label, const char* text) {
    rlr_obj_label_upload_vertices(label, text);
}

void rlr_obj_label_free(rlr_obj_label_t* label) {
    if(!_rlr_raw() || !label) {
        return;
    }
    _rlr_raw()->backend->vertex_array_free(label->vao);
    _rlr_raw()->backend->buffer_free(label->vbo);
    rlr_pipeline_ui_free_label(label);
}