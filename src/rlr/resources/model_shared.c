#include <string.h>
#include <math.h>
#include "../../internal/impl.h"
#include "../resources/texture.h"
#include "../resources/uniform.h"
#include "../error.h"
#include "model_shared.h"

cgltf_data* rlr_res_model_load_glb(const char* filename) {
    cgltf_options options = {0};
    cgltf_data* data = NULL;
    cgltf_result result = cgltf_parse_file(&options, filename, &data);
    if(result != cgltf_result_success) {
        rlr_error_set(RLR_ERR_MODEL_FAILED_TO_LOAD);
        goto err;
    }
    if(data->file_type != cgltf_file_type_glb) {
        rlr_error_set(RLR_ERR_MODEL_IS_NOT_GLB);
        goto err;
    }
    result = cgltf_load_buffers(&options, data, filename);
    if(result != cgltf_result_success) {
        rlr_error_set(RLR_ERR_MODEL_IS_NOT_GLB);
        goto err;
    }
    result = cgltf_validate(data);
    if(result != cgltf_result_success) {
        rlr_error_set(RLR_ERR_MODEL_IS_NOT_GLB);
        goto err;
    }
    return data;
err:
    cgltf_free(data);
    return NULL;
}

bool rlr_res_model_parse_cgltf_material(cgltf_material* material, rlr_res_t* out_material_ubo, rlr_res_t* out_base_texture) {
    rlr_res_t ubo = RLR_NULL;
    rlr_res_t tex = RLR_NULL;
    if(!out_material_ubo || !out_base_texture) {
        goto err;
    }

    rlr_res_material_t mat;
    if(material) {
        tex = rlr_res_texture_load_cgltf_base(material->pbr_metallic_roughness.base_color_texture.texture);
        float roughness = material->pbr_metallic_roughness.roughness_factor;
        memcpy(mat.color, material->pbr_metallic_roughness.base_color_factor, sizeof(float) * 4);
        mat.shininess = 4.0 + powf(1.0 - roughness, 2.0) * 124.0;
        mat.specular_strength = 0.05 + (1.0 - roughness) * 0.45;
        mat.reflectiveness = (1.0 - roughness) * 0.25;
    } else {
        float def_color[4] = { 1.0, 1.0, 1.0, 1.0 };
        memcpy(mat.color, def_color, sizeof(float) * 4);
        mat.shininess = 32.0;
        mat.specular_strength = 0.2;
        mat.reflectiveness = 0.0;
    }
    ubo = rlr_res_uniform_create_static(&mat, sizeof(mat));
    if(ubo == RLR_NULL) {
        goto err;
    }

    *out_material_ubo = ubo;
    *out_base_texture = tex;
    return true;
err:
    rlr_res_uniform_free(ubo);
    rlr_res_texture_free(tex);
    return false;
}