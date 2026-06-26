#pragma once
#include <math.h>
#include "vec.h"

typedef struct rlr_quat_t {
    float w;
    float x;
    float y;
    float z;
} rlr_quat_t;

#define rlr_quat(W, X, Y, Z)        ((rlr_quat_t){.w = W, .x = X, .y = Y, .z = Z})
#define rlr_quat_ident              ((rlr_quat_t){.w = 1.0, .x = 0.0, .y = 0.0, .z = 0.0})

rlr_quat_t rlr_quat_from_euler(const rlr_vec3_t* euler);
rlr_vec3_t rlr_quat_to_euler(const rlr_quat_t* quat);
rlr_quat_t rlr_quat_slerp(const rlr_quat_t* qa, const rlr_quat_t* qb, float t);
rlr_quat_t rlr_quat_normalize(const rlr_quat_t* quat);