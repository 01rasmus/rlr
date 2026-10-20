#pragma once
#include <stdbool.h>
#include <stdint.h>
#include "../math/rect.h"
#include "../math/alignment.h"
#include "../types/entity.h"
#include "../types/res.h"

rlr_lbl_t* rlr_lbl_new(rlr_font_t* font, float x, float y, float size, uint32_t layer, const char* format, ...);
rlr_lbl_t* rlr_lbl_new_rect(rlr_font_t* font, rlr_rect_t rect, float size, uint32_t layer, const char* format, ...);
rlr_lbl_t* rlr_lbl_new_rect_algn(rlr_font_t* font, rlr_rect_t rect, float size, rlr_hori_algn_t hori_algn, rlr_vert_algn_t vert_algn, uint32_t layer, const char* format, ...);
void rlr_lbl_set_textf(rlr_lbl_t* label, const char* format, ...);
void rlr_lbl_set_textf_rect(rlr_lbl_t* label, rlr_rect_t rect, const char* format, ...);
void rlr_lbl_set_vis(rlr_lbl_t* label, bool visible);
void rlr_lbl_free(rlr_lbl_t* label);