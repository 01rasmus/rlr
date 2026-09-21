#pragma once
#include <stdbool.h>
#include <stdint.h>
#include "../../rlr/resources/texture.h"
#include "../../rlr/math/vec.h"
#include "../../rlr/math/quat.h"

/*
    texture
*/
typedef struct rlr_res_texture_t {
    uint64_t texture;
    uint32_t channels;
    float width;
    float height;
} rlr_res_texture_t;

/*
    uniform
*/
typedef struct rlr_res_uniform_t {
    uint64_t buffer;
    uint64_t max_size;
    bool is_static;
    uint8_t slot;
} rlr_res_uniform_t;

/*
    shader
*/
#define RLR_SHADER_ERROR_LENGTH       4096
#define RLR_SHADER_INLINE(SRC)        "#version 330 core\n" #SRC
//#define RLR_SHADER_INLINE(SRC)        "#version 300 es\nprecision highp float;\nprecision highp int;\n" #SRC

typedef struct rlr_res_shader_t {
    uint64_t shader;
} rlr_res_shader_t;

/*
    cube map
*/
typedef struct rlr_res_cube_map_t {
    uint64_t texture;
} rlr_res_cube_map_t;

/*
    font
*/
typedef struct rlr_res_font_glyph_t {
    uint32_t key; //unicode, the hashmap key
    float advance;
    float plane_left;
    float plane_right;
    float plane_bottom;
    float plane_top;
    float atlas_left;
    float atlas_right;
    float atlas_bottom;
    float atlas_top;
} rlr_res_font_glyph_t;

typedef struct rlr_res_font_t {
    rlr_res_texture_t* texture;
    rlr_res_font_glyph_t* glyphs;
    float px_range;
    float vertical_offset;
} rlr_res_font_t;

/*
    shared model
*/
typedef struct cgltf_data cgltf_data;
typedef struct cgltf_material cgltf_material;

typedef struct rlr_res_material_t {
    float color[4];
    float shininess;
    float specular_strength;
    float reflectiveness;
} rlr_res_material_t;

cgltf_data* rlr_res_model_load_glb(const char* filename);
bool rlr_res_model_parse_cgltf_material(cgltf_material* material, rlr_res_uniform_t** out_material_ubo, rlr_res_texture_t** out_base_texture);

/*
    animated model
*/
#define RLR_RES_ANIMATED_MODEL_ANIMATION_NAME_LENGTH        (128)
#define RLR_RES_ANIMATED_MODEL_ANIMATION_TEXTURE_SIZE       (2048)
#define RLR_RES_ANIMATED_MODEL_ANIMATION_FPS                (30.0)

typedef struct rlr_animated_model_vertex_t {
    rlr_vec3_t pos;
    rlr_vec3_t normal;
    rlr_vec2_t uv;
    uint8_t weights[4];
    uint8_t joints[4];
} rlr_animated_model_vertex_t;

typedef struct rlr_res_animated_mesh_t {
    rlr_res_texture_t* texture_base;
    rlr_res_uniform_t* material_ubo;
    uint64_t vbo;
    uint64_t ebo;
    uint64_t index_count;
    bool is_skinned;
} rlr_res_animated_mesh_t;

typedef struct rlr_res_animation_meta_t {
    char name[RLR_RES_ANIMATED_MODEL_ANIMATION_NAME_LENGTH];
    float duration;
    float fps;
    uint64_t pose_offset; //where inside the texture this animation starts
    uint64_t pose_count;  //how many poses there are in the texture
} rlr_res_animation_meta_t;

typedef struct rlr_res_animations_t {
    uint64_t animation_texture;
    uint64_t joint_count;
    rlr_res_animation_meta_t* metas;
} rlr_res_animations_t;

typedef struct rlr_res_animated_model_t {
    rlr_res_animations_t animations;
    rlr_res_animated_mesh_t* meshes;
} rlr_res_animated_model_t;

/*
    static model
*/
typedef struct rlr_static_model_vertex_t {
    rlr_vec3_t pos;
    rlr_vec3_t normal;
    rlr_vec2_t uv;
} rlr_static_model_vertex_t;

typedef struct rlr_res_static_mesh_t {
    rlr_res_texture_t* texture_base;
    rlr_res_uniform_t* material_ubo;
    uint64_t vbo;
    uint64_t ebo;
    uint64_t index_count;
} rlr_res_static_mesh_t;

typedef struct rlr_res_static_model_t {
    rlr_res_static_mesh_t* meshes;
} rlr_res_static_model_t;

/*
    texture atlas
*/
typedef struct rlr_res_texture_atlas_tile_t {
    rlr_res_texture_t* texture;
    rlr_vec2_t uv;
    rlr_vec2_t size;
} rlr_res_texture_atlas_tile_t;

typedef struct rlr_res_texture_atlas_tile_step_ctx_t {
    const char* filepath;
    void* dest;
    size_t dest_offset;
    uint32_t external_width;
    uint32_t external_height;
    uint32_t internal_width;
    uint32_t internal_height;
    uint32_t u;
    uint32_t v;
} rlr_res_texture_atlas_tile_step_ctx_t;

typedef struct rlr_res_texture_atlas_t {
    rlr_res_texture_t* texture;
    uint32_t channels;
    bool use_srgb_color_space;
    rlr_res_texture_filter_t filter;
    rlr_res_texture_atlas_tile_t* tiles;

    //step
    uint8_t* temp_texture;
    uint32_t texture_side_size;
    rlr_res_texture_atlas_tile_step_ctx_t* steps;
    size_t step_count;
    size_t current_step;
    bool is_loaded;
} rlr_res_texture_atlas_t;