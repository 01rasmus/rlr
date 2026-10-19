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

#define rlr_npatch_desc(BORDER) \
    ((rlr_npatch_desc_t){.left = (BORDER), .right = (BORDER), .top = (BORDER), .bottom = (BORDER), .show_middle = false})

#define rlr_npatch_desc_mid(BORDER) \
    ((rlr_npatch_desc_t){.left = (BORDER), .right = (BORDER), .top = (BORDER), .bottom = (BORDER), .show_middle = true})

#define rlr_npatch_desc_lrtb(LEFT, RIGHT, TOP, BOT) \
    ((rlr_npatch_desc_t){.left = (LEFT), .right = (RIGHT), .top = (TOP), .bottom = (BOT), .show_middle = false})

#define rlr_npatch_desc_lrtb_mid(LEFT, RIGHT, TOP, BOT) \
    ((rlr_npatch_desc_t){.left = (LEFT), .right = (RIGHT), .top = (TOP), .bottom = (BOT), .show_middle = true})

rlr_npatch_t* rlr_npatch_create(rlr_tex_t* texture, rlr_npatch_desc_t nine_patch, rlr_rect_t rect, uint32_t layer);
void rlr_npatch_set_vis(rlr_npatch_t* np, bool visible);
void rlr_npatch_set_rect(rlr_npatch_t* np, rlr_rect_t rect);
void rlr_npatch_free(rlr_npatch_t* np);