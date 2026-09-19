#pragma once
#include <stdint.h>
#include <stdbool.h>
#include "../math/anchor.h"
#include "../math/rect.h"

typedef struct rlr_obj_nine_patch_t rlr_obj_nine_patch_t;
typedef struct rlr_res_texture_t rlr_res_texture_t;

typedef struct rlr_nine_patch_t {
    float left;
    float right;
    float top;
    float bottom;
    bool show_middle;
} rlr_nine_patch_t;

rlr_obj_nine_patch_t* rlr_obj_nine_patch_create(rlr_res_texture_t* texture, rlr_nine_patch_t nine_patch, rlr_rect_t rectangle, rlr_anchor_t screen_anchor, rlr_anchor_t local_anchor, uint32_t layer);
void rlr_obj_nine_patch_set_visability(rlr_obj_nine_patch_t* np, bool visible);
void rlr_obj_nine_patch_set_rectangle(rlr_obj_nine_patch_t* np, rlr_rect_t rect);
void rlr_obj_nine_patch_free(rlr_obj_nine_patch_t* np);