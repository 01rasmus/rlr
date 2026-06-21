#include <stddef.h>
#include "pipelines/stencil.h"
#include "model_occluder.h"

rlr_obj_model_occluder_t* rlr_obj_model_occluder_create(rlr_rect_t rectangle, rlr_anchor_t screen_anchor, rlr_anchor_t local_anchor) {
    rlr_obj_model_occluder_t* mo = rlr_pipeline_stencil_alloc_model_occluder();
    mo->screen_anchor = screen_anchor;
    mo->local_anchor = local_anchor;
    mo->rectangle = rectangle;
    return mo;
err:
    rlr_obj_model_occluder_free(mo);
    return NULL;
}

void rlr_obj_model_occluder_free(rlr_obj_model_occluder_t* mo) {
    if(!mo) {
        return;
    }
    rlr_pipeline_stencil_free_model_occluder(mo);
}