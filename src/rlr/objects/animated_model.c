#include <stdlib.h>
#include "external/rlr_stb_ds.h"
#include "internal/pipelines/model.h"
#include "rlr/math/matrix.h"
#include "animated_model.h"

rlr_obj_animated_model_handle_t rlr_obj_animated_model_create(rlr_res_animated_model_t* model, rlr_vec3_t translation, rlr_quat_t rotation, rlr_vec3_t scale) {
    rlr_obj_animated_model_handle_t handle = rlr_pipeline_model_alloc_animated_model();
    rlr_obj_animated_model_t* obj = rlr_pipeline_model_get_animated_model(handle);
    if(!obj) {
        goto err;
    }
    
    rlr_pipeline_animated_model_draw_command_t* cmd = rlr_pipeline_model_find_animated_model_draw_command(model, NULL);
    if(!cmd) {
        goto err;
    }

    (*obj) = (rlr_obj_animated_model_t){
        .translation = translation,
        .rotation = rotation,
        .scale = scale,
        .opaque = true,
        .cmd_index = cmd->index,
        .cmd_generation = cmd->generation,
        .instance_index = 0,
        .animation_speed = 1.0,
        .animation_time = 0.0,
        .current_animation_index = -1,
    };

    rlr_mat4x4_t trs = rlr_mat4x4_trs(&translation, &rotation, &scale);
    rlr_affine_mat4x3_t affine = rlr_mat4x4_to_affine_mat4x3(&trs);
    rlr_pipeline_animated_model_instance_t instance = {
        .matrix = affine,
        .pose_a_offset = 0,
        .pose_b_offset = 0,
        .lerp = 0.0,
    };
    obj->instance_index = rlr_pipeline_model_add_animated_model_instance(cmd, instance);
    return handle;
err:
    rlr_obj_animated_model_free(handle);
    return RLR_SPARSE_GEN_ALLOCATOR_NULL_HANDLE;
}

rlr_vec3_t rlr_obj_animated_model_get_translation(rlr_obj_animated_model_handle_t handle) {
    rlr_obj_animated_model_t* model = rlr_pipeline_model_get_animated_model(handle);
    return model->translation;
}

rlr_quat_t rlr_obj_animated_model_get_rotation(rlr_obj_animated_model_handle_t handle) {
    rlr_obj_animated_model_t* model = rlr_pipeline_model_get_animated_model(handle);
    return model->rotation;
}

rlr_vec3_t rlr_obj_animated_model_get_scale(rlr_obj_animated_model_handle_t handle) {
    rlr_obj_animated_model_t* model = rlr_pipeline_model_get_animated_model(handle);
    return model->scale;
}

int32_t rlr_obj_animated_model_get_current_animation(rlr_obj_animated_model_handle_t handle) {
    rlr_obj_animated_model_t* model = rlr_pipeline_model_get_animated_model(handle);
    return model->current_animation_index;
}

void rlr_obj_animated_model_set_animation(rlr_obj_animated_model_handle_t handle, int32_t animation_index) {
    rlr_obj_animated_model_t* model = rlr_pipeline_model_get_animated_model(handle);
    model->current_animation_index = animation_index;
}

void rlr_obj_animated_model_set_animation_speed(rlr_obj_animated_model_handle_t handle, float speed) {
    rlr_obj_animated_model_t* model = rlr_pipeline_model_get_animated_model(handle);
    model->animation_speed = speed;
}

void rlr_obj_animated_model_set_trs(rlr_obj_animated_model_handle_t handle, const rlr_vec3_t* translation, const rlr_quat_t* rotation, const rlr_vec3_t* scale) {
    rlr_obj_animated_model_t* model = rlr_pipeline_model_get_animated_model(handle);
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
    rlr_pipeline_model_update_animated_model_instance(model->cmd_index, model->cmd_generation, model->instance_index, (rlr_pipeline_animated_model_instance_t){.matrix = affine});
}

void rlr_obj_animated_model_free(rlr_obj_animated_model_handle_t obj) {
    rlr_pipeline_model_free_animated_model(obj);
}

#include <stdio.h>
void rlr_obj_animated_model_update_animation(rlr_obj_animated_model_handle_t obj, float time) {
    rlr_obj_animated_model_t* model = rlr_pipeline_model_get_animated_model(obj);
    rlr_mat4x4_t trs = rlr_mat4x4_trs(&model->translation, &model->rotation, &model->scale);
    rlr_affine_mat4x3_t affine = rlr_mat4x4_to_affine_mat4x3(&trs);

    while(time >= 1.158333) {
        time -= 1.158333;
    }

    float fps = 30.0;
    float frame_interval = 1.0 / fps;
    float joints_per_pose = 24;
    float start_pose = 3072;

    float current_pose = time / frame_interval;
    uint32_t current_pose_upper = start_pose + (ceilf(current_pose)) * joints_per_pose;
    uint32_t current_pose_below = start_pose + (floorf(current_pose)) * joints_per_pose;
    float alpha = current_pose - floorf(current_pose);

    //printf("Pose data:\n\ttime=%f\n\tpose a=%d\n\tpose b=%d\n\tlerp=%f\n\n", time, current_pose_below, current_pose_upper, alpha);

    rlr_pipeline_model_update_animated_model_instance(model->cmd_index, model->cmd_generation, model->instance_index, (rlr_pipeline_animated_model_instance_t){.matrix = affine, .lerp = alpha, .pose_a_offset = current_pose_below, .pose_b_offset = current_pose_upper });
}