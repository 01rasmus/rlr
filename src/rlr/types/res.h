#pragma once
#include <stddef.h>
#include <stdint.h>

/**
 * Animated model resource
 */
typedef struct rlr_anm_mdl_t rlr_anm_mdl_t;

/**
 * Cube map resource
 */
typedef struct rlr_cub_map_t rlr_cub_map_t;

/**
 * Font resource
 */
typedef struct rlr_font_t rlr_font_t;

/**
 * Font glyph resource
 */
typedef struct rlr_font_glyph_t rlr_font_glyph_t;

/**
 * Shader resource
 */
typedef struct rlr_shader_t rlr_shader_t;

/**
 * Static model resource
 */
typedef struct rlr_sta_mdl_t rlr_sta_mdl_t;

/**
 * Texture atlas resource
 */
typedef struct rlr_tex_atl_t rlr_tex_atl_t;

/**
 * Texture atlas tile resource
 */
typedef struct rlr_tex_atl_tile_t rlr_tex_atl_tile_t;

/**
 * Texture atlas tile description used to load texture atlases
 */
typedef struct rlr_tex_atl_tile_dsc_t rlr_tex_atl_tile_dsc_t;

/**
 * Texture resources
 */
typedef struct rlr_tex_t rlr_tex_t;

/**
 * Uniform buffer object resource
 */
typedef struct rlr_ubo_t rlr_ubo_t;

typedef enum rlr_tex_flag_t {
    RLR_TEX_FLAG_SRGB               = 0x1,
    RLR_TEX_FLAG_COMPRESS           = 0x2
} rlr_tex_flag_t;

typedef enum rlr_tex_filter_t {
    RLR_TEX_FILTER_LINEAR_MIPMAP    = 0x0,
    RLR_TEX_FILTER_NEAREST          = 0x1,
    RLR_TEX_FILTER_LINEAR           = 0x2
} rlr_tex_filter_t;

typedef struct rlr_tex_atl_tile_dsc_t {
    const char* filepath;
    uint32_t resize_width;
    uint32_t resize_height;
    uint32_t padding;
    rlr_tex_atl_tile_t** destination;
} rlr_tex_atl_tile_dsc_t;