#include <stdlib.h>
#include "../../internal/core/res_types.h"
#include "../../internal/impl.h"
#include "../rlr.h"
#include "uniform.h"

rlr_ubo_t* rlr_ubo_new(const void* initial_data, size_t data_len, bool is_static) {
    rlr_ubo_t* uniform = NULL;

    uniform = malloc(sizeof(rlr_ubo_t));
    if(!uniform) {
        rlr_log_error("pointer to the uniform handle is null");
        goto err;
    }

    uniform->is_static = is_static;
    uniform->len = data_len;
    uniform->buffer = rlr_backend()->create_buffer();
    if(uniform->buffer == 0) {
        rlr_log_error("failed to create uniform buffer");
        goto err;
    }

    if(uniform->is_static) {
        if(initial_data == NULL) {
            rlr_log_error("a static uniform cannot be initialized with null data");
            goto err;
        }

        rlr_backend()->bind_buffer(uniform->buffer, RLR_BACKEND_BUFFER_UNIFORM);
        rlr_backend()->update_buffer(RLR_BACKEND_BUFFER_UNIFORM, uniform->len, initial_data, RLR_BACKEND_BUFFER_USAGE_STATIC);
    } else if(initial_data) {
        rlr_backend()->bind_buffer(uniform->buffer, RLR_BACKEND_BUFFER_UNIFORM);
        rlr_backend()->update_buffer(RLR_BACKEND_BUFFER_UNIFORM, uniform->len, initial_data, RLR_BACKEND_BUFFER_USAGE_DYNAMIC);
    }
    return uniform;
err:
    rlr_ubo_free(uniform);
    return NULL;
}

void rlr_ubo_update(rlr_ubo_t* ubo, size_t offset, const void* data, size_t size) {
    if(ubo->is_static) {
        return;
    }
    rlr_backend()->bind_buffer(ubo->buffer, RLR_BACKEND_BUFFER_UNIFORM);
    rlr_backend()->update_buffer_orphaned(RLR_BACKEND_BUFFER_UNIFORM, size, data);
}

void rlr_ubo_bind(const rlr_ubo_t* ubo, uint8_t ubo_slot) {
    rlr_backend()->bind_uniform_buffer(ubo->buffer, ubo_slot);
}

void rlr_ubo_free(rlr_ubo_t* ubo) {
    if(!ubo) {
        return;
    }
    rlr_backend()->free_buffer(ubo->buffer);
    free(ubo);
}