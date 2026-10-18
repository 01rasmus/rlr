#pragma once
#include <stdint.h>
#include "../types/res.h"

/**
 * @param file_vertex The file location of the vertex shader to load
 * @param file_fragment The file location of the vertex shader to load
 * 
 * @return The pointer to the shader. Returns `NULL` on failure
 * 
 * @warning The shader needs to be free with `rlr_shader_free`
 */
rlr_shader_t* rlr_shader_load(const char* file_vertex, const char* file_fragment);

/**
 * @param string_vertex The vertex shader string
 * @param string_fragment The fragment shader string
 * 
 * @return The pointer to the shader. Returns `NULL` on failure
 * 
 * @warning The shader needs to be free with `rlr_shader_free`
 */
rlr_shader_t* rlr_shader_load_mem(const char* string_vertex, const char* string_fragment);

/**
 * @related See also `rlr_ubo_bind`
 * 
 * @param shader The shader where the block should be bound
 * @param block_var_name The name of the block to bind a slot to
 * @param uniform_slot The uniform slot to bind to
 */
void rlr_shader_bind_ubo_slot(rlr_shader_t* shader, const char* block_var_name, uint8_t uniform_slot);

/**
 * @related See also `rlr_tex_bind`
 * 
 * @param shader The shader where the block should be bound
 * @param texture_var_name The name of the texture sampler to bind a slot to
 * @param texture_slot The texture slot to bind to
 */
void rlr_shader_bind_tex_slot(rlr_shader_t* shader, const char* texture_var_name, uint8_t texture_slot);

/**
 * @param shader The shader to bind
 */
void rlr_shader_bind(const rlr_shader_t* shader);

/**
 * @param ubo The pointer to the shader to free. If `NULL` is provided, nothing will be done
 */
void rlr_shader_free(rlr_shader_t* shader);