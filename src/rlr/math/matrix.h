#pragma once
#include <math.h>
#include "vec.h"
#include "quat.h"

typedef struct rlr_mat4_t {
    float matrix[4][4];
} rlr_mat4_t;

#define rlr_mat4_ident              ((rlr_mat4_t){.matrix = {{1.0, 0.0, 0.0, 0.0}, {0.0, 1.0, 0.0, 0.0}, {0.0, 0.0, 1.0, 0.0}, {0.0, 0.0, 0.0, 1.0}}})

rlr_mat4_t rlr_mat4_mul(rlr_mat4_t* mat, rlr_mat4_t* other);
rlr_mat4_t rlr_mat4_mulf(rlr_mat4_t* mat, float other);
rlr_mat4_t rlr_mat4_trs(rlr_vec3_t* translation, rlr_quat_t* rotation, rlr_vec3_t* scale);
rlr_mat4_t rlr_mat4_look_towards_quat(rlr_vec3_t* position, rlr_quat_t* look_direction, rlr_vec3_t* up);
rlr_mat4_t rlr_mat4_look_towards_vec3(rlr_vec3_t* position, rlr_vec3_t* look_direction, rlr_vec3_t* up);
rlr_mat4_t rlr_mat4_look_at(rlr_vec3_t* position, rlr_vec3_t* look_position, rlr_vec3_t* up);
rlr_mat4_t rlr_mat4_perspective(float fov, float aspect_ratio, float znear, float zfar);
rlr_mat4_t rlr_mat4_orthographic(float left, float right, float bottom, float top, float znear, float zfar);
rlr_mat4_t rlr_mat4_transpose(rlr_mat4_t* matrix);
rlr_mat4_t rlr_mat4_adjoint(rlr_mat4_t* matrix);
rlr_mat4_t rlr_mat4_inverse(rlr_mat4_t* matrix);
float rlr_mat4_determinant(rlr_mat4_t* matrix);