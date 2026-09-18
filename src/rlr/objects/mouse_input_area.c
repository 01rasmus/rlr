#include "../../internal/impl.h"
#include "mouse_input_area.h"

rlr_obj_mouse_input_area_t* rlr_obj_mouse_input_area_create(rlr_rect_t rect, int32_t layer, void* user, rlr_anchor_t local_anchor, rlr_anchor_t screen_anchor, rlr_input_area_on_enter_t on_enter, rlr_input_area_on_leave_t on_leave, rlr_input_area_on_pressed_t on_pressed, rlr_input_area_on_release_t on_release) {
    rlr_obj_mouse_input_area_t* obj = malloc(sizeof(rlr_obj_mouse_input_area_t));
    if(!obj) {
        goto err;
    }
    
    obj->id = rlr_pipeline_input_add_area((rlr_pipeline_input_mouse_area_t){
        .rect = rect,
        .layer = layer,
        .user = user,
        .enabled = true,
        .on_enter = on_enter,
        .on_leave = on_leave,
        .on_pressed = on_pressed,
        .on_release = on_release,
        .local_anchor = local_anchor,
        .screen_anchor = screen_anchor
    });
    if(obj->id == RLR_NULL) {
        goto err;
    }

    return obj;
err:
    rlr_obj_mouse_input_area_free(obj);
    return NULL;
}

void rlr_obj_mouse_input_area_set_enabled(rlr_obj_mouse_input_area_t* obj, bool enabled) {
    rlpp_get_unchecked(rlr()->pipeline_input.mouse_areas, obj->id)->enabled = enabled;
}

void rlr_obj_mouse_input_area_free(rlr_obj_mouse_input_area_t* obj) {
    if(!obj) {
        return;
    }
    rlr_pipeline_input_remove_area(obj->id);
    free(obj);
}