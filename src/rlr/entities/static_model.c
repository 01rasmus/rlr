#include <stdlib.h>
#include "../../internal/core/entity_types.h"
#include "../../internal/core/res_types.h"
#include "../../external/stb_ds.h"
#include "../../internal/impl.h"
#include "../math/matrix.h"
#include "static_model.h"

rlr_sta_obj_t* rlr_sta_obj_new(rlr_sta_mdl_t* model, rlr_vec3_t translation, rlr_quat_t rotation, rlr_vec3_t scale) {
    rlr_sta_obj_t* obj = malloc(sizeof(rlr_sta_obj_t));
    if(!obj) {
        goto err;
    }

    rlr_pipeline_static_model_draw_command_t* cmd = rlr_pipeline_model_find_static_model_draw_command(model, NULL);
    if(!cmd) {
        goto err;
    }

    (*obj) = (rlr_sta_obj_t){
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
    rlr_sta_obj_free(obj);
    return NULL;
}

rlr_vec3_t rlr_sta_obj_get_trans(const rlr_sta_obj_t* model) {
    return model->translation;
}

rlr_quat_t rlr_sta_obj_get_rot(const rlr_sta_obj_t* model) {
    return model->rotation;
}

rlr_vec3_t rlr_sta_obj_get_scale(const rlr_sta_obj_t* model) {
    return model->scale;
}

void rlr_sta_obj_set_trs(rlr_sta_obj_t* model, const rlr_vec3_t* translation, const rlr_quat_t* rotation, const rlr_vec3_t* scale) {
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

void rlr_sta_obj_free(rlr_sta_obj_t* model) {
    if(!model) {
        return;
    }
    free(model);
}