#pragma once
#include <stdint.h>
#include "rlr/math/anchor.h"
#include "rlr/math/rect.h"
#include "rlr/def.h"

typedef struct rlr_obj_model_occluder_t {
    rlr_anchor_t screen_anchor;
    rlr_anchor_t local_anchor;
    rlr_rect_t rectangle;
    uint32_t index;
} rlr_obj_model_occluder_t;

rlr_obj_t rlr_obj_model_occluder_create(rlr_rect_t rectangle, rlr_anchor_t screen_anchor, rlr_anchor_t local_anchor);
void rlr_obj_model_occluder_free(rlr_obj_t mo);