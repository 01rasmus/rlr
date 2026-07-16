#pragma once
#include <stdint.h>
#include "rlr/math/vec.h"
#include "rlr/def.h"

#define RLR_RES_ANIMATED_MODEL_ANIMATION_NAME_LENGTH        (128)
#define RLR_RES_ANIMATED_MODEL_ANIMATION_TEXTURE_SIZE       (2048)
#define RLR_RES_ANIMATED_MODEL_ANIMATION_FPS                (30.0)

typedef struct cgltf_material cgltf_material;

typedef struct rlr_animated_model_vertex_t {
    rlr_vec3_t pos;
    rlr_vec3_t normal;
    rlr_vec2_t uv;
    uint8_t weights[4];
    uint8_t joints[4];
} rlr_animated_model_vertex_t;

typedef struct rlr_res_animated_mesh_t {
    rlr_res_t texture_base;
    rlr_res_t material_ubo;
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

rlr_res_t rlr_res_animated_model_load_glb(const char* glb_model_location);
void rlr_res_animated_model_free(rlr_res_t model);

/*
    returns -1 if the animation doesnt exist
*/
int32_t rlr_res_animated_model_get_animation_index(rlr_res_t model, const char* animation_name);