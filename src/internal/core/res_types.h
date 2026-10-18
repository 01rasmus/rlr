#pragma once
#include <stdbool.h>
#include <stdint.h>
#include "../../rlr/resources/texture.h"
#include "../../rlr/math/vec.h"
#include "../../rlr/math/quat.h"

/*
    texture
*/
typedef struct rlr_tex_t {
    uint64_t texture;
    uint32_t channels;
    float width;
    float height;
} rlr_tex_t;

/*
    uniform
*/
typedef struct rlr_ubo_t {
    uint64_t buffer;
    uint64_t len;
    bool is_static;
    uint8_t slot;
} rlr_ubo_t;

/*
    shader
*/
#define RLR_SHADER_ERROR_LENGTH       4096
#define RLR_SHADER_INLINE(SRC)        "#version 330 core\n" #SRC
//#define RLR_SHADER_INLINE(SRC)        "#version 300 es\nprecision highp float;\nprecision highp int;\n" #SRC

typedef struct rlr_shader_t {
    uint64_t shader;
} rlr_shader_t;

/*
    cube map
*/
typedef struct rlr_cub_map_t {
    uint64_t texture;
} rlr_cub_map_t;

/*
    font
*/
typedef struct rlr_font_t rlr_font_t;

typedef struct rlr_font_glyph_t {
    uint32_t key; //unicode, the hashmap key
    rlr_font_t* font; //the font that this glyph belongs to
    float advance;
    float plane_left;
    float plane_right;
    float plane_bottom;
    float plane_top;
    float atlas_left;
    float atlas_right;
    float atlas_bottom;
    float atlas_top;
} rlr_font_glyph_t;

typedef struct rlr_font_t {
    rlr_font_t** fallback_fonts;
    size_t fallback_fonts_len;
    rlr_tex_t* texture;
    rlr_font_glyph_t* glyphs;
    float px_range;
    float vertical_offset;
    float size_scale;
} rlr_font_t;

/*
    shared model
*/
typedef struct cgltf_data cgltf_data;
typedef struct cgltf_material cgltf_material;

typedef struct rlr_material_t {
    float color[4];
    float shininess;
    float specular_strength;
    float reflectiveness;
} rlr_material_t;

cgltf_data* rlr_mdl_load_glb(const char* filename);
bool rlr_res_model_parse_cgltf_material(cgltf_material* material, rlr_ubo_t** out_material_ubo, rlr_tex_t** out_base_texture);

/*
    animated model
*/
#define RLR_ANM_MDL_ANIMATION_NAME_LENGTH        (128)
#define RLR_ANM_MDL_ANIMATION_TEXTURE_SIZE       (1920)
#define RLR_ANM_MDL_ANIMATION_FPS                (30.0)

typedef struct rlr_anm_vert_t {
    rlr_vec3_t pos;
    rlr_vec3_t normal;
    rlr_vec2_t uv;
    uint8_t weights[4];
    uint8_t joints[4];
} rlr_anm_vert_t;

typedef struct rlr_anm_mesh_t {
    rlr_tex_t* texture_base;
    rlr_ubo_t* material_ubo;
    uint64_t vbo;
    uint64_t ebo;
    uint64_t index_count;
    bool is_skinned;
} rlr_anm_mesh_t;

typedef struct rlr_animation_sample_t {
    rlr_vec3_t translation;
    rlr_quat_t rotation;
} rlr_animation_sample_t;

typedef struct rlr_animation_meta_t {
    char name[RLR_ANM_MDL_ANIMATION_NAME_LENGTH];
    float duration;
    float fps;
    uint64_t joint_count;
    uint64_t pose_offset; //where inside the texture this animation starts
    uint64_t pose_count;  //how many poses there are in the texture
} rlr_animation_meta_t;

typedef struct rlr_animations_t {
    uint64_t animation_texture;
    uint64_t joint_count;
    rlr_animation_meta_t* metas;
    rlr_animation_sample_t* samples;
} rlr_animations_t;

typedef struct rlr_anm_mdl_t {
    rlr_animations_t animations;
    rlr_anm_mesh_t* meshes;
} rlr_anm_mdl_t;

/*
    static model
*/
typedef struct rlr_sta_mdl_vert_t {
    rlr_vec3_t pos;
    rlr_vec3_t normal;
    rlr_vec2_t uv;
} rlr_sta_mdl_vert_t;

typedef struct rlr_sta_mesh_t {
    rlr_tex_t* texture_base;
    rlr_ubo_t* material_ubo;
    uint64_t vbo;
    uint64_t ebo;
    uint64_t index_count;
} rlr_sta_mesh_t;

typedef struct rlr_sta_mdl_t {
    rlr_sta_mesh_t* meshes;
} rlr_sta_mdl_t;

/*
    texture atlas
*/
typedef struct rlr_tex_atl_tile_t {
    rlr_tex_t* texture;
    rlr_vec2_t uv;
    rlr_vec2_t size;
} rlr_tex_atl_tile_t;

typedef struct rlr_tex_atl_tile_step_ctx_t {
    const char* filepath;
    void* dest;
    size_t dest_offset;
    uint32_t external_width;
    uint32_t external_height;
    uint32_t internal_width;
    uint32_t internal_height;
    uint32_t u;
    uint32_t v;
} rlr_tex_atl_tile_step_ctx_t;

typedef struct rlr_tex_atl_t {
    rlr_tex_t* texture;
    uint32_t channels;
    bool use_srgb_color_space;
    rlr_tex_filter_t filter;
    rlr_tex_atl_tile_t* tiles;
    size_t tile_count;

    //step
    uint8_t* temp_texture;
    uint32_t texture_side_size;
    rlr_tex_atl_tile_step_ctx_t* steps;
    size_t current_step;
    bool is_loaded;
} rlr_tex_atl_t;