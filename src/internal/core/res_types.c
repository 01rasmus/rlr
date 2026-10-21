#include <inttypes.h>
#include <string.h>
#include <math.h>
#include <cgltf.h>
#include <stb_image.h>
#include "../../rlr/resources/texture.h"
#include "../../rlr/resources/uniform.h"
#include "res_types.h"
#include "../impl.h"

cgltf_data* rlr_mdl_load_glb(const char* filename) {
    cgltf_options options = {0};
    cgltf_data* data = NULL;
    cgltf_result result = cgltf_parse_file(&options, filename, &data);
    if(result != cgltf_result_success) {
        rlr_log_error("failed to parse glb file");
        goto err;
    }
    if(data->file_type != cgltf_file_type_glb) {
        rlr_log_error("the model file is not of the type \"glb\"");
        goto err;
    }
    result = cgltf_load_buffers(&options, data, filename);
    if(result != cgltf_result_success) {
        rlr_log_error("failed to load glb buffers");
        goto err;
    }
    result = cgltf_validate(data);
    if(result != cgltf_result_success) {
        rlr_log_error("glb model validation failed");
        goto err;
    }
    return data;
err:
    cgltf_free(data);
    return NULL;
}

bool rlr_mdl_parse_cgltf_material(cgltf_material* material, rlr_ubo_t** out_material_ubo, rlr_tex_t** out_base_texture) {
    if(!out_material_ubo || !out_base_texture) {
        goto err;
    }

    rlr_tex_t* tex = NULL;
    rlr_ubo_t* ubo = NULL;

    rlr_material_t mat;
    if(material) {
        tex = rlr_mdll_load_cgltf_albedo_tex(material->pbr_metallic_roughness.base_color_texture.texture);
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

    ubo = rlr_ubo_create(&mat, sizeof(mat), true);
    if(ubo == NULL) {
        goto err;
    }

    *out_material_ubo = ubo;
    *out_base_texture = tex;
    return true;
err:
    rlr_ubo_free(ubo);
    rlr_tex_free(tex);
    return false;
}

rlr_tex_t* rlr_mdll_load_cgltf_albedo_tex(cgltf_texture* tex) {
    if(!tex) {
        return NULL;
    }

    if(!tex || !tex->image || !tex->image->buffer_view) {
        rlr_log_error("glb texture isn't loaded");
        return NULL;
    }

    cgltf_buffer_view* view = tex->image->buffer_view;
    if(!view->buffer || !view->buffer->data) {
        rlr_log_error("glb texture isn't loaded");
        return NULL;
    }

    cgltf_sampler sampler;
    if(tex->sampler) {
        sampler = (*tex->sampler);
    } else {
        sampler = (cgltf_sampler) {
            .min_filter = cgltf_filter_type_linear_mipmap_nearest,
            .mag_filter = cgltf_filter_type_linear,
            .wrap_s = cgltf_wrap_mode_clamp_to_edge,
            .wrap_t = cgltf_wrap_mode_clamp_to_edge
        };
    }

    uint32_t width;
    uint32_t height;
    uint32_t channels;
    uint8_t* data = stbi_load_from_memory((uint8_t*)view->buffer->data + view->offset, view->size, &width, &height, &channels, 0);
    if(!data) {
        return NULL;
    }

    rlr_tex_t* new_tex = rlr_tex_load_mem_raw_impl(data, width, height, channels, sampler.min_filter, sampler.mag_filter, sampler.wrap_s, sampler.wrap_t, RLR_TEX_FLAG_COMPRESS | RLR_TEX_FLAG_SRGB);
    stbi_image_free(data);
    return new_tex;
}

rlr_tex_t* rlr_tex_load_mem_raw_impl(const void* color, uint32_t width, uint32_t height, uint32_t channels, uint32_t min_filter, uint32_t mag_filter, uint32_t wrap_s, uint32_t wrap_t, uint64_t flags) {
    rlr_tex_t* tex = NULL;

    if(width % 4 != 0 || height % 4 != 0) {
        rlr_log_error("texture's size(%dx%d) has a side that is not divisible by 4", width, height);
        goto err;
    }

    if(channels == 0 || channels > 4) {
        rlr_log_error("invalid channel count %" PRIu32, channels);
        goto err;
    }

    tex = malloc(sizeof(rlr_tex_t));
    if(!tex) {
        rlr_log_error("pointer to the texture handle for texture is null");
        goto err;
    }
    tex->texture = 0;
    tex->width = width;
    tex->height = height;

    bool compress = (flags & RLR_TEX_FLAG_COMPRESS) != 0;
    bool srgb = (flags & RLR_TEX_FLAG_SRGB) != 0;

    if(compress) {
        tex->texture = rlr_backend()->create_compressed_texture(color, width, height, channels, srgb, min_filter, mag_filter, wrap_s, wrap_t);
    } else {
        tex->texture = rlr_backend()->create_texture(color, width, height, channels, srgb, min_filter, mag_filter, wrap_s, wrap_t);
    }

    if(tex->texture == 0) {
        rlr_log_error("backend handle for texture \"%s\" is null");
        goto err;
    }
    
    return tex;
err:
    rlr_tex_free(tex);
    return NULL;
}

rlr_tex_t* rlr_tex_load_mem_raw_preset_filter_impl(const void* color, uint32_t width, uint32_t height, uint32_t channels, rlr_tex_filter_t filter, uint64_t flags){
    uint32_t min_filter;
    uint32_t mag_filter;
    uint32_t wrap_s;
    uint32_t wrap_t;

    switch(filter) {
        case RLR_TEX_FILTER_LINEAR: {
            min_filter = RLR_BACKEND_FILTER_LINEAR;
            mag_filter = RLR_BACKEND_FILTER_LINEAR;
            wrap_s = RLR_BACKEND_FILTER_CLAMP_TO_EDGE;
            wrap_t = RLR_BACKEND_FILTER_CLAMP_TO_EDGE;
            break;
        }
        case RLR_TEX_FILTER_LINEAR_MIPMAP: {
            min_filter = RLR_BACKEND_FILTER_LINEAR_MIPMAP;
            mag_filter = RLR_BACKEND_FILTER_LINEAR;
            wrap_s = RLR_BACKEND_FILTER_CLAMP_TO_EDGE;
            wrap_t = RLR_BACKEND_FILTER_CLAMP_TO_EDGE;
            break;
        }
        case RLR_TEX_FILTER_NEAREST: {
            min_filter = RLR_BACKEND_FILTER_NEAREST;
            mag_filter = RLR_BACKEND_FILTER_NEAREST;
            wrap_s = RLR_BACKEND_FILTER_CLAMP_TO_EDGE;
            wrap_t = RLR_BACKEND_FILTER_CLAMP_TO_EDGE;
            break;
        }
        default:
            rlr_log_error("invalid filter id 0x%" PRIx32, filter);
            return NULL;
    }

    return rlr_tex_load_mem_raw_impl(color, width, height, channels, min_filter, mag_filter, wrap_s, wrap_t, flags);
}