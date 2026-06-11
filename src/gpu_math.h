#pragma once
#include <math.h>

typedef struct vec2_t {
    float x;
    float y;
} vec2_t;

typedef struct vec3_t {
    float x;
    float y;
    float z;
} vec3_t;

typedef struct quat_t {
    float w;
    float x;
    float y;
    float z;
} quat_t;

typedef struct mat4_t {
    float matrix[4][4];
} mat4_t;

#define vec2(X, Y)              ((vec2_t){.x = X, .y = Y})
#define vec2_zero               vec2({.x = 0.0, .y = 0.0})
#define vec2_add(A, B)          vec2((A).x + (B).x, (A).y + (B).y)
#define vec2_sub(A, B)          vec2((A).x - (B).x, (A).y - (B).y)
#define vec2_mul(A, B)          vec2((A).x * (B).x, (A).y * (B).y)
#define vec2_div(A, B)          vec2((A).x / (B).x, (A).y / (B).y)
#define vec2_addf(A, SCALAR)    vec2((A).x + (SCALAR), (A).y + (SCALAR))
#define vec2_subf(A, SCALAR)    vec2((A).x - (SCALAR), (A).y - (SCALAR))
#define vec2_mulf(A, SCALAR)    vec2((A).x * (SCALAR), (A).y * (SCALAR))
#define vec2_divf(A, SCALAR)    vec2((A).x / (SCALAR), (A).y / (SCALAR))
#define vec2_dot(A, B)          ((A).x * (B).x + (A).y * (B).y)
#define vec2_len(A)             sqrtf((A).x * (A).x + (A).y * (A).y)
#define vec2_normalize(A)       vec2_divf((A), vec2_len((A)))
#define vec2_cross(A, B)        ((A).x * (B).y - (A).y * (B).x)

#define vec3(X, Y, Z)           ((vec3_t){.x = X, .y = Y, .z = Z})
#define vec3_zero               vec3({.x = 0.0, .y = 0.0, .z = 0.0})
#define vec3_add(A, B)          vec3((A).x + (B).x, (A).y + (B).y, (A).z + (B).z)
#define vec3_sub(A, B)          vec3((A).x - (B).x, (A).y - (B).y, (A).z - (B).z)
#define vec3_mul(A, B)          vec3((A).x * (B).x, (A).y * (B).y, (A).z * (B).z)
#define vec3_div(A, B)          vec3((A).x / (B).x, (A).y / (B).y, (A).z / (B).z)
#define vec3_addf(A, SCALAR)    vec3((A).x + (SCALAR), (A).y + (SCALAR), (A).z + (SCALAR))
#define vec3_subf(A, SCALAR)    vec3((A).x - (SCALAR), (A).y - (SCALAR), (A).z - (SCALAR))
#define vec3_mulf(A, SCALAR)    vec3((A).x * (SCALAR), (A).y * (SCALAR), (A).z * (SCALAR))
#define vec3_divf(A, SCALAR)    vec3((A).x / (SCALAR), (A).y / (SCALAR), (A).z / (SCALAR))
#define vec3_dot(A, B)          ((A).x * (B).x + (A).y * (B).y + (A).z * (B).z)
#define vec3_len(A)             sqrtf((A).x * (A).x + (A).y * (A).y + (A).z * (A).z)
#define vec3_normalize(A)       vec3_divf((A), vec3_len((A)))
#define vec3_cross(A, B)        vec3((A).y * (B).z - (A).z * (B).y, (A).z * (B).x - (A).x * (B).z, (A).x * (B).y - (A).y * (B).x)

#define quat(W, X, Y, Z)        ((quat_t){.w = W, .x = X, .y = Y, .z = Z})
#define quat_ident              quat({.w = 1.0, .x = 0.0, .y = 0.0, .z = 0.0})

#define mat4_ident              ((mat4_t){.matrix = {{1.0, 0.0, 0.0, 0.0}, {0.0, 1.0, 0.0, 0.0}, {0.0, 0.0, 1.0, 0.0}, {0.0, 0.0, 0.0, 1.0}}})

quat_t quat_from_euler(vec3_t* euler);
vec3_t quat_to_euler(quat_t* quat);
quat_t quat_slerp(quat_t* qa, quat_t* qb, float t);
quat_t quat_normalize(quat_t* quat);

mat4_t mat4_mul(mat4_t* mat, mat4_t* other);
mat4_t mat4_mulf(mat4_t* mat, float other);
mat4_t mat4_trs(vec3_t* translation, quat_t* rotation, vec3_t* scale);
mat4_t mat4_look_towards_quat(vec3_t* position, quat_t* look_direction, vec3_t* up);
mat4_t mat4_look_towards_vec3(vec3_t* position, vec3_t* look_direction, vec3_t* up);
mat4_t mat4_look_at(vec3_t* position, vec3_t* look_position, vec3_t* up);
mat4_t mat4_perspective(float fov, float aspect_ratio, float znear, float zfar);
mat4_t mat4_orthographic(float left, float right, float bottom, float top, float znear, float zfar);
mat4_t mat4_transpose(mat4_t* matrix);
mat4_t mat4_adjoint(mat4_t* matrix);
mat4_t mat4_inverse(mat4_t* matrix);
float mat4_determinant(mat4_t* matrix);