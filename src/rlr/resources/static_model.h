#pragma once
#include <stdint.h>
#include "../math/vec.h"
#include "../def.h"

typedef struct rlr_res_static_model_t rlr_res_static_model_t;

rlr_res_static_model_t* rlr_res_static_model_load_glb(const char* glb_model_location);
void rlr_res_static_model_free(rlr_res_static_model_t* model);