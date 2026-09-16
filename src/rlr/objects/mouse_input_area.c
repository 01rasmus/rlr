#include "../../internal/impl.h"
#include "mouse_input_area.h"

rlr_obj_t rlr_obj_mouse_input_area_create(rlr_rect_t rect, int32_t layer, void* user, rlr_anchor_t local_anchor, rlr_anchor_t screen_anchor, rlr_input_area_on_enter_t on_enter, rlr_input_area_on_leave_t on_leave, rlr_input_area_on_pressed_t on_pressed, rlr_input_area_on_release_t on_release) {
    rlr_obj_t id = rlr_mem_man_allocate_obj_mouse_input_area(rlr_mem_man(), (rlr_obj_mouse_input_area_t){0});
    if(id == RLR_NULL) {
        goto err;
    }
    rlr_obj_mouse_input_area_t* obj = rlr_mem_man_get_obj_mouse_input_area(rlr_mem_man(), id);
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

    return id;
err:
    rlr_obj_mouse_input_area_free(id);
    return RLR_NULL;
}

void rlr_obj_mouse_input_area_set_enabled(rlr_obj_t id, bool enabled) {
    rlr_obj_mouse_input_area_t* mi = rlr_mem_man_get_obj_mouse_input_area(rlr_mem_man(), id);
    rlpp_get_unchecked(rlr()->pipeline_input.mouse_areas, mi->id)->enabled = enabled;
}

void rlr_obj_mouse_input_area_free(rlr_obj_t id) {
    if(id == RLR_NULL) {
        return;
    }
    rlr_obj_mouse_input_area_t* mi = rlr_mem_man_get_obj_mouse_input_area(rlr_mem_man(), id);
    if(mi) {
        return;
    }
    rlr_pipeline_input_remove_area(mi->id);
    rlr_mem_man_free_obj_mouse_input_area(rlr_mem_man(), id);
}