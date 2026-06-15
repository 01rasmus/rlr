#pragma once
#include "backends/backend.h"
#include "gpu_math.h"

typedef struct rlr_texture_t rlr_texture_t;

typedef struct rlr_material_t {
    float color[4];
    float shininess;
    float specular_strength;
    float reflectiveness;
} rlr_material_t;

typedef struct rlr_mesh_static_t {
    rlr_texture_t* texture_base;
    rlr_material_t material;
    rlr_handle_t vao;
    rlr_handle_t vbo;
    rlr_handle_t ebo;
    uint64_t index_count;
} rlr_mesh_static_t;

typedef struct rlr_model_static_t {
    rlr_mesh_static_t* meshes;
} rlr_model_static_t;

rlr_model_static_t* rlr_model_static_create(const char* glb_model_location);
void rlr_model_static_free(rlr_model_static_t* model);