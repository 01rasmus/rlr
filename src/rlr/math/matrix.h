#pragma once
#include <math.h>
#include "vec.h"
#include "quat.h"

#define rlr_mat4x4_get(MAT, COLUMN, ROW)        (MAT.matrix[COLUMN][ROW])
#define rlr_mat4x4_ident                        ((rlr_mat4x4_t){.matrix = {{1.0, 0.0, 0.0, 0.0}, {0.0, 1.0, 0.0, 0.0}, {0.0, 0.0, 1.0, 0.0}, {0.0, 0.0, 0.0, 1.0}}})

typedef struct rlr_affine_mat4x3_t {
    float matrix[4][3]; // [col][row]
} rlr_affine_mat4x3_t;

typedef struct rlr_mat4x4_t {
    float matrix[4][4]; // [col][row]
} rlr_mat4x4_t;

rlr_mat4x4_t rlr_mat4x4_mul(const rlr_mat4x4_t* mat, const rlr_mat4x4_t* other);
rlr_mat4x4_t rlr_mat4x4_mulf(const rlr_mat4x4_t* mat, float other);
rlr_mat4x4_t rlr_mat4x4_trs(const rlr_vec3_t* translation, const rlr_quat_t* rotation, const rlr_vec3_t* scale);
rlr_mat4x4_t rlr_mat4x4_look_towards_quat(const rlr_vec3_t* position, const rlr_quat_t* q);
rlr_mat4x4_t rlr_mat4x4_look_towards_vec3(const rlr_vec3_t* position, const rlr_vec3_t* look_direction, const rlr_vec3_t* up);
rlr_mat4x4_t rlr_mat4x4_look_at(const rlr_vec3_t* position, const rlr_vec3_t* look_position, const rlr_vec3_t* up);
rlr_mat4x4_t rlr_mat4x4_perspective(float fov, float aspect_ratio, float znear, float zfar);
rlr_mat4x4_t rlr_mat4x4_orthographic(float left, float right, float bottom, float top, float znear, float zfar);
rlr_mat4x4_t rlr_mat4x4_transpose(const rlr_mat4x4_t* matrix);
rlr_mat4x4_t rlr_mat4x4_adjoint(const rlr_mat4x4_t* matrix);
rlr_mat4x4_t rlr_mat4x4_inverse(const rlr_mat4x4_t* matrix);
float rlr_mat4x4_determinant(const rlr_mat4x4_t* matrix);

rlr_affine_mat4x3_t rlr_mat4x4_to_affine_mat4x3(const rlr_mat4x4_t* matrix);
rlr_mat4x4_t rlr_affine_mat4x3_to_mat4x4(const rlr_affine_mat4x3_t* affine);

void rlr_mat4x4_print(const rlr_mat4x4_t* matrix);