#pragma once
#include <stddef.h>
#include <stdint.h>
#include "../types/res.h"

typedef struct cgltf_texture cgltf_texture;

/**
 * @param file The location of an encoded image file like a .png or .jpeg
 * @param filter The filter that the gpu should sample the texture with
 * @param flags Settings for how the texture should be stored. See @ref rlr_tex_flag_t
 * 
 * @return Returns a pointer to the loaded texture. Returns `NULL` on failure
 * 
 * @warning The texture needs to be free with `rlr_tex_free`
 */
rlr_tex_t* rlr_tex_load(const char* file, rlr_tex_filter_t filter, uint64_t flags);

/**
 * @param color The base pointer to an encoded texture like a .png or .jpeg in memory
 * @param color_len The size in bytes of the color data
 * @param filter The filter that the gpu should sample the texture with
 * @param flags Settings for how the texture should be stored. See @ref rlr_tex_flag_t
 * 
 * @return Returns a pointer to the loaded texture. Returns `NULL` on failure
 * 
 * @warning The texture needs to be free with `rlr_tex_free`
 */
rlr_tex_t* rlr_tex_load_mem(const void* color, size_t color_len, rlr_tex_filter_t filter, uint64_t flags);

/**
 * @param file The file location of a raw, uncompressed, and non-encoded texture
 * @param width The width of the texture
 * @param height The height of the texture
 * @param channels The channel count of each pixel (the amount of bytes per pixel)
 * @param filter The filter that the gpu should sample the texture with
 * @param flags Settings for how the texture should be stored. See @ref rlr_tex_flag_t
 * 
 * @return Returns a pointer to the loaded texture. Returns `NULL` on failure
 * 
 * @warning The texture needs to be free with `rlr_tex_free`
 */
rlr_tex_t* rlr_tex_load_raw(const char* file, uint32_t width, uint32_t height, uint32_t channels, rlr_tex_filter_t filter, uint64_t flags);

/**
 * @param color The base pointer of a raw, uncompressed, and non-encoded texture in memory
 * @param width The width of the texture
 * @param height The height of the texture
 * @param channels The channel count of each pixel (the amount of bytes per pixel)
 * @param filter The filter that the gpu should sample the texture with
 * @param flags Settings for how the texture should be stored. See @ref rlr_tex_flag_t
 * 
 * @return Returns a pointer to the loaded texture. Returns `NULL` on failure
 * 
 * @details The size of the color buffer needs to be at least width * height * channels
 * 
 * @warning The texture needs to be free with `rlr_tex_free`
 */
rlr_tex_t* rlr_tex_load_mem_raw(const void* color, uint32_t width, uint32_t height, uint32_t channels, rlr_tex_filter_t filter, uint64_t flags);

/**
 * @param texture The texture to bind
 * @param texture_slot Which slot to bind the texture to
 */
void rlr_tex_bind(const rlr_tex_t* texture, uint8_t texture_slot);

/**
 * @param texture The pointer to the texture to free. If `NULL` is provided, nothing will be done
 */
void rlr_tex_free(rlr_tex_t* texture);

rlr_tex_t* rlr_tex_load_cgltf_albedo(cgltf_texture* tex);