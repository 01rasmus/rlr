#pragma once
#include <stdbool.h>
#include <stdint.h>
#include "../../rlr/math/vec.h"
#include "../../rlr/def.h"

typedef struct rlr_res_animated_model_t rlr_res_animated_model_t;

rlr_res_animated_model_t* rlr_res_animated_model_load_glb(const char* glb_model_location);
void rlr_res_animated_model_free(rlr_res_animated_model_t* model);

/*
    returns -1 if the animation doesnt exist
*/
int32_t rlr_res_animated_model_get_animation_index(const rlr_res_animated_model_t* model, const char* animation_name);