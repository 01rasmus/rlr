#pragma once
#include <stdbool.h>
#include <stdint.h>
#include "../math/anchor.h"
#include "../math/rect.h"
#include "../math/color.h"
#include "../types/entity.h"
#include "../types/res.h"

rlr_spr_t* rlr_spr_create(rlr_tex_t* tex, rlr_rect_t rect, uint32_t layer);
rlr_spr_t* rlr_spr_create_from_tile(rlr_tex_atl_tile_t* tile, rlr_rect_t rect, uint32_t layer);
rlr_spr_t* rlr_spr_create_col_rect(rlr_color_t col, rlr_rect_t rect, uint32_t layer);
void rlr_spr_set_tex_from_tile(rlr_spr_t* sprite, rlr_tex_atl_tile_t* tile);
void rlr_spr_set_tex(rlr_spr_t* sprite, rlr_tex_t* texture);
void rlr_spr_set_col(rlr_spr_t* sprite, rlr_color_t col);
void rlr_spr_set_vis(rlr_spr_t* sprite, bool visible);
void rlr_spr_set_rect(rlr_spr_t* sprite, rlr_rect_t rect);
rlr_rect_t rlr_spr_get_rect(const rlr_spr_t* sprite);
uint32_t rlr_spr_get_col(const rlr_spr_t* sprite);
void rlr_spr_free(rlr_spr_t* sprite);