#include <stdint.h>
#include <string.h>
#include "gpu_math.h"

quat_t quat_from_euler(vec3_t* euler) {
    quat_t quat;
    quat.w =  cos(euler->x/2.0)*cos(euler->y/2.0)*cos(euler->z/2.0) + sin(euler->x/2.0)*sin(euler->y/2.0)*sin(euler->z/2.0);
    quat.x = -cos(euler->x/2.0)*sin(euler->y/2.0)*cos(euler->z/2.0) - sin(euler->x/2.0)*cos(euler->y/2.0)*sin(euler->z/2.0);
    quat.y =  cos(euler->x/2.0)*sin(euler->y/2.0)*sin(euler->z/2.0) - sin(euler->x/2.0)*cos(euler->y/2.0)*cos(euler->z/2.0);
    quat.z =  sin(euler->x/2.0)*sin(euler->y/2.0)*cos(euler->z/2.0) - cos(euler->x/2.0)*cos(euler->y/2.0)*sin(euler->z/2.0);
    return quat;
}

vec3_t quat_to_euler(quat_t* quat) {
    float sp = -2.0 * (quat->y*quat->z - quat->w*quat->x);
    if(fabsf(sp) > 0.9999) {
        float p = 3.14159265 / 2.0 * sp;
        float h = atan2(-quat->x*quat->z - quat->w*quat->y, 0.5 - quat->y*quat->y - quat->z*quat->z);
        float b = 0.0;
        return vec3(h, p ,b);
    } else {
        float p = asin(sp);
        float h = atan2(quat->x*quat->z - quat->w*quat->y, 0.5 - quat->x*quat->x - quat->y*quat->y);
        float b = atan2(quat->x*quat->y - quat->w*quat->z, 0.5 - quat->x*quat->x - quat->z*quat->z);
        return vec3(h, p ,b);
    }
}

quat_t quat_slerp(quat_t* q1, quat_t* q2, float t) {
    float cos_half_theta = q1->w * q2->w + q1->x * q2->x + q1->y * q2->y + q1->z * q2->z;
    if(fabsf(cos_half_theta) >= 1.0){
        return quat(q1->w, q1->x, q1->y, q1->z);
    }

    float half_theta = acos(cos_half_theta);
    float sinhalf_theta = sqrt(1.0 - cos_half_theta*cos_half_theta);
    if (fabs(sinhalf_theta) < 0.001){
        return quat(q1->w * 0.5 + q2->w * 0.5, q1->x * 0.5 + q2->x * 0.5, q1->y * 0.5 + q2->y * 0.5, q1->z * 0.5 + q2->z * 0.5);
    }

    float ratio_1 = sin((1 - t) * half_theta) / sinhalf_theta;
    float ratio_2 = sin(t * half_theta) / sinhalf_theta; 
    return quat(q1->w * ratio_1 + q2->w * ratio_2, q1->x * ratio_1 + q2->x * ratio_2, q1->y * ratio_1 + q2->y * ratio_2, q1->z * ratio_1 + q2->z * ratio_2);
}

quat_t quat_normalize(quat_t* quat) {
    quat_t new_quat;
    float norm = sqrt(quat->x*quat->x + quat->y*quat->y + quat->z*quat->z + quat->w*quat->w);
    new_quat.x = quat->w / norm;
    new_quat.y = quat->x / norm;
    new_quat.z = quat->y / norm;
    new_quat.w = quat->z / norm;
    return new_quat;
}

mat4_t mat4_mul(mat4_t* mat, mat4_t* other) {
    mat4_t multiplied;
    for(int32_t y = 0; y < 4; y++) {
        for(int32_t x = 0; x < 4; x++) {
            float m1 = other->matrix[x][0] * mat->matrix[0][y];
            float m2 = other->matrix[x][1] * mat->matrix[1][y];
            float m3 = other->matrix[x][2] * mat->matrix[2][y];
            float m4 = other->matrix[x][3] * mat->matrix[3][y];
            multiplied.matrix[x][y] = m1 + m2 + m3 + m4;
        }
    }
    return multiplied;
}

mat4_t mat4_mulf(mat4_t* mat, float other) {
    mat4_t multiplied;
    for(int32_t y = 0; y < 4; y++) {
        for(int32_t x = 0; x < 4; x++) {
            multiplied.matrix[x][y] = mat->matrix[x][y] * other;
        }
    }
    return multiplied;
}

mat4_t mat4_trs(vec3_t* translation, quat_t* rotation, vec3_t* scale) {
    mat4_t translation_rotation_matrix;
    mat4_t scale_matrix;
    
    memset(translation_rotation_matrix.matrix, 0, 16);
    memset(scale_matrix.matrix, 0, 16);
    
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

    return mat4_mul(&translation_rotation_matrix, &scale_matrix);
}

mat4_t mat4_look_towards_vec3(vec3_t* position, vec3_t* look_direction, vec3_t* up) {
    vec3_t nf = *look_direction;
    vec3_t nu = vec3_normalize(*up);
    vec3_t ns = vec3_cross(nf, nu);
    vec3_normalize(ns);
    nu = vec3_cross(ns, nf);

    mat4_t view_matrix;
    view_matrix.matrix[0][0] = ns.x;
    view_matrix.matrix[0][1] = nu.x;
    view_matrix.matrix[0][2] = -nf.x;
    view_matrix.matrix[1][0] = ns.y;
    view_matrix.matrix[1][1] = nu.y;
    view_matrix.matrix[1][2] = -nf.y;
    view_matrix.matrix[2][0] = ns.z;
    view_matrix.matrix[2][1] = nu.z;
    view_matrix.matrix[2][2] = -nf.z;
    view_matrix.matrix[3][0] = -vec3_dot(ns, *position);
    view_matrix.matrix[3][1] = -vec3_dot(nu, *position);
    view_matrix.matrix[3][2] = vec3_dot(nf, *position);
    return view_matrix;
}

mat4_t mat4_look_towards_quat(vec3_t* position, quat_t* look_direction, vec3_t* up) {
    vec3_t euler_direction = quat_to_euler(look_direction);
    return mat4_look_towards_vec3(position, &euler_direction, up);
}

mat4_t mat4_look_at(vec3_t* position, vec3_t* look_position, vec3_t* up) {
    vec3_t direction = vec3_sub(*look_position, *position);
    vec3_normalize(direction);
    return mat4_look_towards_vec3(position, &direction, up);
}

mat4_t mat4_perspective(float fov, float aspect_ratio, float znear, float zfar) {
    float scale = tan(fov * 0.5) * znear;
    float right = aspect_ratio * scale;
    float left = -right;
    float top = scale;
    float bottom = -top;

    mat4_t perspective;
    perspective.matrix[0][0] = (2.0 * znear) / (right - left);
    perspective.matrix[0][1] = 0.0;
    perspective.matrix[0][2] = 0.0;
    perspective.matrix[0][3] = 0.0;
    perspective.matrix[1][0] = 0.0;
    perspective.matrix[1][1] = (2.0 * znear) / (top - bottom);
    perspective.matrix[1][2] = 0.0;
    perspective.matrix[1][3] = 0.0;
    perspective.matrix[2][0] = (right + left) / (right - left);
    perspective.matrix[2][1] = (top + bottom) / (top - bottom);
    perspective.matrix[2][2] = -(zfar + znear) / (zfar - znear);
    perspective.matrix[2][3] = -1.0;   
    perspective.matrix[3][0] = 0.0;
    perspective.matrix[3][1] = 0.0;
    perspective.matrix[3][2] = (-2.0 * zfar * znear) / (zfar - znear);
    perspective.matrix[3][3] = 0.0;
    return perspective;
}

mat4_t mat4_orthographic(float left, float right, float bottom, float top, float znear, float zfar) {
    float subx = right-left;
    float suby = top-bottom;
    float subz = zfar-znear;

    mat4_t orthographic;
    orthographic.matrix[0][0] = 2.0 / subx;
    orthographic.matrix[0][1] = 0.0;
    orthographic.matrix[0][2] = 0.0;
    orthographic.matrix[0][3] = 0.0;
    orthographic.matrix[1][0] = 0.0;
    orthographic.matrix[1][1] = 2.0 / suby;
    orthographic.matrix[1][2] = 0.0;
    orthographic.matrix[1][3] = 0.0;
    orthographic.matrix[2][0] = 0.0;
    orthographic.matrix[2][1] = 0.0;
    orthographic.matrix[2][2] = -2.0 / subz;
    orthographic.matrix[2][3] = 0.0;
    orthographic.matrix[3][0] = -(right + left) / subx;
    orthographic.matrix[3][1] = -(top + bottom) / suby;
    orthographic.matrix[3][2] = -(zfar + znear) / subz;
    orthographic.matrix[3][3] = 1.0;
    return orthographic;
}

mat4_t mat4_transpose(mat4_t* matrix) {
    mat4_t transposed_matrix;
    transposed_matrix.matrix[0][0] = matrix->matrix[0][0];
    transposed_matrix.matrix[1][0] = matrix->matrix[0][1];
    transposed_matrix.matrix[2][0] = matrix->matrix[0][2];
    transposed_matrix.matrix[3][0] = matrix->matrix[0][3];
    transposed_matrix.matrix[0][1] = matrix->matrix[1][0];
    transposed_matrix.matrix[1][1] = matrix->matrix[1][1];
    transposed_matrix.matrix[2][1] = matrix->matrix[1][2];
    transposed_matrix.matrix[3][1] = matrix->matrix[1][3];
    transposed_matrix.matrix[0][2] = matrix->matrix[2][0];
    transposed_matrix.matrix[1][2] = matrix->matrix[2][1];
    transposed_matrix.matrix[2][2] = matrix->matrix[2][2];
    transposed_matrix.matrix[3][2] = matrix->matrix[2][3];
    transposed_matrix.matrix[0][3] = matrix->matrix[3][0];
    transposed_matrix.matrix[1][3] = matrix->matrix[3][1];
    transposed_matrix.matrix[2][3] = matrix->matrix[3][2];
    transposed_matrix.matrix[3][3] = matrix->matrix[3][3];
    return transposed_matrix;
}

mat4_t mat4_adjoint(mat4_t* matrix) {
    mat4_t adjoint_matrix;
    float a = matrix->matrix[0][0];
    float b = matrix->matrix[0][1];
    float c = matrix->matrix[0][2];
    float d = matrix->matrix[0][3];
    float e = matrix->matrix[1][0];
    float f = matrix->matrix[1][1];
    float g = matrix->matrix[1][2];
    float h = matrix->matrix[1][3];
    float i = matrix->matrix[2][0];
    float j = matrix->matrix[2][1];
    float k = matrix->matrix[2][2];
    float l = matrix->matrix[2][3];
    float m = matrix->matrix[3][0];
    float n = matrix->matrix[3][1];
    float o = matrix->matrix[3][2];
    float p = matrix->matrix[3][3];
    adjoint_matrix.matrix[0][0] = -h*k*n+g*l*n+h*j*o-f*l*o-g*j*p+f*k*p;
    adjoint_matrix.matrix[0][1] = d*k*n-c*l*n-d*j*o+b*l*o+c*j*p-b*k*p;
    adjoint_matrix.matrix[0][2] = -d*g*n+c*h*n+d*f*o-b*h*o-c*f*p+b*g*p;
    adjoint_matrix.matrix[0][3] = d*g*j-c*h*j-d*f*k+b*h*k+c*f*l-b*g*l;
    adjoint_matrix.matrix[1][0] = h*k*m-g*l*m-h*i*o+e*l*o+g*i*p-e*k*p;
    adjoint_matrix.matrix[1][1] = -d*k*m+c*l*m+d*i*o-a*l*o-c*i*p+a*k*p;
    adjoint_matrix.matrix[1][2] = d*g*m-c*h*m-d*e*o+a*h*o+c*e*p-a*g*p;
    adjoint_matrix.matrix[1][3] = -d*g*i+c*h*i+d*e*k-a*h*k-c*e*l+a*g*l;
    adjoint_matrix.matrix[2][0] = -h*j*m+f*l*m+h*i*n-e*l*n-f*i*p+e*j*p;
    adjoint_matrix.matrix[2][1] = d*j*m-b*l*m-d*i*n+a*l*n+b*i*p-a*j*p;
    adjoint_matrix.matrix[2][2] = -d*f*m+b*h*m+d*e*n-a*h*n-b*e*p+a*f*p;
    adjoint_matrix.matrix[2][3] = d*f*i-b*h*i-d*e*j+a*h*j+b*e*l-a*f*l;
    adjoint_matrix.matrix[3][0] = g*j*m-f*k*m-g*i*n+e*k*n+f*i*o-e*j*o;
    adjoint_matrix.matrix[3][1] = -c*j*m+b*k*m+c*i*n-a*k*n-b*i*o+a*j*o;
    adjoint_matrix.matrix[3][2] = c*f*m-b*g*m-c*e*n+a*g*n+b*e*o-a*f*o;
    adjoint_matrix.matrix[3][3] = -c*f*i+b*g*i+c*e*j-a*g*j-b*e*k+a*f*k;
    return adjoint_matrix;
}

mat4_t mat4_inverse(mat4_t* matrix) {
    mat4_t adjoint = mat4_adjoint(matrix);
    float determinant = mat4_determinant(matrix);
    return mat4_mulf(&adjoint, 1.0 / determinant);
}

float mat4_determinant(mat4_t* matrix) {
    float det1 = matrix->matrix[0][0] * (matrix->matrix[1][1] * (matrix->matrix[2][2]*matrix->matrix[3][3]-matrix->matrix[2][3]*matrix->matrix[3][2]) + matrix->matrix[1][2]*(matrix->matrix[2][3]*matrix->matrix[3][1] - matrix->matrix[2][1]*matrix->matrix[3][3]) + matrix->matrix[1][3] * (matrix->matrix[2][1]*matrix->matrix[3][2]-matrix->matrix[2][2]*matrix->matrix[3][1]));
    float det2 = matrix->matrix[0][1] * (matrix->matrix[1][0] * (matrix->matrix[2][2]*matrix->matrix[3][3]-matrix->matrix[2][3]*matrix->matrix[3][2]) + matrix->matrix[1][2]*(matrix->matrix[2][3]*matrix->matrix[3][0] - matrix->matrix[2][0]*matrix->matrix[3][3]) + matrix->matrix[1][3] * (matrix->matrix[2][0]*matrix->matrix[3][2]-matrix->matrix[2][2]*matrix->matrix[3][0]));
    float det3 = matrix->matrix[0][2] * (matrix->matrix[1][0] * (matrix->matrix[2][1]*matrix->matrix[3][3]-matrix->matrix[2][3]*matrix->matrix[3][1]) + matrix->matrix[1][1]*(matrix->matrix[2][3]*matrix->matrix[3][1] - matrix->matrix[2][0]*matrix->matrix[3][3]) + matrix->matrix[1][3] * (matrix->matrix[2][0]*matrix->matrix[3][1]-matrix->matrix[2][1]*matrix->matrix[3][0]));
    float det4 = matrix->matrix[0][3] * (matrix->matrix[1][0] * (matrix->matrix[2][1]*matrix->matrix[3][2]-matrix->matrix[2][3]*matrix->matrix[3][1]) + matrix->matrix[1][1]*(matrix->matrix[2][2]*matrix->matrix[3][1] - matrix->matrix[2][0]*matrix->matrix[3][2]) + matrix->matrix[1][2] * (matrix->matrix[2][0]*matrix->matrix[3][1]-matrix->matrix[2][1]*matrix->matrix[3][0]));
    return det1 - det2 + det3 - det4;
}