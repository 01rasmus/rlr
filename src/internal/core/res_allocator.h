// #pragma once
// #include "rlr.h"

// #define rlr_res_allocator_handle_create(VERSION, INDEX)
// #define rlr_res_allocator_get(ALLOC, RES_HANDLE)   (ALLOC->data + RES_HANDLE)

// typedef struct rlr_res_allocator_t {
//     uint64_t element_size;
//     uint8_t* data;
// } rlr_res_allocator_t;

// rlr_res_allocator_t* rlr_res_allocator_create(uint64_t element_size);

// rlr_res_t rlr_res_allocator_allocate_resource(rlr_res_allocator_t* allocator);
// void rlr_res_allocator_free_resource(rlr_res_allocator_t* allocator, rlr_res_t handle);

// void rlr_res_allocator_free(rlr_res_allocator_t* allocator);