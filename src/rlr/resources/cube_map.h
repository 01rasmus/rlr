#pragma once
#include <stdint.h>
#include "../types/res.h"

/**
 * @param file_right The file location for the right texture
 * @param file_left The file location for the left texture
 * @param file_top The file location for the top texture
 * @param file_bottom The file location for the bottom texture
 * @param file_front The file location for the front texture
 * @param file_back The file location for the back texture
 * 
 * @return The pointer to the cube map. Returns `NULL` on failure
 * 
 * @warning The cube map needs to be freed with `rlr_cub_map_free`
 */
rlr_cub_map_t* rlr_cub_map_load(const char* file_right, const char* file_left, const char* file_top, const char* file_bottom, const char* file_front, const char* file_back);

/**
 * @param cm The cube map to bind
 * @param texture_slot Which slot to bind the cube map's textures to
 */
void rlr_cub_map_bind(const rlr_cub_map_t* cm, uint8_t texture_slot);

/**
 * @param model The pointer to the cube map to free. If `NULL` is provided, nothing will be done
 */
void rlr_cub_map_free(rlr_cub_map_t* cm);