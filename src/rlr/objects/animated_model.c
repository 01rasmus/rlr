#include <stdlib.h>
#include "../../external/stb_ds.h"
#include "../../rlr/math/matrix.h"
#include "../../internal/impl.h"
#include "../../internal/core/obj_types.h"
#include "animated_model.h"

rlr_obj_animated_model_t* rlr_obj_animated_model_create(rlr_res_animated_model_t* model, rlr_vec3_t translation, rlr_quat_t rotation, rlr_vec3_t scale) {
    rlr_obj_animated_model_t* obj = malloc(sizeof(rlr_obj_animated_model_t));
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
        .instance_index = 0
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
    if(!rlpp_pipeline_model_set_animated_model_animation_state(obj)) {
        goto err;
    }
    return obj;
err:
    rlr_obj_animated_model_free(obj);
    return NULL;
}

rlr_vec3_t rlr_obj_animated_model_get_translation(const rlr_obj_animated_model_t* model) {
    return model->translation;
}

rlr_quat_t rlr_obj_animated_model_get_rotation(const rlr_obj_animated_model_t* model) {
    return model->rotation;
}

rlr_vec3_t rlr_obj_animated_model_get_scale(const rlr_obj_animated_model_t* model) {
    return model->scale;
}

int32_t rlr_obj_animated_model_get_current_animation(rlr_obj_animated_model_t* model) {
    rlr_pipeline_animated_model_animation_state_t* state = rlr_pipeline_model_get_animation_state(model);
    return state->animation_states[RLR_OBJ_ANIMATION_PRIMARY].animation_index;
}

bool rlr_obj_animated_model_is_animating(rlr_obj_animated_model_t* model) {
    rlr_pipeline_animated_model_animation_state_t* state = rlr_pipeline_model_get_animation_state(model);
    return state->animation_states[RLR_OBJ_ANIMATION_PRIMARY].animation_index != -1;
}

void rlr_obj_animated_model_set_animation(rlr_obj_animated_model_t* model, int32_t animation_index, float speed, bool loop) {
    rlr_pipeline_animated_model_animation_state_t* state = rlr_pipeline_model_get_animation_state(model);
    state->animation_states[RLR_OBJ_ANIMATION_PRIMARY].animation_index = animation_index;
    state->animation_states[RLR_OBJ_ANIMATION_PRIMARY].animation_time = 0.0;
    state->animation_states[RLR_OBJ_ANIMATION_PRIMARY].animation_speed = speed;
    state->animation_states[RLR_OBJ_ANIMATION_PRIMARY].animation_loop = loop;
    state->transition_time = 0.0;
}

void rlr_obj_animated_model_set_animation_blended(rlr_obj_animated_model_t* model, int32_t animation_index, float speed, float transition_time, bool loop) {
    rlr_pipeline_animated_model_animation_state_t* state = rlr_pipeline_model_get_animation_state(model);
    state->transition_time = transition_time;
    state->current_transition_time = 0.0;
    state->animation_states[RLR_OBJ_ANIMATION_SECONDARY] = state->animation_states[RLR_OBJ_ANIMATION_PRIMARY];
    state->animation_states[RLR_OBJ_ANIMATION_PRIMARY].animation_index = animation_index;
    state->animation_states[RLR_OBJ_ANIMATION_PRIMARY].animation_time = 0.0;
    state->animation_states[RLR_OBJ_ANIMATION_PRIMARY].animation_speed = speed;
    state->animation_states[RLR_OBJ_ANIMATION_PRIMARY].animation_loop = loop;
    rlr_pipeline_model_swap_animation_states(model->cmd_index, model->cmd_generation, model->instance_index);
}

void rlr_obj_animated_model_set_animation_speed(rlr_obj_animated_model_t* model, float speed) {
    rlr_pipeline_animated_model_animation_state_t* state = rlr_pipeline_model_get_animation_state(model);
    state->animation_states[RLR_OBJ_ANIMATION_PRIMARY].animation_speed = speed;
}

void rlr_obj_animated_model_set_trs(rlr_obj_animated_model_t* model, const rlr_vec3_t* translation, const rlr_quat_t* rotation, const rlr_vec3_t* scale) {
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

void rlr_obj_animated_model_free(rlr_obj_animated_model_t* model) {
    if(!model) {
        return;
    }
    free(model);
}