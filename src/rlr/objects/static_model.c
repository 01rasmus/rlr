#include <stdlib.h>
#include "external/rlr_stb_ds.h"
#include "internal/pipelines/model.h"
#include "rlr/math/matrix.h"
#include "static_model.h"

rlr_obj_static_model_handle_t rlr_obj_static_model_create(rlr_res_static_model_t* model, rlr_vec3_t translation, rlr_quat_t rotation, rlr_vec3_t scale) {
    rlr_obj_static_model_handle_t handle = rlr_pipeline_model_alloc_static_model();
    rlr_obj_static_model_t* obj = rlr_pipeline_model_get_static_model(handle);
    if(!obj) {
        goto err;
    }

    rlr_pipeline_static_model_draw_command_t* cmd = rlr_pipeline_model_find_static_model_draw_command(model, NULL);
    if(!cmd) {
        goto err;
    }

    (*obj) = (rlr_obj_static_model_t){
        .translation = translation,
        .rotation = rotation,
        .scale = scale,
        .opaque = true,
        .alpha = 1.0,
        .cmd_index = cmd->index,
        .cmd_generation = cmd->generation,
        .instance_index = 0,
    };
    rlr_mat4x4_t trs = rlr_mat4x4_trs(&translation, &rotation, &scale);
    rlr_affine_mat4x3_t affine = rlr_mat4x4_to_affine_mat4x3(&trs);
    rlr_pipeline_static_model_instance_t instance = {.matrix = affine};
    obj->instance_index = rlr_pipeline_model_add_static_model_instance(cmd, instance);
    return handle;
err:
    rlr_obj_static_model_free(handle);
    return RLR_SPARSE_GEN_ALLOCATOR_NULL_HANDLE;
}

rlr_vec3_t rlr_obj_static_model_get_translation(rlr_obj_static_model_handle_t model) {
    return rlr_pipeline_model_get_static_model(model)->translation;
}

rlr_quat_t rlr_obj_static_model_get_rotation(rlr_obj_static_model_handle_t model) {
    return rlr_pipeline_model_get_static_model(model)->rotation;
}

rlr_vec3_t rlr_obj_static_model_get_scale(rlr_obj_static_model_handle_t model) {
    return rlr_pipeline_model_get_static_model(model)->scale;
}

void rlr_obj_static_model_set_trs(rlr_obj_static_model_handle_t handle, const rlr_vec3_t* translation, const rlr_quat_t* rotation, const rlr_vec3_t* scale) {
    rlr_obj_static_model_t* model = rlr_pipeline_model_get_static_model(handle);
    if(translation) {
        model->translation = *translation;
    }
    if(rotation) {
        model->rotation = *rotation;
    }
    if(scale) {
        model->scale = *scale;
    }

    rlr_mat4x4_t trs = rlr_mat4x4_trs(&model->translation, &model->rotation, &model->scale);
    rlr_affine_mat4x3_t affine = rlr_mat4x4_to_affine_mat4x3(&trs);
    rlr_pipeline_model_update_static_model_instance(model->cmd_index, model->cmd_generation, model->instance_index, (rlr_pipeline_static_model_instance_t){.matrix = affine});
}

void rlr_obj_static_model_free(rlr_obj_static_model_handle_t handle) {
    rlr_pipeline_model_free_static_model(handle);
}