#include "quat.h"

rlr_quat_t rlr_quat_from_euler(const rlr_vec3_t* euler) {
    rlr_quat_t quat;
    quat.w =  cos(euler->x/2.0)*cos(euler->y/2.0)*cos(euler->z/2.0) + sin(euler->x/2.0)*sin(euler->y/2.0)*sin(euler->z/2.0);
    quat.x = -cos(euler->x/2.0)*sin(euler->y/2.0)*cos(euler->z/2.0) - sin(euler->x/2.0)*cos(euler->y/2.0)*sin(euler->z/2.0);
    quat.y =  cos(euler->x/2.0)*sin(euler->y/2.0)*sin(euler->z/2.0) - sin(euler->x/2.0)*cos(euler->y/2.0)*cos(euler->z/2.0);
    quat.z =  sin(euler->x/2.0)*sin(euler->y/2.0)*cos(euler->z/2.0) - cos(euler->x/2.0)*cos(euler->y/2.0)*sin(euler->z/2.0);
    return quat;
}

rlr_vec3_t rlr_quat_to_euler(const rlr_quat_t* quat) {
    float sp = -2.0 * (quat->y*quat->z - quat->w*quat->x);
    if(fabsf(sp) > 0.9999) {
        float p = 3.14159265 / 2.0 * sp;
        float h = atan2(-quat->x*quat->z - quat->w*quat->y, 0.5 - quat->y*quat->y - quat->z*quat->z);
        float b = 0.0;
        return rlr_vec3(h, p ,b);
    } else {
        float p = asin(sp);
        float h = atan2(quat->x*quat->z - quat->w*quat->y, 0.5 - quat->x*quat->x - quat->y*quat->y);
        float b = atan2(quat->x*quat->y - quat->w*quat->z, 0.5 - quat->x*quat->x - quat->z*quat->z);
        return rlr_vec3(h, p ,b);
    }
}

rlr_quat_t rlr_quat_slerp(const rlr_quat_t* q1, const rlr_quat_t* q2, float t) {
    rlr_quat_t end = *q2;

    float dot = q1->w * end.w + q1->x * end.x + q1->y * end.y + q1->z * end.z;
    if(dot < 0.0f) {
        end.w = -end.w;
        end.x = -end.x;
        end.y = -end.y;
        end.z = -end.z;
        dot = -dot;
    }

    dot = fminf(fmaxf(dot, -1.0f), 1.0f);
    if(dot > 0.9995f) {
        rlr_quat_t result = rlr_quat(
            q1->w + t * (end.w - q1->w),
            q1->x + t * (end.x - q1->x),
            q1->y + t * (end.y - q1->y),
            q1->z + t * (end.z - q1->z)
        );
        float length = sqrtf(result.w * result.w + result.x * result.x + result.y * result.y + result.z * result.z);
        if(length > 0.0f) {
            float inverse_length = 1.0f / length;
            result.w *= inverse_length;
            result.x *= inverse_length;
            result.y *= inverse_length;
            result.z *= inverse_length;
        }
        return result;
    }

    float theta = acosf(dot);
    float sin_theta = sinf(theta);
    float ratio_1 = sinf((1.0f - t) * theta) / sin_theta;
    float ratio_2 = sinf(t * theta) / sin_theta;

    return rlr_quat(
        q1->w * ratio_1 + end.w * ratio_2,
        q1->x * ratio_1 + end.x * ratio_2,
        q1->y * ratio_1 + end.y * ratio_2,
        q1->z * ratio_1 + end.z * ratio_2
    );
}

rlr_quat_t rlr_quat_normalize(const rlr_quat_t* quat) {
    rlr_quat_t new_quat;
    float norm = sqrt(quat->x*quat->x + quat->y*quat->y + quat->z*quat->z + quat->w*quat->w);
    new_quat.x = quat->x / norm;
    new_quat.y = quat->y / norm;
    new_quat.z = quat->z / norm;
    new_quat.w = quat->w / norm;
    return new_quat;
}