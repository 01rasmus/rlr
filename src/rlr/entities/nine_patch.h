#pragma once
#include <stdint.h>
#include <stdbool.h>
#include "../math/anchor.h"
#include "../math/rect.h"
#include "../types/entity.h"
#include "../types/res.h"

typedef struct rlr_npatch_desc_t {
    float left;
    float right;
    float top;
    float bottom;
    bool show_middle;
} rlr_npatch_desc_t;

rlr_npatch_t* rlr_npatch_create(rlr_tex_t* texture, rlr_npatch_desc_t nine_patch, rlr_rect_t rectangle, rlr_anchor_t screen_anchor, rlr_anchor_t local_anchor, uint32_t layer);
void rlr_npatch_set_vis(rlr_npatch_t* np, bool visible);
void rlr_npatch_set_rect(rlr_npatch_t* np, rlr_rect_t rect);
void rlr_npatch_free(rlr_npatch_t* np);