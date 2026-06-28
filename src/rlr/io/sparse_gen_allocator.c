#include <stdlib.h>
#include <stb_ds.h>
#include "sparse_gen_allocator.h"

#define SLOT_COUNT(ALLOC)                   (arrlenu(ALLOC->data) / RLR_SPARSE_GEN_ALLOCATOR_ENTRY_SIZE(ALLOC))

rlr_sparse_gen_allocator_t* rlr_sparse_gen_allocator_create(uint64_t element_size) {
    rlr_sparse_gen_allocator_t* allocator = NULL;
    allocator = malloc(sizeof(rlr_sparse_gen_allocator_t));
    if(!allocator) {
        goto err;
    }

    (*allocator) = (rlr_sparse_gen_allocator_t) {
        .data = NULL,
        .free_slots = NULL,
        .element_size = element_size,
        .generation_counter = 0,
    };
    return allocator;
err:
    rlr_sparse_gen_allocator_free(allocator);
    return NULL;
}

bool rlr_sparse_gen_allocator_exist(rlr_sparse_gen_allocator_t* allocator, rlr_sparse_gen_allocator_handle_t handle) {
    return rlr_sparse_gen_allocator_get(allocator, handle) != NULL;
}

rlr_sparse_gen_allocator_handle_t rlr_sparse_gen_allocator_alloc(rlr_sparse_gen_allocator_t* allocator) {
    uint64_t generation = allocator->generation_counter++;
    if(arrlen(allocator->free_slots) > 0) {
        uint64_t last_index = arrpop(allocator->free_slots);
        rlr_sparse_gen_allocator_handle_t handle = (rlr_sparse_gen_allocator_handle_t) {
            .generation = generation,
            .index = last_index
        };

        (*RLR_SPARSE_GEN_ALLOCATOR_GET_ACTIVE_HANDLE(allocator, last_index)) = handle;
        return handle;
    }

    //there were no free slots, so we need to create a new one
    uint64_t current_length = arrlenu(allocator->data);
    uint64_t new_slot_size = RLR_SPARSE_GEN_ALLOCATOR_ENTRY_SIZE(allocator);
    arrsetlen(allocator->data, current_length + new_slot_size);
    uint64_t slot_count = SLOT_COUNT(allocator);
    uint64_t new_index = slot_count - 1;
    rlr_sparse_gen_allocator_handle_t handle = (rlr_sparse_gen_allocator_handle_t) {
        .generation = generation,
        .index = new_index
    };
    (*RLR_SPARSE_GEN_ALLOCATOR_GET_ACTIVE_HANDLE(allocator, new_index)) = handle;
    return handle;
}

bool rlr_sparse_gen_allocator_dealloc(rlr_sparse_gen_allocator_t* allocator, rlr_sparse_gen_allocator_handle_t handle) {
    if(handle.index > SLOT_COUNT(allocator) - 1) {
        return false;
    }
    rlr_sparse_gen_allocator_handle_t* active_handle = RLR_SPARSE_GEN_ALLOCATOR_GET_ACTIVE_HANDLE(allocator, handle.index);
    if(active_handle->generation != handle.generation) {
        return false;
    }
    (*active_handle) = RLR_SPARSE_GEN_ALLOCATOR_NULL_HANDLE;
    arrpush(allocator->free_slots, handle.index);
    return true;
}

void rlr_sparse_gen_allocator_free(rlr_sparse_gen_allocator_t* allocator) {
    if(!allocator) {
        return;
    }
    arrfree(allocator->free_slots);
    arrfree(allocator->data);
    free(allocator);   
}
