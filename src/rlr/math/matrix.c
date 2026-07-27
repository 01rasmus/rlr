#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include "matrix.h"

rlr_mat4x4_t rlr_mat4x4_mul(const rlr_mat4x4_t* other, const rlr_mat4x4_t* matrix) {
    rlr_mat4x4_t multiplied;
    for(int32_t row = 0; row < 4; row++) {
        for(int32_t col = 0; col < 4; col++) {
            float m1 = other->matrix[0][row] * matrix->matrix[col][0];
            float m2 = other->matrix[1][row] * matrix->matrix[col][1];
            float m3 = other->matrix[2][row] * matrix->matrix[col][2];
            float m4 = other->matrix[3][row] * matrix->matrix[col][3];
            multiplied.matrix[col][row] = m1 + m2 + m3 + m4;
        }
    }
    return multiplied;
}

rlr_mat4x4_t rlr_mat4x4_mulf(const rlr_mat4x4_t* mat, float other) {
    rlr_mat4x4_t multiplied;
    for(int32_t row = 0; row < 4; row++) {
        for(int32_t col = 0; col < 4; col++) {
            multiplied.matrix[col][row] = mat->matrix[col][row] * other;
        }
    }
    return multiplied;
}

rlr_mat4x4_t rlr_mat4x4_trs(const rlr_vec3_t* translation, const rlr_quat_t* rotation, const rlr_vec3_t* scale) {
    rlr_mat4x4_t translation_rotation_matrix = {0};
    rlr_mat4x4_t scale_matrix = {0};
    
    translation_rotation_matrix.matrix[3][0] = translation->x;
    translation_rotation_matrix.matrix[3][1] = translation->y;
    translation_rotation_matrix.matrix[3][2] = translation->z;
    translation_rotation_matrix.matrix[3][3] = 1.0;

    translation_rotation_matrix.matrix[0][0] = 1.0 - 2.0*rotation->y*rotation->y - 2.0*rotation->z*rotation->z;
    translation_rotation_matrix.matrix[0][1] = 2.0*rotation->x*rotation->y + 2.0*rotation->w*rotation->z;
    translation_rotation_matrix.matrix[0][2] = 2.0*rotation->x*rotation->z - 2.0*rotation->w*rotation->y;
    translation_rotation_matrix.matrix[1][0] = 2.0*rotation->x*rotation->y - 2.0*rotation->w*rotation->z;
    translation_rotation_matrix.matrix[1][1] = 1.0 - 2.0*rotation->x*rotation->x - 2.0*rotation->z*rotation->z;
    translation_rotation_matrix.matrix[1][2] = 2.0*rotation->y*rotation->z + 2.0*rotation->w*rotation->x;
    translation_rotation_matrix.matrix[2][0] = 2.0*rotation->x*rotation->z + 2.0*rotation->w*rotation->y;
    translation_rotation_matrix.matrix[2][1] = 2.0*rotation->y*rotation->z - 2.0*rotation->w*rotation->x;
    translation_rotation_matrix.matrix[2][2] = 1.0 - 2.0*rotation->x*rotation->x - 2.0*rotation->y*rotation->y;

    scale_matrix.matrix[0][0] = scale->x;
    scale_matrix.matrix[1][1] = scale->y;
    scale_matrix.matrix[2][2] = scale->z;
    scale_matrix.matrix[3][3] = 1.0;

    return rlr_mat4x4_mul(&translation_rotation_matrix, &scale_matrix);
}

rlr_mat4x4_t rlr_mat4x4_look_towards_vec3(const rlr_vec3_t* position, const rlr_vec3_t* look_direction, const rlr_vec3_t* up) {
    rlr_vec3_t nf = *look_direction;
    rlr_vec3_t nu = rlr_vec3_normalize(*up);
    rlr_vec3_t ns = rlr_vec3_normalize(rlr_vec3_cross(nf, nu));
    nu = rlr_vec3_cross(ns, nf);

    rlr_mat4x4_t view_matrix = {0};
    view_matrix.matrix[0][0] = ns.x;
    view_matrix.matrix[1][0] = ns.y;
    view_matrix.matrix[2][0] = ns.z;
    view_matrix.matrix[0][1] = nu.x;
    view_matrix.matrix[1][1] = nu.y;
    view_matrix.matrix[2][1] = nu.z;
    view_matrix.matrix[0][2] = -nf.x;
    view_matrix.matrix[1][2] = -nf.y;
    view_matrix.matrix[2][2] = -nf.z;
    view_matrix.matrix[3][0] = -rlr_vec3_dot(ns, *position);
    view_matrix.matrix[3][1] = -rlr_vec3_dot(nu, *position);
    view_matrix.matrix[3][2] =  rlr_vec3_dot(nf, *position);
    view_matrix.matrix[3][3] = 1.0f;
    return view_matrix;
}

rlr_mat4x4_t rlr_mat4x4_look_towards_quat(const rlr_vec3_t* position, const rlr_quat_t* q) {
    float x = q->x;
    float y = q->y;
    float z = q->z;
    float w = q->w;

    float xx = x * x;
    float yy = y * y;
    float zz = z * z;
    float xy = x * y;
    float xz = x * z;
    float yz = y * z;
    float wx = w * x;
    float wy = w * y;
    float wz = w * z;

    float ns_x = 1.0 - 2.0 * (yy + zz);
    float ns_y = 2.0 * (xy + wz);
    float ns_z = 2.0 * (xz - wy);
    float nu_x = 2.0 * (xy - wz);
    float nu_y = 1.0 - 2.0 * (xx + zz);
    float nu_z = 2.0 * (yz + wx);
    float nb_x = 2.0 * (xz + wy);
    float nb_y = 2.0 * (yz - wx);
    float nb_z = 1.0 - 2.0 * (xx + yy);

    rlr_mat4x4_t m = {0};
    m.matrix[0][0] = ns_x;
    m.matrix[1][0] = ns_y;
    m.matrix[2][0] = ns_z;
    m.matrix[0][1] = nu_x;
    m.matrix[1][1] = nu_y;
    m.matrix[2][1] = nu_z;
    m.matrix[0][2] = nb_x;
    m.matrix[1][2] = nb_y;
    m.matrix[2][2] = nb_z;
    m.matrix[3][0] = -(ns_x * position->x + ns_y * position->y + ns_z * position->z);
    m.matrix[3][1] = -(nu_x * position->x + nu_y * position->y + nu_z * position->z);
    m.matrix[3][2] = -(nb_x * position->x + nb_y * position->y + nb_z * position->z);
    m.matrix[3][3] = 1.0;
    return m;
}

rlr_mat4x4_t rlr_mat4x4_look_at(const rlr_vec3_t* position, const rlr_vec3_t* look_position, const rlr_vec3_t* up) {
    rlr_vec3_t direction = rlr_vec3_normalize(rlr_vec3_sub(*look_position, *position));
    return rlr_mat4x4_look_towards_vec3(position, &direction, up);
}

rlr_mat4x4_t rlr_mat4x4_perspective(float fov, float aspect_ratio, float znear, float zfar) {
    float scale = tan(fov * 0.5) * znear;
    float right = aspect_ratio * scale;
    float left = -right;
    float top = scale;
    float bottom = -top;

    rlr_mat4x4_t perspective = {0};
    perspective.matrix[0][0] = (2.0 * znear) / (right - left);
    perspective.matrix[1][1] = (2.0 * znear) / (top - bottom);
    perspective.matrix[0][2] = (right + left) / (right - left);
    perspective.matrix[1][2] = (top + bottom) / (top - bottom);
    perspective.matrix[2][2] = -(zfar + znear) / (zfar - znear);
    perspective.matrix[2][3] = -1.0;
    perspective.matrix[3][2] = (-2.0 * zfar * znear) / (zfar - znear);
    return perspective;
}

rlr_mat4x4_t rlr_mat4x4_orthographic(float left, float right, float bottom, float top, float znear, float zfar) {
    float subx = right-left;
    float suby = top-bottom;
    float subz = zfar-znear;

    rlr_mat4x4_t orthographic = {0};
    orthographic.matrix[0][0] = 2.0 / subx;
    orthographic.matrix[1][1] = 2.0 / suby;
    orthographic.matrix[2][2] = -2.0 / subz;
    orthographic.matrix[3][0] = -(right + left) / subx;
    orthographic.matrix[3][1] = -(top + bottom) / suby;
    orthographic.matrix[3][2] = -(zfar + znear) / subz;
    orthographic.matrix[3][3] = 1.0;
    return orthographic;
}

rlr_mat4x4_t rlr_mat4x4_transpose(const rlr_mat4x4_t* matrix) {
    rlr_mat4x4_t transposed_matrix;
    transposed_matrix.matrix[0][0] = matrix->matrix[0][0];
    transposed_matrix.matrix[0][1] = matrix->matrix[1][0];
    transposed_matrix.matrix[0][2] = matrix->matrix[2][0];
    transposed_matrix.matrix[0][3] = matrix->matrix[3][0];
    transposed_matrix.matrix[1][0] = matrix->matrix[0][1];
    transposed_matrix.matrix[1][1] = matrix->matrix[1][1];
    transposed_matrix.matrix[1][2] = matrix->matrix[2][1];
    transposed_matrix.matrix[1][3] = matrix->matrix[3][1];
    transposed_matrix.matrix[2][0] = matrix->matrix[0][2];
    transposed_matrix.matrix[2][1] = matrix->matrix[1][2];
    transposed_matrix.matrix[2][2] = matrix->matrix[2][2];
    transposed_matrix.matrix[2][3] = matrix->matrix[3][2];
    transposed_matrix.matrix[3][0] = matrix->matrix[0][3];
    transposed_matrix.matrix[3][1] = matrix->matrix[1][3];
    transposed_matrix.matrix[3][2] = matrix->matrix[2][3];
    transposed_matrix.matrix[3][3] = matrix->matrix[3][3];
    return transposed_matrix;
}

rlr_mat4x4_t rlr_mat4x4_adjoint(const rlr_mat4x4_t* matrix) {
    rlr_mat4x4_t adjoint_matrix;
    float a = matrix->matrix[0][0];
    float b = matrix->matrix[1][0];
    float c = matrix->matrix[2][0];
    float d = matrix->matrix[3][0];
    float e = matrix->matrix[0][1];
    float f = matrix->matrix[1][1];
    float g = matrix->matrix[2][1];
    float h = matrix->matrix[3][1];
    float i = matrix->matrix[0][2];
    float j = matrix->matrix[1][2];
    float k = matrix->matrix[2][2];
    float l = matrix->matrix[3][2];
    float m = matrix->matrix[0][3];
    float n = matrix->matrix[1][3];
    float o = matrix->matrix[2][3];
    float p = matrix->matrix[3][3];
    adjoint_matrix.matrix[0][0] = -h*k*n+g*l*n+h*j*o-f*l*o-g*j*p+f*k*p;
    adjoint_matrix.matrix[1][0] = d*k*n-c*l*n-d*j*o+b*l*o+c*j*p-b*k*p;
    adjoint_matrix.matrix[2][0] = -d*g*n+c*h*n+d*f*o-b*h*o-c*f*p+b*g*p;
    adjoint_matrix.matrix[3][0] = d*g*j-c*h*j-d*f*k+b*h*k+c*f*l-b*g*l;
    adjoint_matrix.matrix[0][1] = h*k*m-g*l*m-h*i*o+e*l*o+g*i*p-e*k*p;
    adjoint_matrix.matrix[1][1] = -d*k*m+c*l*m+d*i*o-a*l*o-c*i*p+a*k*p;
    adjoint_matrix.matrix[2][1] = d*g*m-c*h*m-d*e*o+a*h*o+c*e*p-a*g*p;
    adjoint_matrix.matrix[3][1] = -d*g*i+c*h*i+d*e*k-a*h*k-c*e*l+a*g*l;
    adjoint_matrix.matrix[0][2] = -h*j*m+f*l*m+h*i*n-e*l*n-f*i*p+e*j*p;
    adjoint_matrix.matrix[1][2] = d*j*m-b*l*m-d*i*n+a*l*n+b*i*p-a*j*p;
    adjoint_matrix.matrix[2][2] = -d*f*m+b*h*m+d*e*n-a*h*n-b*e*p+a*f*p;
    adjoint_matrix.matrix[3][2] = d*f*i-b*h*i-d*e*j+a*h*j+b*e*l-a*f*l;
    adjoint_matrix.matrix[0][3] = g*j*m-f*k*m-g*i*n+e*k*n+f*i*o-e*j*o;
    adjoint_matrix.matrix[1][3] = -c*j*m+b*k*m+c*i*n-a*k*n-b*i*o+a*j*o;
    adjoint_matrix.matrix[2][3] = c*f*m-b*g*m-c*e*n+a*g*n+b*e*o-a*f*o;
    adjoint_matrix.matrix[3][3] = -c*f*i+b*g*i+c*e*j-a*g*j-b*e*k+a*f*k;
    return adjoint_matrix;
}

rlr_mat4x4_t rlr_mat4x4_inverse(const rlr_mat4x4_t* matrix) {
    rlr_mat4x4_t adjoint = rlr_mat4x4_adjoint(matrix);
    float determinant = rlr_mat4x4_determinant(matrix);
    return rlr_mat4x4_mulf(&adjoint, 1.0 / determinant);
}

static float rlr_det3(float a00, float a01, float a02, float a10, float a11, float a12, float a20, float a21, float a22) {
    return a00 * (a11 * a22 - a12 * a21) - a01 * (a10 * a22 - a12 * a20) + a02 * (a10 * a21 - a11 * a20);
}

float rlr_mat4x4_determinant(const rlr_mat4x4_t* matrix) {
    return
        matrix->matrix[0][0] * rlr_det3(
            matrix->matrix[1][1], matrix->matrix[2][1], matrix->matrix[3][1],
            matrix->matrix[1][2], matrix->matrix[2][2], matrix->matrix[3][2],
            matrix->matrix[1][3], matrix->matrix[2][3], matrix->matrix[3][3]
        )
      - matrix->matrix[1][0] * rlr_det3(
            matrix->matrix[0][1], matrix->matrix[2][1], matrix->matrix[3][1],
            matrix->matrix[0][2], matrix->matrix[2][2], matrix->matrix[3][2],
            matrix->matrix[0][3], matrix->matrix[2][3], matrix->matrix[3][3]
        )
      + matrix->matrix[2][0] * rlr_det3(
            matrix->matrix[0][1], matrix->matrix[1][1], matrix->matrix[3][1],
            matrix->matrix[0][2], matrix->matrix[1][2], matrix->matrix[3][2],
            matrix->matrix[0][3], matrix->matrix[1][3], matrix->matrix[3][3]
        )
      - matrix->matrix[3][0] * rlr_det3(
            matrix->matrix[0][1], matrix->matrix[1][1], matrix->matrix[2][1],
            matrix->matrix[0][2], matrix->matrix[1][2], matrix->matrix[2][2],
            matrix->matrix[0][3], matrix->matrix[1][3], matrix->matrix[2][3]
        );
}

rlr_affine_mat4x3_t rlr_mat4x4_to_affine_mat4x3(const rlr_mat4x4_t* matrix) {
    rlr_affine_mat4x3_t affine;
    memcpy(affine.matrix[0], matrix->matrix[0], sizeof(float) * 3);
    memcpy(affine.matrix[1], matrix->matrix[1], sizeof(float) * 3);
    memcpy(affine.matrix[2], matrix->matrix[2], sizeof(float) * 3);
    memcpy(affine.matrix[3], matrix->matrix[3], sizeof(float) * 3);
    return affine;
}

rlr_mat4x4_t rlr_affine_mat4x3_to_mat4x4(const rlr_affine_mat4x3_t* affine) {
    rlr_mat4x4_t matrix;
    memcpy(matrix.matrix[0], affine->matrix[0], sizeof(float) * 3);
    memcpy(matrix.matrix[1], affine->matrix[1], sizeof(float) * 3);
    memcpy(matrix.matrix[2], affine->matrix[2], sizeof(float) * 3);
    memcpy(matrix.matrix[3], affine->matrix[3], sizeof(float) * 3);
    matrix.matrix[0][3] = 0.0;
    matrix.matrix[1][3] = 0.0;
    matrix.matrix[2][3] = 0.0;
    matrix.matrix[3][3] = 1.0;
    return matrix;
}

void rlr_mat4x4_print(const rlr_mat4x4_t* matrix) {
    printf("[%f, %f, %f, %f]\n", matrix->matrix[0][0], matrix->matrix[0][1], matrix->matrix[0][2], matrix->matrix[0][3]);
    printf("[%f, %f, %f, %f]\n", matrix->matrix[1][0], matrix->matrix[1][1], matrix->matrix[1][2], matrix->matrix[1][3]);
    printf("[%f, %f, %f, %f]\n", matrix->matrix[2][0], matrix->matrix[2][1], matrix->matrix[2][2], matrix->matrix[2][3]);
    printf("[%f, %f, %f, %f]\n", matrix->matrix[3][0], matrix->matrix[3][1], matrix->matrix[3][2], matrix->matrix[3][3]);
}