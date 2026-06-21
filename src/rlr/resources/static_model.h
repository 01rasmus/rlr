#pragma once

typedef struct rlr_res_texture_t rlr_res_texture_t;

typedef struct rlr_res_material_t {
    float color[4];
    float shininess;
    float specular_strength;
    float reflectiveness;
} rlr_res_material_t;

typedef struct rlr_res_static_mesh_t {
    rlr_res_texture_t* texture_base;
    rlr_res_material_t material;
    uint64_t vao;
    uint64_t vbo;
    uint64_t ebo;
    uint64_t index_count;
} rlr_res_static_mesh_t;

typedef struct rlr_res_static_model_t {
    rlr_res_static_mesh_t* meshes;
} rlr_res_static_model_t;

rlr_res_static_model_t* rlr_res_static_model_load(const char* glb_model_location);
void rlr_res_static_model_free(rlr_res_static_model_t* model);