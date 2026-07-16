#pragma once
#include <stdint.h>
#include "rlr/math/anchor.h"
#include "rlr/math/rect.h"
#include "rlr/def.h"

typedef struct rlr_res_texture_t rlr_res_texture_t;

typedef struct rlr_obj_sprite_t {
    rlr_res_t texture;
    rlr_anchor_t screen_anchor;
    rlr_anchor_t local_anchor;
    rlr_rect_t rectangle;
    rlr_rect_t scissor;
    rlr_rect_t uv;
    int32_t layer;
    uint32_t index;
} rlr_obj_sprite_t;

rlr_obj_t rlr_obj_sprite_create(rlr_res_t texture, rlr_rect_t rectangle, rlr_anchor_t screen_anchor, rlr_anchor_t local_anchor, int32_t layer);
rlr_obj_t rlr_obj_sprite_create_ext(rlr_res_t texture, rlr_rect_t rectangle, rlr_anchor_t screen_anchor, rlr_anchor_t local_anchor, int32_t layer, rlr_rect_t uv, rlr_rect_t scissor);
void rlr_obj_sprite_free(rlr_obj_t sprite);