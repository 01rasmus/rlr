#pragma once
#include <stdbool.h>
#include <stdint.h>
#include "../math/vec.h"
#include "../math/rect.h"
#include "../math/anchor.h"
#include "../math/alignment.h"
#include "../def.h"

typedef struct rlr_res_font_t rlr_res_font_t;

typedef struct rlr_obj_label_t {
    rlr_res_t font;
    uint64_t cmd_id;
    uint64_t* instance_indices;
    rlr_rect_t rectangle;
    rlr_anchor_t screen_anchor;
    rlr_anchor_t local_anchor;
    rlr_horizontal_alignment_t horizontal_alignment;
    rlr_vertical_alignment_t vertical_alignment;
    float size;
} rlr_obj_label_t;

rlr_obj_t rlr_obj_label_create(rlr_rect_t rect, float size, uint32_t layer, const char* text, rlr_res_t font);
rlr_obj_t rlr_obj_label_create_ext(rlr_rect_t rectangle, float text_size, uint32_t layer, rlr_res_t font, rlr_anchor_t screen_anchor, rlr_anchor_t local_anhor, rlr_horizontal_alignment_t horizontal_alignment, rlr_vertical_alignment_t vertical_alignment, const char* format, ...);
void rlr_obj_label_set_text(rlr_obj_t label, const char* text);
void rlr_obj_label_set_visability(rlr_obj_t label, bool visible);
void rlr_obj_label_free(rlr_obj_t obj);