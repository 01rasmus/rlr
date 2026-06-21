#include <stdlib.h>
#include "error.h"
#include "uniform.h"
#include "rlr.h"

rlr_res_uniform_t* rlr_res_uniform_create_dynamic(uint64_t size) {
    rlr_res_uniform_t* uniform = malloc(sizeof(rlr_res_uniform_t));
    if(!uniform) {
        rlr_error_set(RLR_ERR_NO_MEMORY);
        goto err;
    }
    (*uniform) = (rlr_res_uniform_t){0};
    uniform->is_static = false;
    uniform->max_size = size;
    uniform->buffer = rlr_backend()->buffer_create();
    rlr_backend()->buffer_bind(uniform->buffer, RLR_BACKEND_BUFFER_UNIFORM);
    rlr_backend()->buffer_update(RLR_BACKEND_BUFFER_UNIFORM, size, NULL, RLR_BACKEND_BUFFER_USAGE_DYNAMIC);
    return uniform;
err:
    rlr_res_uniform_free(uniform);
    return NULL;
}

rlr_res_uniform_t* rlr_res_uniform_create_static(void* data, uint64_t size) {
    rlr_res_uniform_t* uniform = malloc(sizeof(rlr_res_uniform_t));
    if(!uniform) {
        rlr_error_set(RLR_ERR_NO_MEMORY);
        goto err;
    }
    (*uniform) = (rlr_res_uniform_t){0};
    uniform->is_static = true;
    uniform->max_size = size;
    uniform->buffer = rlr_backend()->buffer_create();
    rlr_backend()->buffer_bind(uniform->buffer, RLR_BACKEND_BUFFER_UNIFORM);
    rlr_backend()->buffer_update(RLR_BACKEND_BUFFER_UNIFORM, size, data, RLR_BACKEND_BUFFER_USAGE_STATIC);
    return uniform;
err:
    rlr_res_uniform_free(uniform);
    return NULL;
}

void rlr_res_uniform_update(rlr_res_uniform_t* ubo, uint64_t offset, void* data, uint64_t size) {
    if(ubo->is_static) {
        return;
    }
    rlr_backend()->buffer_bind(ubo->buffer, RLR_BACKEND_BUFFER_UNIFORM);
    rlr_backend()->buffer_update(RLR_BACKEND_BUFFER_UNIFORM, size, data, RLR_BACKEND_BUFFER_USAGE_DYNAMIC);
}

void rlr_res_uniform_bind(rlr_res_uniform_t* ubo, uint8_t ubo_slot) {
    rlr_backend()->uniform_buffer_bind(ubo->buffer, ubo_slot);
}

void rlr_res_uniform_free(rlr_res_uniform_t* ubo) {
    if(!ubo) {
        return;
    }
    rlr_backend()->buffer_free(ubo->buffer);
    free(ubo);
}