#pragma once
#include <stdbool.h>
#include <stdint.h>
#include "../../rlr/math/vec.h"
#include "../types/res.h"

/**
 * @param file_glb The file location of the animated model to load from
 * 
 * @return The pointer to the animated model. Returns `NULL` on failure
 * 
 * @warning The animated model needs to be free with `rlr_anm_mdl_free`
 * @warning Only binary glTF models are supported
 */
rlr_anm_mdl_t* rlr_anm_mdl_load(const char* file_glb);

/**
 * @param model The animated model to look up the animation index from
 * @param animation_name The name of the animation
 * 
 * @return The index of the animation. Returns -1 if the animation doesn't exist
 */
int32_t rlr_anm_mdl_get_anm_indx(const rlr_anm_mdl_t* model, const char* animation_name);

/**
 * @param model The pointer to the animated model to free. If `NULL` is provided, nothing will be done
 */
void rlr_anm_mdl_free(rlr_anm_mdl_t* model);