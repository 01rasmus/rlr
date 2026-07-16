#pragma once
#include <stdint.h>
#include "../math/vec.h"
#include "../def.h"

typedef struct rlr_static_model_vertex_t {
    rlr_vec3_t pos;
    rlr_vec3_t normal;
    rlr_vec2_t uv;
} rlr_static_model_vertex_t;

typedef struct rlr_res_static_mesh_t {
    rlr_res_t texture_base;
    rlr_res_t material_ubo;
    uint64_t vbo;
    uint64_t ebo;
    uint64_t index_count;
} rlr_res_static_mesh_t;

typedef struct rlr_res_static_model_t {
    rlr_res_static_mesh_t* meshes;
} rlr_res_static_model_t;

rlr_res_t rlr_res_static_model_load_glb(const char* glb_model_location);
void rlr_res_static_model_free(rlr_res_t model);