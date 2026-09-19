#include <stdlib.h>
#include "../../internal/core/obj_types.h"
#include "../../internal/core/res_types.h"
#include "../../external/stb_ds.h"
#include "../../internal/impl.h"
#include "../math/matrix.h"
#include "static_model.h"

rlr_obj_static_model_t* rlr_obj_static_model_create(rlr_res_static_model_t* model, rlr_vec3_t translation, rlr_quat_t rotation, rlr_vec3_t scale) {
    rlr_obj_static_model_t* obj = malloc(sizeof(rlr_obj_static_model_t));
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
        .cmd_index = cmd->index,
        .cmd_generation = cmd->generation,
        .instance_index = 0,
    };
    rlr_mat4x4_t trs = rlr_mat4x4_trs(&translation, &rotation, &scale);
    rlr_affine_mat4x3_t affine = rlr_mat4x4_to_affine_mat4x3(&trs);
    rlr_pipeline_static_model_instance_t instance = {.matrix = affine};
    obj->instance_index = rlr_pipeline_model_add_static_model_instance(cmd, instance);
    return obj;
err:
    rlr_obj_static_model_free(obj);
    return NULL;
}

rlr_vec3_t rlr_obj_static_model_get_translation(const rlr_obj_static_model_t* model) {
    return model->translation;
}

rlr_quat_t rlr_obj_static_model_get_rotation(const rlr_obj_static_model_t* model) {
    return model->rotation;
}

rlr_vec3_t rlr_obj_static_model_get_scale(const rlr_obj_static_model_t* model) {
    return model->scale;
}

void rlr_obj_static_model_set_trs(rlr_obj_static_model_t* model, const rlr_vec3_t* translation, const rlr_quat_t* rotation, const rlr_vec3_t* scale) {
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

void rlr_obj_static_model_free(rlr_obj_static_model_t* model) {
    if(!model) {
        return;
    }
    free(model);
}