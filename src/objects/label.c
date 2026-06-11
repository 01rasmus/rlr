#include <stb_ds.h>
#include <utf8.h>
#include "resources/font.h"
#include "gpu_math.h"
#include "rlr.h"
#include "label.h"

typedef struct rlr_obj_label_vertex_t {
    vec2_t pos;
    vec2_t uv;
} rlr_obj_label_vertex_t;

static void rlr_obj_label_upload_vertices(rlr_t* rlr, rlr_obj_label_t* label, const char* text) {
    rlr_obj_label_vertex_t* vertices = NULL;

    rlr_font_glyph_t* space_glyph = rlr_font_glyph_get(label->font, 32);
    rlr_font_glyph_t* unknown_glyph = rlr_font_glyph_get(label->font, '?');
    float x = label->x;
    float y = label->y;
    float scale = label->size;
    float space_width = space_glyph ? ((space_glyph->advance) * scale) : label->size;
    float start_x = x;

    const void* p = text;
    utf8_int32_t unicode;
    while(p && *(const char*)p) {
        p = utf8codepoint(p, &unicode);

        if(unicode == ' ') {
            x += space_width;
            continue;
        }

        if(unicode == '\n') {
            y += label->size;
            x = start_x;
            continue;
        }

        rlr_font_glyph_t* glyph = rlr_font_glyph_get(label->font, unicode);
        if(!glyph) {
            if(!unknown_glyph) {
                continue;
            }
            glyph = unknown_glyph;
        }

        float draw_x1 = x + glyph->plane_left * label->size;
        float draw_y1 = y + glyph->plane_bottom * label->size;
        float draw_x2 = x + glyph->plane_right * label->size;
        float draw_y2 = y + glyph->plane_top * label->size;

        float u1 = glyph->atlas_left;
        float v2 = glyph->atlas_bottom;
        float u2 = glyph->atlas_right;
        float v1 = glyph->atlas_top;

        rlr_obj_label_vertex_t vert1 = (rlr_obj_label_vertex_t){.pos = vec2(draw_x1, draw_y1), .uv = vec2(u1, v1)};
        rlr_obj_label_vertex_t vert2 = (rlr_obj_label_vertex_t){.pos = vec2(draw_x2, draw_y1), .uv = vec2(u2, v1)};
        rlr_obj_label_vertex_t vert3 = (rlr_obj_label_vertex_t){.pos = vec2(draw_x1, draw_y2), .uv = vec2(u1, v2)};
        rlr_obj_label_vertex_t vert4 = (rlr_obj_label_vertex_t){.pos = vec2(draw_x2, draw_y2), .uv = vec2(u2, v2)};

        arrpush(vertices, vert1);
        arrpush(vertices, vert2);
        arrpush(vertices, vert3);
        arrpush(vertices, vert2);
        arrpush(vertices, vert4);
        arrpush(vertices, vert3);

        x += glyph->advance * label->size;
    }

    rlr->backend->buffer_bind(label->vbo, RLR_BACKEND_BUFFER_ARRAY);
    rlr->backend->buffer_update(label->vbo, RLR_BACKEND_BUFFER_ARRAY, arrlenu(vertices) * sizeof(rlr_obj_label_vertex_t), vertices, RLR_BACKEND_BUFFER_USAGE_STATIC);
    label->vertex_count = arrlenu(vertices);
    arrfree(vertices);
}

rlr_obj_label_t* rlr_obj_label_create(rlr_t* rlr, float x, float y, float size, const char* text, rlr_font_t* font) {
    arrpush(rlr->obj_labels, (rlr_obj_label_t){0});
    rlr_obj_label_t* label = &arrlast(rlr->obj_labels);

    label->index = arrlenu(rlr->obj_labels) - 1;
    label->font = font;
    label->size = size;
    label->x = x;
    label->y = y;
    label->visible = true;
    label->vao = rlr->backend->vertex_array_create();
    label->vbo = rlr->backend->buffer_create();

    if(!label->vao || !label->vbo) {
        goto err;
    }
    rlr->backend->vertex_array_bind(label->vao);
    rlr->backend->buffer_bind(label->vbo, RLR_BACKEND_BUFFER_ARRAY);
    
    rlr->backend->vertex_array_attrib_pointer(label->vao, 0, 2, RLR_BACKEND_BUFFER_TYPE_FLOAT, false, sizeof(rlr_obj_label_vertex_t), offsetof(rlr_obj_label_vertex_t, pos));
    rlr->backend->vertex_array_attrib_pointer(label->vao, 1, 2, RLR_BACKEND_BUFFER_TYPE_FLOAT, false, sizeof(rlr_obj_label_vertex_t), offsetof(rlr_obj_label_vertex_t, uv));

    rlr_obj_label_upload_vertices(rlr, label, text);
    return label;
err:
    rlr_obj_label_free(rlr, label);
    return NULL;
}

void rlr_obj_label_visible_set(rlr_t* rlr, rlr_obj_label_t* label, bool visible) {
    label->visible = false;
}

void rlr_obj_label_text_set(rlr_t* rlr, rlr_obj_label_t* label, const char* text) {
    rlr_obj_label_upload_vertices(rlr, label, text);
}

void rlr_obj_label_free(rlr_t* rlr, rlr_obj_label_t* label) {
    if(!rlr || !label) {
        return;
    }
    rlr->backend->vertex_array_free(label->vao);
    rlr->backend->buffer_free(label->vbo);
}