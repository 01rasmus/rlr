#pragma once
#include <stdbool.h>
/*
    this file includes things that are shared
    between both the static and the animated
    model resources
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
bool rlr_res_model_parse_cgltf_material(cgltf_material* material, rlr_res_t* out_material_ubo, rlr_res_t* out_base_texture);