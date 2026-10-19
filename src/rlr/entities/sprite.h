#pragma once
#include <stdbool.h>
#include <stdint.h>
#include "../math/anchor.h"
#include "../math/rect.h"
#include "../types/entity.h"
#include "../types/res.h"

rlr_spr_t* rlr_spr_create(rlr_tex_t* texture, rlr_rect_t rectangle, rlr_anchor_t screen_anchor, rlr_anchor_t local_anchor, uint32_t layer);
rlr_spr_t* rlr_spr_create_ext(rlr_tex_t* texture, rlr_rect_t rectangle, rlr_anchor_t screen_anchor, rlr_anchor_t local_anchor, uint32_t layer, rlr_rect_t uv, rlr_rect_t scissor);
rlr_spr_t* rlr_spr_create_from_tile(rlr_tex_atl_tile_t* texture_atlas_tile, rlr_rect_t rectangle, rlr_anchor_t screen_anchor, rlr_anchor_t local_anchor, uint32_t layer);

void rlr_spr_set_tex_from_tile(rlr_spr_t* sprite, rlr_tex_atl_tile_t* texture_atlas_tile);
void rlr_spr_set_tex(rlr_spr_t* sprite, rlr_tex_t* texture);
void rlr_spr_set_col(rlr_spr_t* sprite, uint8_t r, uint8_t g, uint8_t b, uint8_t a);
void rlr_spr_set_vis(rlr_spr_t* sprite, bool visible);
void rlr_spr_set_rect(rlr_spr_t* sprite, rlr_rect_t rect);
void rlr_spr_set_lcl_anch(rlr_spr_t* sprite, rlr_anchor_t anchor);
void rlr_spr_set_scrn_anch(rlr_spr_t* sprite, rlr_anchor_t anchor);
rlr_rect_t rlr_spr_get_rect(const rlr_spr_t* sprite);
rlr_anchor_t rlr_spr_get_lcl_anch(const rlr_spr_t* sprite);
rlr_anchor_t rlr_spr_get_scrn_anch(const rlr_spr_t* sprite);
uint32_t rlr_spr_get_col(const rlr_spr_t* sprite);

void rlr_spr_free(rlr_spr_t* sprite);