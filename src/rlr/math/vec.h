#pragma once
#include <math.h>

typedef struct rlr_vec2_t {
    float x;
    float y;
} rlr_vec2_t;

typedef struct rlr_vec3_t {
    float x;
    float y;
    float z;
} rlr_vec3_t;

typedef struct rlr_vec4_t {
    float x;
    float y;
    float z;
    float w;
} rlr_vec4_t;

#define rlr_vec2(X, Y)              ((rlr_vec2_t){.x = X, .y = Y})
#define rlr_vec2_zero               rlr_vec2({.x = 0.0, .y = 0.0})
#define rlr_vec2_add(A, B)          rlr_vec2((A).x + (B).x, (A).y + (B).y)
#define rlr_vec2_sub(A, B)          rlr_vec2((A).x - (B).x, (A).y - (B).y)
#define rlr_vec2_mul(A, B)          rlr_vec2((A).x * (B).x, (A).y * (B).y)
#define rlr_vec2_div(A, B)          rlr_vec2((A).x / (B).x, (A).y / (B).y)
#define rlr_vec2_addf(A, SCALAR)    rlr_vec2((A).x + (SCALAR), (A).y + (SCALAR))
#define rlr_vec2_subf(A, SCALAR)    rlr_vec2((A).x - (SCALAR), (A).y - (SCALAR))
#define rlr_vec2_mulf(A, SCALAR)    rlr_vec2((A).x * (SCALAR), (A).y * (SCALAR))
#define rlr_vec2_divf(A, SCALAR)    rlr_vec2((A).x / (SCALAR), (A).y / (SCALAR))
#define rlr_vec2_dot(A, B)          ((A).x * (B).x + (A).y * (B).y)
#define rlr_vec2_len(A)             sqrtf((A).x * (A).x + (A).y * (A).y)
#define rlr_vec2_normalize(A)       rlr_vec2_divf((A), rlr_vec2_len((A)))
#define rlr_vec2_cross(A, B)        ((A).x * (B).y - (A).y * (B).x)

#define rlr_vec3(X, Y, Z)           ((rlr_vec3_t){.x = (X), .y = (Y), .z = (Z)})
#define rlr_vec3_zero               ((rlr_vec3_t){.x = 0.0, .y = 0.0, .z = 0.0})
#define rlr_vec3_add(A, B)          rlr_vec3((A).x + (B).x, (A).y + (B).y, (A).z + (B).z)
#define rlr_vec3_sub(A, B)          rlr_vec3((A).x - (B).x, (A).y - (B).y, (A).z - (B).z)
#define rlr_vec3_mul(A, B)          rlr_vec3((A).x * (B).x, (A).y * (B).y, (A).z * (B).z)
#define rlr_vec3_div(A, B)          rlr_vec3((A).x / (B).x, (A).y / (B).y, (A).z / (B).z)
#define rlr_vec3_addf(A, SCALAR)    rlr_vec3((A).x + (SCALAR), (A).y + (SCALAR), (A).z + (SCALAR))
#define rlr_vec3_subf(A, SCALAR)    rlr_vec3((A).x - (SCALAR), (A).y - (SCALAR), (A).z - (SCALAR))
#define rlr_vec3_mulf(A, SCALAR)    rlr_vec3((A).x * (SCALAR), (A).y * (SCALAR), (A).z * (SCALAR))
#define rlr_vec3_divf(A, SCALAR)    rlr_vec3((A).x / (SCALAR), (A).y / (SCALAR), (A).z / (SCALAR))
#define rlr_vec3_dot(A, B)          ((A).x * (B).x + (A).y * (B).y + (A).z * (B).z)
#define rlr_vec3_len(A)             sqrtf((A).x * (A).x + (A).y * (A).y + (A).z * (A).z)
#define rlr_vec3_normalize(A)       rlr_vec3_divf((A), rlr_vec3_len((A)))
#define rlr_vec3_cross(A, B)        rlr_vec3((A).y * (B).z - (A).z * (B).y, (A).z * (B).x - (A).x * (B).z, (A).x * (B).y - (A).y * (B).x)
#define rlr_vec3_lerp(A, B, AMOUNT) rlr_vec3((A).x + (AMOUNT) * ((B).x - (A).x), (A).y + (AMOUNT) * ((B).y - (A).y), (A).z + (AMOUNT) * ((B).z - (A).z))