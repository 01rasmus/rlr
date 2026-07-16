#include "../../external/rlpp.h"
#include "memory_manager.h"

bool rlr_mem_man_init(rlr_mem_man_t* rm) {
    if(!rm) {
        return false;
    }
    (*rm) = (rlr_mem_man_t){0};
    return true;
}

void rlr_mem_man_deinit(rlr_mem_man_t* rm) {
    #define X(TYPE, NAME) rlpp_free(rm->TYPE##_##NAME##s);
    RLR_MEM_MAN_RESOURCE_SCHEMA(X)
    RLR_MEM_MAN_OBJECT_SCHEMA(X)
    #undef X
}

//allocation implementations
#define X(TYPE, NAME) \
    rlr_##TYPE##_t rlr_mem_man_allocate_##TYPE##_##NAME(rlr_mem_man_t* rm, rlr_##TYPE##_##NAME##_t data) { \
        return rlpp_alloc(rm->TYPE##_##NAME##s, data); \
    }

RLR_MEM_MAN_RESOURCE_SCHEMA(X)
RLR_MEM_MAN_OBJECT_SCHEMA(X)
#undef X

//deallocation implementations
#define X(TYPE, NAME) \
    rlr_##TYPE##_t rlr_mem_man_free_##TYPE##_##NAME(rlr_mem_man_t* rm, rlr_##TYPE##_t TYPE) { \
        rlpp_remove(rm->TYPE##_##NAME##s, TYPE); \
    }

RLR_MEM_MAN_RESOURCE_SCHEMA(X)
RLR_MEM_MAN_OBJECT_SCHEMA(X)
#undef X