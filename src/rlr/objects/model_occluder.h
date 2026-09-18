#pragma once
#include <stdbool.h>
#include <stdint.h>
#include "../math/anchor.h"
#include "../math/rect.h"
#include "../def.h"

typedef struct rlr_obj_model_occluder_t rlr_obj_model_occluder_t;

rlr_obj_model_occluder_t* rlr_obj_model_occluder_create(rlr_rect_t rectangle, rlr_anchor_t screen_anchor, rlr_anchor_t local_anchor);
void rlr_obj_model_occluder_set_visiblity(rlr_obj_model_occluder_t* mo, bool visible);
void rlr_obj_model_occluder_set_rectangle(rlr_obj_model_occluder_t* mo, rlr_rect_t rectangle);
void rlr_obj_model_occluder_free(rlr_obj_model_occluder_t* mo);