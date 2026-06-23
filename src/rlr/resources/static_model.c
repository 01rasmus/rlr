#include <stdlib.h>
#include <cgltf.h>
#include <stb_ds.h>
#include "internal/rlr.h"
#include "rlr/resources/texture.h"
#include "rlr/math/vec.h"
#include "rlr/error.h"
#include "rlr/rlr.h"
#include "static_model.h"

typedef struct rlr_model_static_vertex_t {
    rlr_vec3_t pos;
    rlr_vec3_t normal;
    rlr_vec2_t uv;
} rlr_model_static_vertex_t;

rlr_res_static_model_t* rlr_res_static_model_load(const char* glb_model_location) {
    rlr_res_static_model_t* model = NULL;
    rlr_model_static_vertex_t* vertices = NULL;
    uint32_t* indices = NULL;
    model = malloc(sizeof(rlr_res_static_model_t));
    if(!model) {
        goto err;
    }

    model->meshes = NULL;

    cgltf_options options = {0};
    cgltf_data* data = NULL;
    cgltf_result result = cgltf_parse_file(&options, glb_model_location, &data);
    if(result != cgltf_result_success) {
        rlr_error_set(RLR_ERR_MODEL_FAILED_TO_LOAD);
        goto err;
    }
    if(data->file_type != cgltf_file_type_glb) {
        rlr_error_set(RLR_ERR_MODEL_IS_NOT_GLB);
        goto err;
    }
    result = cgltf_load_buffers(&options, data, glb_model_location);
    if(result != cgltf_result_success) {
        rlr_error_set(RLR_ERR_MODEL_IS_NOT_GLB);
        goto err;
    }
    result = cgltf_validate(data);
    if(result != cgltf_result_success) {
        rlr_error_set(RLR_ERR_MODEL_IS_NOT_GLB);
        goto err;
    }

    for(size_t i = 0; i < data->meshes_count; i++) {
        cgltf_mesh* mesh = &data->meshes[i];

        for(size_t p = 0; p < mesh->primitives_count; p++) {
            cgltf_primitive* primitive = &mesh->primitives[p];
            if(primitive->type != cgltf_primitive_type_triangles) {
                rlr_error_set(RLR_ERR_MODEL_PRIMITIVE_NOT_TRIANGLES);
                goto err;
            }

            arrsetlen(vertices, 0);
            arrsetlen(indices, 0);
            size_t count_position = 0;
            size_t count_normal = 0;
            size_t count_uv = 0;
            cgltf_accessor* accessor_position = NULL;
            cgltf_accessor* accessor_normal = NULL;
            cgltf_accessor* accessor_uv = NULL;

            for(size_t a = 0; a < primitive->attributes_count; a++) {
                cgltf_attribute* attribute = &primitive->attributes[a];
                switch(attribute->type) {
                    case cgltf_attribute_type_position: {
                        accessor_position = attribute->data;
                        count_position = attribute->data->count;
                        break;
                    }
                    case cgltf_attribute_type_normal: {
                        accessor_normal = attribute->data;
                        count_normal = attribute->data->count;
                        break;
                    }
                    case cgltf_attribute_type_texcoord: {
                        accessor_uv = attribute->data;
                        count_uv = attribute->data->count;
                        break;
                    }
                    default:
                        break;
                }
            }

            if(!accessor_position || count_position == 0) {
                rlr_error_set(RLR_ERR_MODEL_NO_POSITION_ATTRIBUTE);
                goto err;
            }
            if(!accessor_normal || count_normal != count_position) {
                rlr_error_set(RLR_ERR_MODEL_ATTRIBUTE_COUNT_ARE_DIFFERENT);
                goto err;
            }
            if(accessor_uv && count_uv != count_position) {
                rlr_error_set(RLR_ERR_MODEL_ATTRIBUTE_COUNT_ARE_DIFFERENT);
                goto err;
            }

            //load vertices
            for(size_t v = 0; v < count_position; v++) {
                float pos[3];
                float norm[3] = {0};
                float uv[2] = {0};
                cgltf_accessor_read_float(accessor_position, v, pos, 3);
                if(accessor_normal) {
                    cgltf_accessor_read_float(accessor_normal, v, norm, 3);
                }
                if(accessor_uv) {
                    cgltf_accessor_read_float(accessor_uv, v, uv, 2);
                }

                rlr_model_static_vertex_t vertex = {
                    .pos = rlr_vec3(pos[0], pos[1], pos[2]),
                    .normal = rlr_vec3(norm[0], norm[1], norm[2]),
                    .uv = rlr_vec2(uv[0], uv[1]),
                };
                arrpush(vertices, vertex);
            }

            //load indices
            for(size_t ind = 0; ind < primitive->indices->count; ind++) {
                uint32_t index = cgltf_accessor_read_index(primitive->indices, ind);
                arrpush(indices, index);
            }

            //create mesh
            arrpush(model->meshes, (rlr_res_static_mesh_t){0});
            rlr_res_static_mesh_t* mesh = &arrlast(model->meshes);
            mesh->vao = rlr_backend()->create_vertex_array();
            mesh->vbo = rlr_backend()->create_buffer();
            mesh->ebo = rlr_backend()->create_buffer();
            mesh->index_count = arrlenu(indices);
            if(mesh->vao == 0 || mesh->vbo == 0 || mesh->ebo == 0) {
                rlr_error_set(RLR_ERR_BACKEND_NULL_HANDLE);
                goto err;
            }

            //set vertex array and fill buffers
            rlr_backend()->bind_vertex_array(mesh->vao);
            rlr_backend()->bind_buffer(mesh->vbo, RLR_BACKEND_BUFFER_ARRAY);
            rlr_backend()->update_buffer(RLR_BACKEND_BUFFER_ARRAY, sizeof(rlr_model_static_vertex_t) * arrlenu(vertices), vertices, RLR_BACKEND_BUFFER_USAGE_STATIC);
            rlr_backend()->set_vertex_array_attrib(RLR_BACKEND_VERTEX_ARRAY_ATTRIB_PER_VERTEX, 0, 3, RLR_BACKEND_BUFFER_TYPE_FLOAT, false, sizeof(rlr_model_static_vertex_t), offsetof(rlr_model_static_vertex_t, pos));
            rlr_backend()->set_vertex_array_attrib(RLR_BACKEND_VERTEX_ARRAY_ATTRIB_PER_VERTEX, 1, 3, RLR_BACKEND_BUFFER_TYPE_FLOAT, false, sizeof(rlr_model_static_vertex_t), offsetof(rlr_model_static_vertex_t, normal));
            rlr_backend()->set_vertex_array_attrib(RLR_BACKEND_VERTEX_ARRAY_ATTRIB_PER_VERTEX, 2, 2, RLR_BACKEND_BUFFER_TYPE_FLOAT, false, sizeof(rlr_model_static_vertex_t), offsetof(rlr_model_static_vertex_t, uv));
            rlr_backend()->bind_buffer(mesh->ebo, RLR_BACKEND_BUFFER_ELEMENT_ARRAY);
            rlr_backend()->update_buffer(RLR_BACKEND_BUFFER_ELEMENT_ARRAY, sizeof(uint32_t) * arrlenu(indices), indices, RLR_BACKEND_BUFFER_USAGE_STATIC);

            //material
            cgltf_material* material = primitive->material;
            if(material) {
                mesh->texture_base = rlr_res_texture_load_cgltf_base(material->pbr_metallic_roughness.base_color_texture.texture);
                float roughness = material->pbr_metallic_roughness.roughness_factor;
                memcpy(mesh->material.color, material->pbr_metallic_roughness.base_color_factor, sizeof(float) * 4);
                mesh->material.shininess = 4.0 + powf(1.0 - roughness, 2.0) * 124.0;
                mesh->material.specular_strength = 0.05 + (1.0 - roughness) * 0.45;
                mesh->material.reflectiveness = (1.0 - roughness) * 0.25;
                printf("%p\n", material->pbr_metallic_roughness.metallic_roughness_texture.texture);
            } else {
                float def_color[4] = { 1.0, 1.0, 1.0, 1.0 };
                memcpy(mesh->material.color, def_color, sizeof(float) * 4);
                mesh->material.shininess = 32.0;
                mesh->material.specular_strength = 0.2;
                mesh->material.reflectiveness = 0.0;
            }
        }
    }

    cgltf_free(data);
    arrfree(vertices);
    arrfree(indices);
    return model;
err:
    cgltf_free(data);
    arrfree(vertices);
    arrfree(indices);
    rlr_res_static_model_free(model);
    return NULL;
}

void rlr_res_static_model_free(rlr_res_static_model_t* model) {
    if(!model) {
        return;
    }

    for(int64_t i = 0; i < arrlen(model->meshes); i++) {
        rlr_res_static_mesh_t* mesh = &model->meshes[i];
        rlr_backend()->free_buffer(mesh->ebo);
        rlr_backend()->free_buffer(mesh->vbo);
        rlr_backend()->free_vertex_array(mesh->vao);
    }
    arrfree(model->meshes);
    free(model);
}