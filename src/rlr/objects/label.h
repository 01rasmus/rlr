#pragma once
#include <stdint.h>
#include "rlr/math/vec.h"

typedef struct rlr_res_font_t rlr_res_font_t;
typedef struct rlr_t rlr_t;

typedef struct rlr_obj_label_t {
    uint32_t index;
    rlr_res_font_t* font;
    uint32_t vao;
    uint32_t vbo;
    uint32_t vertex_count;
    rlr_vec3_t color;
    float x;
    float y;
    float size;
    bool visible;
} rlr_obj_label_t;

rlr_obj_label_t* rlr_obj_label_create(float x, float y, float size, const char* text, rlr_res_font_t* font);
void rlr_obj_label_set_text(rlr_obj_label_t* label, const char* text);
void rlr_obj_label_set_visability(rlr_obj_label_t* label, bool visible);
void rlr_obj_label_free(rlr_obj_label_t* label_handle);