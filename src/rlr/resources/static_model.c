#include <stdlib.h>
#include <cgltf.h>
#define RLMESH_IMPLEMENTATION
#define RLMESH_STATIC
#include <rlmesh/rlmesh.h>
#include "../../internal/core/res_types.h"
#include "../../external/stb_ds.h"
#include "../../internal/impl.h"
#include "../resources/texture.h"
#include "../resources/uniform.h"
#include "../math/vec.h"
#include "../rlr.h"
#include "static_model.h"

rlr_sta_mdl_t* rlr_sta_mdl_load(const char* file_glb) {
    cgltf_data* data = NULL;
    rlr_sta_mdl_t* model = NULL;
    rlr_sta_mdl_vert_t* vertices = NULL;
    uint32_t* indices = NULL;
    rlmesh_t* optimized_mesh = NULL;

    model = malloc(sizeof(rlr_sta_mdl_t));
    if(!model) {
        rlr_log_error("pointer to the static model is null");
        goto err;
    }

    model->meshes = NULL;
    data = rlr_mdl_load_glb(file_glb);
    if(!data) {
        goto err;
    }

    for(size_t i = 0; i < data->meshes_count; i++) {
        cgltf_mesh* mesh = &data->meshes[i];

        for(size_t p = 0; p < mesh->primitives_count; p++) {
            cgltf_primitive* primitive = &mesh->primitives[p];
            if(primitive->type != cgltf_primitive_type_triangles) {
                rlr_log_error("the animated model did not contain triangle primitives");
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
                rlr_log_error("the animated model did not have any position attributes");
                goto err;
            }
            if(!accessor_normal || count_normal != count_position) {
                rlr_log_error("the normal count did not match the position count");
                goto err;
            }
            if(accessor_uv && count_uv != count_position) {
                rlr_log_error("the uv count did not match the position count");
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

                rlr_sta_mdl_vert_t vertex = {
                    .pos = rlr_vec3(pos[0], pos[1], pos[2]),
                    .normal = rlr_vec3(norm[0], norm[1], norm[2]),
                    .uv = rlr_vec2(uv[0], uv[1]),
                };
                arrpush(vertices, vertex);
            }

            //load indices
            if(primitive->indices) {
                for(size_t ind = 0; ind < primitive->indices->count; ind++) {
                    size_t index = cgltf_accessor_read_index(primitive->indices, ind);
                    if(index > UINT32_MAX) {
                        rlr_log_error("index value %zu is larger than the largest allowed 32 bit unsigned integer limit", index);
                        goto err;
                    }

                    arrpush(indices, (uint32_t)index);
                }
            }

            size_t index_count = arrlenu(indices);
            optimized_mesh = rlmesh_create(vertices, sizeof(vertices[0]), arrlenu(vertices), indices, sizeof(uint32_t), index_count);
            if(!optimized_mesh) {
                rlr_log_error("unable to optimize glb model \"%s\"", file_glb);
                goto err;
            }

            rlr_log_debug(
                "optimization stats for \"%s\"\n\t\t\tbefore\tafter\n\tnum vert\t%zu\t%zu\n\tnum indx\t%zu\t%zu\n\n\tindices size\t%zu\n\tvertices size\t%zu",
                mesh->name,
                arrlenu(vertices),
                optimized_mesh->vertex_count,
                arrlenu(indices),
                optimized_mesh->index_count,
                optimized_mesh->indices_size,
                optimized_mesh->vertices_size
            );

            //create mesh
            arrpush(model->meshes, (rlr_sta_mesh_t){0});
            rlr_sta_mesh_t* mesh = &arrlast(model->meshes);
            mesh->material_ubo = NULL;
            mesh->vbo = rlr_backend()->create_buffer();
            mesh->ebo = rlr_backend()->create_buffer();
            mesh->index_count = optimized_mesh->index_count;
            if(mesh->vbo == 0 || mesh->ebo == 0) {
                rlr_log_error("unable to create buffers");
                goto err;
            }

            //fill buffers
            rlr_backend()->bind_vertex_array(0);
            rlr_backend()->bind_buffer(mesh->vbo, RLR_BACKEND_BUFFER_ARRAY);
            rlr_backend()->update_buffer(RLR_BACKEND_BUFFER_ARRAY, optimized_mesh->vertices_size, optimized_mesh->vertices, RLR_BACKEND_BUFFER_USAGE_STATIC);
            rlr_backend()->bind_buffer(mesh->ebo, RLR_BACKEND_BUFFER_ELEMENT_ARRAY);
            rlr_backend()->update_buffer(RLR_BACKEND_BUFFER_ELEMENT_ARRAY, optimized_mesh->indices_size, optimized_mesh->indices, RLR_BACKEND_BUFFER_USAGE_STATIC);

            //free optimized mesh
            rlmesh_free(optimized_mesh);
            optimized_mesh = NULL;

            //material
            if(!rlr_mdl_parse_cgltf_material(primitive->material, &mesh->material_ubo, &mesh->texture_base)) {
                goto err;
            }
        }
    }

    cgltf_free(data);
    arrfree(vertices);
    arrfree(indices);
    rlr_log("loaded static model %s", file_glb);
    return model;
err:
    cgltf_free(data);
    arrfree(vertices);
    arrfree(indices);
    rlmesh_free(optimized_mesh);
    rlr_sta_mdl_free(model);
    return NULL;
}

void rlr_sta_mdl_free(rlr_sta_mdl_t* model) {
    if(!model) {
        return;
    }

    for(int64_t i = 0; i < arrlen(model->meshes); i++) {
        rlr_sta_mesh_t* mesh = &model->meshes[i];
        rlr_backend()->free_buffer(mesh->ebo);
        rlr_backend()->free_buffer(mesh->vbo);
        rlr_tex_free(mesh->texture_base);
        rlr_ubo_free(mesh->material_ubo);
    }
    arrfree(model->meshes);
    free(model);
}