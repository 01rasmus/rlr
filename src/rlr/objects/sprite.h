#pragma once
#include <stdbool.h>
#include <stdint.h>
#include "../math/anchor.h"
#include "../math/rect.h"
#include "../def.h"

typedef struct rlr_res_texture_t rlr_res_texture_t;

typedef struct rlr_obj_sprite_t {
    rlr_rect_t rect;
    rlr_anchor_t local_anchor;
    uint64_t cmd_id;
    uint64_t instance_index;
} rlr_obj_sprite_t;

rlr_obj_t rlr_obj_sprite_create(rlr_res_t texture, rlr_rect_t rectangle, rlr_anchor_t screen_anchor, rlr_anchor_t local_anchor, uint32_t layer);
rlr_obj_t rlr_obj_sprite_create_ext(rlr_res_t texture, rlr_rect_t rectangle, rlr_anchor_t screen_anchor, rlr_anchor_t local_anchor, uint32_t layer, rlr_rect_t uv, rlr_rect_t scissor);
rlr_obj_t rlr_obj_sprite_create_from_atlas_tile(rlr_res_t texture_atlas_tile, rlr_rect_t rectangle, rlr_anchor_t screen_anchor, rlr_anchor_t local_anchor, uint32_t layer);
void rlr_obj_sprite_set_color(rlr_obj_t sprite, uint8_t r, uint8_t g, uint8_t b, uint8_t a);
void rlr_obj_sprite_set_visability(rlr_obj_t sprite, bool visible);
void rlr_obj_sprite_set_rectangle(rlr_obj_t sprite, rlr_rect_t rect);
void rlr_obj_sprite_set_local_anchor(rlr_obj_t sprite, rlr_anchor_t anchor);
void rlr_obj_sprite_set_screen_anchor(rlr_obj_t sprite, rlr_anchor_t anchor);
rlr_rect_t rlr_obj_sprite_get_rectangle(rlr_obj_t sprite);
rlr_anchor_t rlr_obj_sprite_get_local_anchor(rlr_obj_t sprite);
rlr_anchor_t rlr_obj_sprite_get_screen_anchor(rlr_obj_t sprite);
uint32_t rlr_obj_sprite_get_color(rlr_obj_t sprite);
void rlr_obj_sprite_free(rlr_obj_t sprite);