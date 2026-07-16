#include <stdlib.h>
#include "external/stb_ds.h"
#include "internal/impl.h"
#include "rlr/math/matrix.h"
#include "animated_model.h"

rlr_obj_t rlr_obj_animated_model_create(rlr_res_t model, rlr_vec3_t translation, rlr_quat_t rotation, rlr_vec3_t scale) {
    rlr_obj_t handle = rlr_mem_man_allocate_obj_animated_model(rlr_mem_man(), (rlr_obj_animated_model_t){0});
    if(handle == RLR_NULL) {
        goto err;
    }

    rlr_obj_animated_model_t* obj = rlr_mem_man_get_obj_animated_model(rlr_mem_man(), handle);
    if(!obj) {
        goto err;
    }
    
    rlr_pipeline_animated_model_draw_command_t* cmd = rlr_pipeline_model_find_animated_model_draw_command(model, RLR_NULL);
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
        .animation_states = {
            {.animation_index = -1, .animation_loop = false, .animation_speed = 1.0, .animation_time = 0.0 },
            {.animation_index = -1, .animation_loop = false, .animation_speed = 1.0, .animation_time = 0.0 },
        },
    };

    rlr_mat4x4_t trs = rlr_mat4x4_trs(&translation, &rotation, &scale);
    rlr_affine_mat4x3_t affine = rlr_mat4x4_to_affine_mat4x3(&trs);
    rlr_pipeline_animated_model_instance_t instance = {
        .matrix = affine,
        .lerp_primary = 0.0,
        .lerp_secondary = 0.0,
        .pose_a_offset_primary = 0,
        .pose_b_offset_primary = 0,
        .pose_a_offset_secondary = 0,
        .pose_b_offset_secondary = 0,
        .transition_lerp = 0.0,
    };
    obj->instance_index = rlr_pipeline_model_add_animated_model_instance(cmd, instance);
    return handle;
err:
    rlr_obj_animated_model_free(handle);
    return RLR_NULL;
}

rlr_vec3_t rlr_obj_animated_model_get_translation(rlr_obj_t handle) {
    rlr_obj_animated_model_t* model = rlr_mem_man_get_obj_animated_model(rlr_mem_man(), handle);
    return model->translation;
}

rlr_quat_t rlr_obj_animated_model_get_rotation(rlr_obj_t handle) {
    rlr_obj_animated_model_t* model = rlr_mem_man_get_obj_animated_model(rlr_mem_man(), handle);
    return model->rotation;
}

rlr_vec3_t rlr_obj_animated_model_get_scale(rlr_obj_t handle) {
    rlr_obj_animated_model_t* model = rlr_mem_man_get_obj_animated_model(rlr_mem_man(), handle);
    return model->scale;
}

int32_t rlr_obj_animated_model_get_current_animation(rlr_obj_t handle) {
    rlr_obj_animated_model_t* model = rlr_mem_man_get_obj_animated_model(rlr_mem_man(), handle);
    return model->animation_states[RLR_OBJ_ANIMATION_PRIMARY].animation_index;
}

bool rlr_obj_animated_model_is_animating(rlr_obj_t handle) {
    rlr_obj_animated_model_t* model = rlr_mem_man_get_obj_animated_model(rlr_mem_man(), handle);
    return model->animation_states[RLR_OBJ_ANIMATION_PRIMARY].animation_index != -1;
}

void rlr_obj_animated_model_set_animation(rlr_obj_t handle, int32_t animation_index, float speed, bool loop) {
    rlr_obj_animated_model_t* model = rlr_mem_man_get_obj_animated_model(rlr_mem_man(), handle);
    model->animation_states[RLR_OBJ_ANIMATION_PRIMARY].animation_index = animation_index;
    model->animation_states[RLR_OBJ_ANIMATION_PRIMARY].animation_time = 0.0;
    model->animation_states[RLR_OBJ_ANIMATION_PRIMARY].animation_speed = speed;
    model->animation_states[RLR_OBJ_ANIMATION_PRIMARY].animation_loop = loop;
    model->transition_time = 0.0;
}

void rlr_obj_animated_model_set_animation_blended(rlr_obj_t handle, int32_t animation_index, float speed, float transition_time, bool loop) {
    rlr_obj_animated_model_t* model = rlr_mem_man_get_obj_animated_model(rlr_mem_man(), handle);
    model->transition_time = transition_time;
    model->current_transition_time = 0.0;
    model->animation_states[RLR_OBJ_ANIMATION_SECONDARY] = model->animation_states[RLR_OBJ_ANIMATION_PRIMARY];
    model->animation_states[RLR_OBJ_ANIMATION_PRIMARY].animation_index = animation_index;
    model->animation_states[RLR_OBJ_ANIMATION_PRIMARY].animation_time = 0.0;
    model->animation_states[RLR_OBJ_ANIMATION_PRIMARY].animation_speed = speed;
    model->animation_states[RLR_OBJ_ANIMATION_PRIMARY].animation_loop = loop;
    rlr_pipeline_model_swap_animation_states(model->cmd_index, model->cmd_generation, model->instance_index);
}

void rlr_obj_animated_model_set_animation_speed(rlr_obj_t handle, float speed) {
    rlr_obj_animated_model_t* model = rlr_mem_man_get_obj_animated_model(rlr_mem_man(), handle);
    model->animation_states[RLR_OBJ_ANIMATION_PRIMARY].animation_speed = speed;
}

void rlr_obj_animated_model_set_trs(rlr_obj_t handle, const rlr_vec3_t* translation, const rlr_quat_t* rotation, const rlr_vec3_t* scale) {
    rlr_obj_animated_model_t* model = rlr_mem_man_get_obj_animated_model(rlr_mem_man(), handle);
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

void rlr_obj_animated_model_free(rlr_obj_t obj) {
    rlr_mem_man_free_obj_animated_model(rlr_mem_man(), obj);
}