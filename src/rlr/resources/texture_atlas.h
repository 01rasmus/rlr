#pragma once
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "../types/res.h"
#include "../math/vec.h"
#include "texture.h"

/**
 * @brief Creates a texture atlas tile entry to be loaded to a texture atlas
 * 
 * @warning The tile will be automatically padded to make sure linear filtering doesn't intefere with other tiles during rendering
 * 
 * @param file The file location of the texture to load
 * @param dst The pointer to the atlas tile pointer to set after the tiles are loaded
 * 
 * @return The description struct to use to load tiles in the atlas
 */
static inline const rlr_tex_atl_tile_dsc_t rlr_tex_atl_tile_dsc(const char* file, rlr_tex_atl_tile_t** dst) {
    return (rlr_tex_atl_tile_dsc_t) {
        .filepath = file,
        .destination = dst,
    };
}

/**
 * @brief Creates a texture atlas tile entry to be loaded and resized into a texture atlas
 * 
 * @warning The tile will be automatically padded to make sure linear filtering doesn't intefere with other tiles during rendering
 * 
 * @param file The file location of the texture to load
 * @param dst The pointer to the atlas tile pointer to set after the tiles are loaded
 * @param resize_width The width to resize to
 * @param resize_height The height to resize to
 * 
 * @return The description struct to use to load tiles in the atlas
 */
static inline const rlr_tex_atl_tile_dsc_t rlr_tex_atl_tile_dsc_rsz(const char* file, rlr_tex_atl_tile_t** dst, uint32_t resize_width, uint32_t resize_height) {
    return (rlr_tex_atl_tile_dsc_t) {
        .filepath = file,
        .destination = dst,
        .resize_width = resize_width,
        .resize_height = resize_height
    };
}

/**
 * @brief Creates a texture atlas tile entry to be loaded, add padding and resized into a texture atlas
 * 
 * @param file The file location of the texture to load
 * @param dst The pointer to the atlas tile pointer to set after the tiles are loaded
 * @param resize_width The width to resize to
 * @param resize_height The height to resize to
 * @param padding Override of the edge padding. The other functions automatically add padding
 * 
 * @return The description struct to use to load tiles in the atlas
 */
static inline const rlr_tex_atl_tile_dsc_t rlr_tex_atl_tile_dsc_rsz_pad(const char* file, rlr_tex_atl_tile_t** dst, uint32_t resize_width, uint32_t resize_height, uint32_t padding) {
    return (rlr_tex_atl_tile_dsc_t) {
        .filepath = file,
        .destination = dst,
        .resize_width = resize_width,
        .resize_height = resize_height,
        .padding = padding
    };
}

/** 
 * @warning This function loads the tiles instantly
 * @warning The resulting texture atlas cannot be larger than 4096x4096
 * 
 * @param descriptions The array of tile descriptions to load
 * @param description_count The amount of descriptions
 * @param channels The resulting texture atlas' channel count
 * @param srgb Wether the resulting texture should be interpreted as srgb or linear color space while rendering
 * @param filter The filter. See @ref rlr_tex_filter_t
 * 
 * @return A pointer to the texture atlas. On failure, `NULL` is returned
 * 
 * @warning The font needs to be freed with `rlr_font_free`
 * @warning After the texture atlas is freed, all the `rlr_tex_atl_tile_t` tiles become invalid
 */
rlr_tex_atl_t* rlr_tex_atl_load(rlr_tex_atl_tile_dsc_t* descriptions, size_t description_count, uint32_t channels, bool srgb, rlr_tex_filter_t filter);

/** 
 * @warning This function loads does not load the tile textures. To load them, @ref `rlr_tex_atl_atlas_step` needs to be called the same amount of times as there are description entries to divide the load into steps. To see the amount, @ref `rlr_tex_atl_get_step_cnt` can be used
 * @warning The resulting texture atlas cannot be larger than 4096x4096
 * 
 * @param descriptions The array of tile descriptions to load
 * @param description_count The amount of descriptions
 * @param channels The resulting texture atlas' channel count
 * @param srgb Wether the resulting texture should be interpreted as srgb or linear color space while rendering
 * @param filter The filter. See @ref rlr_tex_filter_t
 * 
 * @return A pointer to the texture atlas. On failure, `NULL` is returned
 * 
 * @warning The font needs to be freed with `rlr_font_free`
 * @warning After the texture atlas is freed, all the `rlr_tex_atl_tile_t` tiles become invalid
 */
rlr_tex_atl_t* rlr_tex_atl_load_stpd(rlr_tex_atl_tile_dsc_t* descriptions, size_t description_count, uint32_t channels, bool srgb, rlr_tex_filter_t filter);

/**
 * @param atlas The texture atlas that is returned from @ref `rlr_tex_atl_load_stpd`
 * 
 * @return The amount of steps needed to load the texture atlas fully
 */
size_t rlr_tex_atl_get_step_cnt(const rlr_tex_atl_t* atlas);

/**
 * @param atlas The texture atlas that is returned from @ref `rlr_tex_atl_load_stpd`
 * 
 * @return True if the atlas is successfully loaded.
 *         It returns false when it isn't done.
 *         Normally, this function will return true whenever
 *         the function has been called the same amount of times
 *         as there are load steps. However, if it instead returns
 *         false, the texture atlas failed to be created
 */
bool rlr_tex_atl_atlas_step(rlr_tex_atl_t* atlas);

/**
 * @param ubo The pointer to the texture atlas to free. If `NULL` is provided, nothing will be done
 */
void rlr_tex_atl_free(rlr_tex_atl_t* atlas);