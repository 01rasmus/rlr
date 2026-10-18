#pragma once
#include <stdint.h>
#include "../types/res.h"
#include "../math/vec.h"

/**
 * @param file_glb The file location to load the static model from
 * 
 * @return The pointer to the static model. Returns `NULL` on failure
 * 
 * @warning The static model needs to be free with `rlr_sta_mdl_free`
 * @warning Only binary glTF models are supported
 */
rlr_sta_mdl_t* rlr_sta_mdl_load(const char* file_glb);

/**
 * @param model The pointer to the static model to free. If `NULL` is provided, nothing will be done
 */
void rlr_sta_mdl_free(rlr_sta_mdl_t* model);