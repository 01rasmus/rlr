#pragma once
#include <stdbool.h>
#include "../types/res.h"

/**
 * @param initial_data Base pointer to the data to initialize with. If `NULL` is provided the data will not be initialized
 * @param data_size The size of the initialize data
 * @param is_static Wether the UBO is static or not. If it isn't, the data cannot be changed
 * 
 * @return The pointer to the UBO. Returns `NULL` on failure
 * 
 * @warning The UBO needs to be free with `rlr_ubo_free`
 */
rlr_ubo_t* rlr_ubo_create(const void* initial_data, size_t data_size, bool is_static);

/**
 * @param ubo The UBO to update. If the UBO is static, this function does nothing
 * @param offset Offset into the UBO to copy the new data to
 * @param data The base pointer to the data to copy to the UBO
 * @param size The amount of data to copy
 */
void rlr_ubo_update(rlr_ubo_t* ubo, size_t offset, const void* data, size_t size);

/**
 * @param ubo The UBO to bind
 * @param ubo_slot Which slot to bind the UBO to
 */
void rlr_ubo_bind(const rlr_ubo_t* ubo, uint8_t ubo_slot);

/**
 * @param ubo The pointer to the UBO to free. If `NULL` is provided, nothing will be done
 */
void rlr_ubo_free(rlr_ubo_t* ubo);