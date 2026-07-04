#pragma once
#include <stdbool.h>
#include <stdint.h>

#define RLR_SPARSE_GEN_ALLOCATOR_NULL_HANDLE                        ((rlr_sparse_gen_allocator_handle_t){.generation = -1, .index = -1})
#define RLR_SPARSE_GEN_ALLOCATOR_ENTRY_SIZE(ALLOC)                  (sizeof(rlr_sparse_gen_allocator_handle_t) + (ALLOC)->element_size)
#define RLR_SPARSE_GEN_ALLOCATOR_GET_ACTIVE_HANDLE(ALLOC, INDEX)    (rlr_sparse_gen_allocator_handle_t*)(&((ALLOC)->data[(INDEX) * RLR_SPARSE_GEN_ALLOCATOR_ENTRY_SIZE(ALLOC)]))

typedef struct rlr_sparse_gen_allocator_handle_t {
    uint64_t index;
    uint64_t generation;
} rlr_sparse_gen_allocator_handle_t;

typedef struct rlr_sparse_gen_allocator_t {
    uint64_t generation_counter;
    uint64_t element_size;
    uint8_t* data;
    uint64_t* free_slots;
} rlr_sparse_gen_allocator_t;

rlr_sparse_gen_allocator_t* rlr_sparse_gen_allocator_create(uint64_t element_size);
rlr_sparse_gen_allocator_handle_t rlr_sparse_gen_allocator_alloc(rlr_sparse_gen_allocator_t* allocator);

/* returns false whenever the deallocation fails, which can happen if the handle has an older generation than the current one */
bool rlr_sparse_gen_allocator_dealloc(rlr_sparse_gen_allocator_t* allocator, rlr_sparse_gen_allocator_handle_t handle);
void rlr_sparse_gen_allocator_free(rlr_sparse_gen_allocator_t* allocator);

static inline void* rlr_sparse_gen_allocator_get_unchecked(rlr_sparse_gen_allocator_t* allocator, rlr_sparse_gen_allocator_handle_t handle) {
    rlr_sparse_gen_allocator_handle_t* active = RLR_SPARSE_GEN_ALLOCATOR_GET_ACTIVE_HANDLE(allocator, handle.index);
    uintptr_t valid_gen = active->generation == handle.generation;
    return (void*)((uintptr_t)(active + 1) * valid_gen);
}