#include <stdlib.h>
#include <cgltf.h>
#include "../../external/stb_ds.h"
#include "../../internal/impl.h"
#include "../resources/model_shared.h"
#include "../resources/texture.h"
#include "../resources/uniform.h"
#include "../math/vec.h"
#include "../error.h"
#include "../rlr.h"
#include "static_model.h"

rlr_res_t rlr_res_static_model_load_glb(const char* glb_model_location) {
    rlr_res_t id = RLR_NULL;
    cgltf_data* data = NULL;
    rlr_res_static_model_t* model = NULL;
    rlr_static_model_vertex_t* vertices = NULL;
    uint32_t* indices = NULL;

    id = rlr_mem_man_allocate_res_static_model(rlr_mem_man(), (rlr_res_static_model_t){0});
    if(id == RLR_NULL) {
        rlr_error_set(RLR_ERR_NO_MEMORY);
        goto err;
    }

    model = rlr_mem_man_get_res_static_model(rlr_mem_man(), id);
    if(!model) {
        goto err;
    }

    model->meshes = NULL;
    data = rlr_res_model_load_glb(glb_model_location);
    if(!data) {
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

                rlr_static_model_vertex_t vertex = {
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

            printf("mesh %s: vertices=%u indices=%u triangles=%u\n",
                mesh->name,
                count_position,
                primitive->indices->count,
                primitive->indices->count / 3);

            //create mesh
            arrpush(model->meshes, (rlr_res_static_mesh_t){0});
            rlr_res_static_mesh_t* mesh = &arrlast(model->meshes);
            mesh->material_ubo = RLR_NULL;
            mesh->vbo = rlr_backend()->create_buffer();
            mesh->ebo = rlr_backend()->create_buffer();
            mesh->index_count = arrlenu(indices);
            if(mesh->vbo == 0 || mesh->ebo == 0) {
                rlr_error_set(RLR_ERR_BACKEND_NULL_HANDLE);
                goto err;
            }

            //fill buffers
            rlr_backend()->bind_vertex_array(0);
            rlr_backend()->bind_buffer(mesh->vbo, RLR_BACKEND_BUFFER_ARRAY);
            rlr_backend()->update_buffer(RLR_BACKEND_BUFFER_ARRAY, sizeof(rlr_static_model_vertex_t) * arrlenu(vertices), vertices, RLR_BACKEND_BUFFER_USAGE_STATIC);
            rlr_backend()->bind_buffer(mesh->ebo, RLR_BACKEND_BUFFER_ELEMENT_ARRAY);
            rlr_backend()->update_buffer(RLR_BACKEND_BUFFER_ELEMENT_ARRAY, sizeof(uint32_t) * arrlenu(indices), indices, RLR_BACKEND_BUFFER_USAGE_STATIC);

            //material
            if(!rlr_res_model_parse_cgltf_material(primitive->material, &mesh->material_ubo, &mesh->texture_base)) {
                goto err;
            }
        }
    }

    cgltf_free(data);
    arrfree(vertices);
    arrfree(indices);
    return id;
err:
    cgltf_free(data);
    arrfree(vertices);
    arrfree(indices);
    rlr_res_static_model_free(id);
    return RLR_NULL;
}

void rlr_res_static_model_free(rlr_res_t id) {
    if(id == RLR_NULL) {
        return;
    }

    rlr_res_static_model_t* model = rlr_mem_man_get_res_static_model(rlr_mem_man(), id);
    if(!model) {
        return;
    }

    for(int64_t i = 0; i < arrlen(model->meshes); i++) {
        rlr_res_static_mesh_t* mesh = &model->meshes[i];
        rlr_backend()->free_buffer(mesh->ebo);
        rlr_backend()->free_buffer(mesh->vbo);
        rlr_res_texture_free(mesh->texture_base);
        rlr_res_uniform_free(mesh->material_ubo);
    }
    arrfree(model->meshes);
    rlr_mem_man_free_res_static_model(rlr_mem_man(), id);
}