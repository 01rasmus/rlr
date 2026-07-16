#include <stddef.h>
#include "../../internal/impl.h"
#include "model_occluder.h"

rlr_obj_t rlr_obj_model_occluder_create(rlr_rect_t rectangle, rlr_anchor_t screen_anchor, rlr_anchor_t local_anchor) {
    rlr_obj_t id = rlr_mem_man_allocate_obj_model_occluder(rlr_mem_man(), (rlr_obj_model_occluder_t){ .screen_anchor = screen_anchor, .local_anchor = local_anchor, .rectangle = rectangle });
    if(id == RLR_NULL) {
        goto err;
    }
    return id;
err:
    rlr_obj_model_occluder_free(id);
    return RLR_NULL;
}

void rlr_obj_model_occluder_free(rlr_obj_t obj) {
    if(!obj) {
        return;
    }
    rlr_mem_man_free_obj_model_occluder(rlr_mem_man(), obj);
}