#include <stdlib.h>
#include "internal/pipelines/model.h"
#include "rlr/math/matrix.h"
#include "static_model.h"

rlr_obj_static_model_t* rlr_obj_static_model_create(rlr_res_static_model_t* model, rlr_vec3_t translation, rlr_quat_t rotation, rlr_vec3_t scale) {
    rlr_obj_static_model_t* obj = rlr_pipeline_model_alloc_static_model();
    if(!obj) {
        goto err;
    }

    (*obj) = (rlr_obj_static_model_t){
        .model = model,
        .shader = NULL, //uses the default one
        .translation = translation,
        .rotation = rotation,
        .scale = scale,
        .matrix = rlr_mat4_trs(&translation, &rotation, &scale),
        .opaque = true,
        .alpha = 1.0,
    };

err:
    rlr_obj_static_model_free(obj);
    return NULL;
}

void rlr_obj_static_model_free(rlr_obj_static_model_t* obj) {
    if(!obj) {
        return;
    }
    rlr_pipeline_model_free_static_model(obj);
}