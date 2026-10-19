#include "../../internal/impl.h"
#include "mouse_input_area.h"

rlr_ms_inpt_area_t* rlr_ms_inpt_area_create_hov_click(rlr_rect_t rect, void* user, rlr_input_area_on_enter_t on_enter, rlr_input_area_on_leave_t on_leave, rlr_input_area_on_pressed_t on_pressed, rlr_input_area_on_release_t on_release, uint32_t layer) {
    rlr_ms_inpt_area_t* obj = malloc(sizeof(rlr_ms_inpt_area_t));
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
        .on_release = on_release
    });
    if(obj->id == RLR_NULL) {
        goto err;
    }

    return obj;
err:
    rlr_ms_inpt_area_free(obj);
    return NULL;
}

rlr_ms_inpt_area_t* rlr_ms_inpt_area_create_hov(rlr_rect_t rect, void* user, rlr_input_area_on_enter_t on_enter, rlr_input_area_on_leave_t on_leave, uint32_t layer) {
    return rlr_ms_inpt_area_create_hov_click(rect, user, on_enter, on_leave, NULL, NULL, layer);
}

rlr_ms_inpt_area_t* rlr_ms_inpt_area_create_click(rlr_rect_t rect, void* user, rlr_input_area_on_pressed_t on_pressed, rlr_input_area_on_release_t on_release, uint32_t layer) {
    return rlr_ms_inpt_area_create_hov_click(rect, user, NULL, NULL, on_pressed, on_release, layer);
}

void rlr_ms_inpt_area_set_enabled(rlr_ms_inpt_area_t* obj, bool enabled) {
    rlpp_get_unchecked(rlr()->pipeline_input.mouse_areas, obj->id)->enabled = enabled;
}

void rlr_ms_inpt_area_free(rlr_ms_inpt_area_t* obj) {
    if(!obj) {
        return;
    }
    rlr_pipeline_input_remove_area(obj->id);
    free(obj);
}