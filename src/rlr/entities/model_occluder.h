#pragma once
#include <stdbool.h>
#include "../math/rect.h"
#include "../types/entity.h"

rlr_mdl_occl_t* rlr_mdl_occl_new(rlr_rect_t rect);
void rlr_mdl_occl_set_vis(rlr_mdl_occl_t* mo, bool visible);
void rlr_mdl_occl_set_rect(rlr_mdl_occl_t* mo, rlr_rect_t rect);
void rlr_mdl_occl_free(rlr_mdl_occl_t* mo);