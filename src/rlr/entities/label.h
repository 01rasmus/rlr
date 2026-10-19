#pragma once
#include <stdbool.h>
#include <stdint.h>
#include "../math/vec.h"
#include "../math/rect.h"
#include "../math/anchor.h"
#include "../math/alignment.h"
#include "../types/entity.h"
#include "../types/res.h"

rlr_lbl_t* rlr_lbl_create(rlr_rect_t rect, float size, uint32_t layer, const char* text, rlr_font_t* font);
rlr_lbl_t* rlr_lbl_create_ext(rlr_rect_t rectangle, float text_size, uint32_t layer, rlr_font_t* font, rlr_anchor_t screen_anchor, rlr_anchor_t local_anhor, rlr_horizontal_alignment_t horizontal_alignment, rlr_vertical_alignment_t vertical_alignment, const char* format, ...);
void rlr_lbl_set_text(rlr_lbl_t* label, const char* text);
void rlr_lbl_set_vis(rlr_lbl_t* label, bool visible);
void rlr_lbl_set_rect(rlr_lbl_t* label, rlr_rect_t rect);
void rlr_lbl_free(rlr_lbl_t* label);