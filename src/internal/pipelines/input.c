#include "input.h"
#include "../impl.h"

#define RLR_PIPELINE_INPUT    (&rlr()->pipeline_input)

/* default input fucntions */
static void default_area_on_enter(void* user) {}
static void default_area_on_leave(void* user) {}
static void default_area_on_pressed(void* user) {}
static void default_area_on_release(void* user) {}

bool rlr_pipeline_input_init() {
    rlr_pipeline_input_t* pi = RLR_PIPELINE_INPUT;
    pi->mouse_areas = NULL;
    pi->current_hovered = RLR_NULL;
    pi->current_pressed = RLR_NULL;
    return true;
}

static inline rlr_rect_t make_anchored_rect(const rlr_rect_t* original, rlr_vec2_t screen_size, rlr_anchor_t local_anchor, rlr_anchor_t screen_anchor) {
    rlr_vec2_t screen_anchor_vec = rlr_vec2_mul(rlr_anchor_vec(screen_anchor), screen_size);
    rlr_vec2_t rect_anchor_vec = rlr_vec2_mul(rlr_anchor_vec(local_anchor), rlr_vec2(original->width, original->height));
    return (rlr_rect_t) {
        .x = original->x + screen_anchor_vec.x - rect_anchor_vec.x,
        .y = original->y + screen_anchor_vec.y - rect_anchor_vec.y,
        .width = original->width,
        .height = original->height
    };
}

static inline void rlr_pipeline_input_update_mouse_areas(rlr_pipeline_input_t* pi, rlr_vec2_t mouse_position, rlr_vec2_t screen_size) {
    rlr_pipeline_input_mouse_area_t* best_area = NULL;
    for(uint64_t i = 0; i < rlpp_len(pi->mouse_areas); i++) {
        rlr_pipeline_input_mouse_area_t* area = &pi->mouse_areas[i];
        
        if(!area->enabled) {
            continue;
        }

        rlr_rect_t anchored_rect = make_anchored_rect(&area->rect, screen_size, area->local_anchor, area->screen_anchor);
        if(rlr_rect_contains_position(&anchored_rect, mouse_position)) {

            //only pick the area with the highest layer if multiple overlap
            if(best_area && best_area->layer > area->layer) {
                continue;
            }

            best_area = area;
        }
    }

    uint64_t best_id = best_area == NULL ? RLR_NULL : best_area->id;
    if(best_id != pi->current_hovered) {

        //leave the old one
        
        rlr_pipeline_input_mouse_area_t* prev_area = rlpp_get(pi->mouse_areas, pi->current_hovered);
        if(prev_area) {
            prev_area->on_leave(prev_area->user);
        }

        //enter the new area
        if(best_area != NULL) {
            best_area->on_enter(best_area->user);
        }

        //set the new area
        pi->current_hovered = best_id;
    }
}

void rlr_pipeline_input_update(rlr_vec2_t mouse_position, rlr_vec2_t screen_size) {
    rlr_pipeline_input_t* pi = RLR_PIPELINE_INPUT;
    rlr_pipeline_input_update_mouse_areas(pi, mouse_position, screen_size);
}

void rlr_pipeline_input_deinit() {
    rlr_pipeline_input_t* pi = RLR_PIPELINE_INPUT;
    rlpp_free(pi->mouse_areas);
}

rlpp_id_t rlr_pipeline_input_add_area(rlr_pipeline_input_mouse_area_t input_area) {
    rlr_pipeline_input_t* pi = RLR_PIPELINE_INPUT;
    rlpp_id_t id = rlpp_alloc(pi->mouse_areas, (rlr_pipeline_input_mouse_area_t){0});
    rlr_pipeline_input_mouse_area_t* area = rlpp_get_unchecked(pi->mouse_areas, id);

    (*area) = (rlr_pipeline_input_mouse_area_t){
        .id = id,
        .user = input_area.user,
        .rect = input_area.rect,
        .enabled = input_area.enabled,
        .layer = input_area.layer,
        .local_anchor = input_area.local_anchor,
        .screen_anchor = input_area.screen_anchor,
        .on_enter = input_area.on_enter == NULL ? default_area_on_enter : input_area.on_enter,
        .on_leave = input_area.on_leave == NULL ? default_area_on_leave : input_area.on_leave,
        .on_pressed = input_area.on_pressed == NULL ? default_area_on_pressed : input_area.on_pressed,
        .on_release = input_area.on_release == NULL ? default_area_on_release : input_area.on_release,
    };
    return id;
}

void rlr_pipeline_input_remove_area(uint64_t id) {
    rlr_pipeline_input_t* pi = RLR_PIPELINE_INPUT;
    rlpp_remove(pi->mouse_areas, id);
}