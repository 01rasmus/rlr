#pragma once
#include <stdbool.h>
#include <assert.h>
#include "../../rlr/resources/animated_model.h"
#include "../../rlr/resources/cube_map.h"
#include "../../rlr/resources/font.h"
#include "../../rlr/resources/shader.h"
#include "../../rlr/resources/static_model.h"
#include "../../rlr/resources/texture_atlas.h"
#include "../../rlr/resources/texture.h"
#include "../../rlr/resources/uniform.h"
#include "../../rlr/objects/animated_model.h"
#include "../../rlr/objects/label.h"
#include "../../rlr/objects/model_occluder.h"
#include "../../rlr/objects/sprite.h"
#include "../../rlr/objects/static_model.h"
#include "../../external/rlpp.h"
#include "../../rlr/def.h"

/*
    a memory manager that stores all
    the resources and the objects
*/

#define RLR_MEM_MAN_RESOURCE_SCHEMA(X) \
    X(res, animated_model) \
    X(res, cube_map) \
    X(res, font) \
    X(res, shader) \
    X(res, static_model) \
    X(res, texture_atlas_tile) \
    X(res, texture_atlas) \
    X(res, texture) \
    X(res, uniform)

#define RLR_MEM_MAN_OBJECT_SCHEMA(X) \
    X(obj, animated_model) \
    X(obj, label) \
    X(obj, model_occluder) \
    X(obj, sprite) \
    X(obj, static_model)

typedef struct rlr_mem_man_t {
    #define X(TYPE, NAME) rlr_##TYPE##_##NAME##_t* TYPE##_##NAME##s;
    RLR_MEM_MAN_RESOURCE_SCHEMA(X)
    RLR_MEM_MAN_OBJECT_SCHEMA(X)
    #undef X
} rlr_mem_man_t;

bool rlr_mem_man_init(rlr_mem_man_t* rm);
void rlr_mem_man_deinit(rlr_mem_man_t* rm);

//define allocators
#define X(TYPE, NAME) rlr_##TYPE##_t rlr_mem_man_allocate_##TYPE##_##NAME(rlr_mem_man_t* rm, rlr_##TYPE##_##NAME##_t data);
RLR_MEM_MAN_RESOURCE_SCHEMA(X)
RLR_MEM_MAN_OBJECT_SCHEMA(X)
#undef X

//define deallocators
#define X(TYPE, NAME) rlr_##TYPE##_t rlr_mem_man_free_##TYPE##_##NAME(rlr_mem_man_t* rm, rlr_##TYPE##_t TYPE);
RLR_MEM_MAN_RESOURCE_SCHEMA(X)
RLR_MEM_MAN_OBJECT_SCHEMA(X)
#undef X

//define getters
#ifndef RLR_DEBUG
#define X(TYPE, NAME) \
    static inline rlr_##TYPE##_##NAME##_t* rlr_mem_man_get_##TYPE##_##NAME(rlr_mem_man_t* rm, rlr_##TYPE##_t TYPE) { \
        return rlpp_get_unchecked(rm->TYPE##_##NAME##s, TYPE); \
    }
#else
#define X(TYPE, NAME) \
    static inline rlr_##TYPE##_##NAME##_t* rlr_mem_man_get_##TYPE##_##NAME(rlr_mem_man_t* rm, rlr_##TYPE##_t TYPE) { \
        rlr_##TYPE##_##NAME##_t* ptr = rlpp_get(rm->TYPE##_##NAME##s, TYPE); \
        assert(ptr != NULL && "resource pointer is null"); \
        return ptr; \
    }
#endif

RLR_MEM_MAN_RESOURCE_SCHEMA(X)
RLR_MEM_MAN_OBJECT_SCHEMA(X)
#undef X

//define array getters
#define X(TYPE, NAME) \
    static inline rlr_##TYPE##_##NAME##_t* rlr_mem_man_get_##TYPE##_##NAME##s(rlr_mem_man_t* rm) {\
        return rm->TYPE##_##NAME##s; \
    }

RLR_MEM_MAN_RESOURCE_SCHEMA(X)
RLR_MEM_MAN_OBJECT_SCHEMA(X)
#undef X

//define array count getters
#define X(TYPE, NAME) \
    static inline uint32_t rlr_mem_man_get_##TYPE##_##NAME##s_count(rlr_mem_man_t* rm) {\
        return rlpp_len(rm->TYPE##_##NAME##s); \
    }

RLR_MEM_MAN_RESOURCE_SCHEMA(X)
RLR_MEM_MAN_OBJECT_SCHEMA(X)
#undef X