#include <stdlib.h>
#include "../../internal/impl.h"
#include "../error.h"
#include "../rlr.h"
#include "uniform.h"

rlr_res_t rlr_res_uniform_create_dynamic(uint64_t size) {
    rlr_res_t id = RLR_NULL;
    rlr_res_uniform_t* uniform = NULL;

    id = rlr_mem_man_allocate_res_uniform(rlr_mem_man(), (rlr_res_uniform_t){0});
    if(id == RLR_NULL) {
        rlr_error_set(RLR_ERR_NO_MEMORY);
        goto err;
    }

    uniform = rlr_mem_man_get_res_uniform(rlr_mem_man(), id);
    if(!uniform) {
        rlr_error_set(RLR_ERR_NO_MEMORY);
        goto err;
    }
    uniform->is_static = false;
    uniform->max_size = size;
    uniform->buffer = rlr_backend()->create_buffer();
    rlr_backend()->bind_buffer(uniform->buffer, RLR_BACKEND_BUFFER_UNIFORM);
    rlr_backend()->update_buffer(RLR_BACKEND_BUFFER_UNIFORM, size, NULL, RLR_BACKEND_BUFFER_USAGE_DYNAMIC);
    return id;
err:
    rlr_res_uniform_free(id);
    return RLR_NULL;
}

rlr_res_t rlr_res_uniform_create_static(void* data, uint64_t size) {
    rlr_res_t id = RLR_NULL;
    rlr_res_uniform_t* uniform = NULL;

    id = rlr_mem_man_allocate_res_uniform(rlr_mem_man(), (rlr_res_uniform_t){0});
    if(id == RLR_NULL) {
        rlr_error_set(RLR_ERR_NO_MEMORY);
        goto err;
    }

    uniform = rlr_mem_man_get_res_uniform(rlr_mem_man(), id);
    if(!uniform) {
        rlr_error_set(RLR_ERR_NO_MEMORY);
        goto err;
    }
    uniform->is_static = true;
    uniform->max_size = size;
    uniform->buffer = rlr_backend()->create_buffer();
    rlr_backend()->bind_buffer(uniform->buffer, RLR_BACKEND_BUFFER_UNIFORM);
    rlr_backend()->update_buffer(RLR_BACKEND_BUFFER_UNIFORM, size, data, RLR_BACKEND_BUFFER_USAGE_STATIC);
    return id;
err:
    rlr_res_uniform_free(id);
    return RLR_NULL;
}

void rlr_res_uniform_update(rlr_res_t ubo_id, uint64_t offset, void* data, uint64_t size) {
    rlr_res_uniform_t* ubo = rlr_mem_man_get_res_uniform(rlr_mem_man(), ubo_id);
    if(ubo->is_static) {
        return;
    }
    rlr_backend()->bind_buffer(ubo->buffer, RLR_BACKEND_BUFFER_UNIFORM);
    rlr_backend()->update_buffer(RLR_BACKEND_BUFFER_UNIFORM, size, data, RLR_BACKEND_BUFFER_USAGE_DYNAMIC);
}

void rlr_res_uniform_bind(rlr_res_t ubo_id, uint8_t ubo_slot) {
    rlr_res_uniform_t* ubo = rlr_mem_man_get_res_uniform(rlr_mem_man(), ubo_id);
    rlr_backend()->bind_uniform_buffer(ubo->buffer, ubo_slot);
}

void rlr_res_uniform_free(rlr_res_t id) {
    if(id == RLR_NULL) {
        return;
    }

    rlr_res_uniform_t* ubo = rlr_mem_man_get_res_uniform(rlr_mem_man(), id);
    if(!ubo) {
        return;
    }
    rlr_backend()->free_buffer(ubo->buffer);
    rlr_mem_man_free_res_uniform(rlr_mem_man(), id);
}