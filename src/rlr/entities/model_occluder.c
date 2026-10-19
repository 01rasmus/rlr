#include <stddef.h>
#include "../../internal/impl.h"
#include "model_occluder.h"

static void make_instance(rlr_pipeline_stencil_occluder_instance_t* instance, rlr_mdl_occl_t* mo) {
    rlr_vec2_t screen_anchor_vec = rlr_anchor_vec(mo->rect.screen_anchor);
    rlr_vec2_t local_anchor_vec = rlr_anchor_vec(mo->rect.local_anchor);
    float width = mo->rect.width;
    float height = mo->rect.height;

    instance->pos = rlr_vec2(mo->rect.x - (width * local_anchor_vec.x), mo->rect.y - (height * local_anchor_vec.y));
    instance->screen_anchor = screen_anchor_vec;
    instance->size = rlr_vec2(width, height);
    instance->visible = mo->visible == true ? 1 : 0;
}

rlr_mdl_occl_t* rlr_mdl_occl_create(rlr_rect_t rect) {
    rlr_mdl_occl_t* occluder = malloc(sizeof(rlr_mdl_occl_t));
    if(!occluder) {
        goto err;
    }

    occluder->rect = rect;
    occluder->visible = true;

    rlr_pipeline_stencil_occluder_instance_t inst = {0};
    make_instance(&inst, occluder);

    if(!rlr_pipeline_stencil_add_model_occluder_instance(inst, &occluder->instance_ref)) {
        goto err;
    }
    
    return occluder;
err:
    rlr_mdl_occl_free(occluder);
    return NULL;
}

void rlr_mdl_occl_set_vis(rlr_mdl_occl_t* mo, bool visible) {
    mo->visible = visible;
    rlr_pipeline_stencil_set_model_occluder_instance_visability(mo->instance_ref, visible);
}

void rlr_mdl_occl_set_rect(rlr_mdl_occl_t* mo, rlr_rect_t rect) {
    mo->rect = rect;
    make_instance(rlr_pipeline_stencil_get_and_dirty_model_occluder_instance(mo->instance_ref), mo);
}

void rlr_mdl_occl_free(rlr_mdl_occl_t* mo) {
    if(!mo) {
        return;
    }
    rlr_pipeline_stencil_remove_model_occluder_instance(mo->instance_ref);
    free(mo);
}