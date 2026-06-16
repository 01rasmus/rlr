#include "quat.h"

rlr_quat_t rlr_quat_from_euler(rlr_vec3_t* euler) {
    rlr_quat_t quat;
    quat.w =  cos(euler->x/2.0)*cos(euler->y/2.0)*cos(euler->z/2.0) + sin(euler->x/2.0)*sin(euler->y/2.0)*sin(euler->z/2.0);
    quat.x = -cos(euler->x/2.0)*sin(euler->y/2.0)*cos(euler->z/2.0) - sin(euler->x/2.0)*cos(euler->y/2.0)*sin(euler->z/2.0);
    quat.y =  cos(euler->x/2.0)*sin(euler->y/2.0)*sin(euler->z/2.0) - sin(euler->x/2.0)*cos(euler->y/2.0)*cos(euler->z/2.0);
    quat.z =  sin(euler->x/2.0)*sin(euler->y/2.0)*cos(euler->z/2.0) - cos(euler->x/2.0)*cos(euler->y/2.0)*sin(euler->z/2.0);
    return quat;
}

rlr_vec3_t rlr_quat_to_euler(rlr_quat_t* quat) {
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

rlr_quat_t rlr_quat_slerp(rlr_quat_t* q1, rlr_quat_t* q2, float t) {
    float cos_half_theta = q1->w * q2->w + q1->x * q2->x + q1->y * q2->y + q1->z * q2->z;
    if(fabsf(cos_half_theta) >= 1.0){
        return rlr_quat(q1->w, q1->x, q1->y, q1->z);
    }

    float half_theta = acos(cos_half_theta);
    float sinhalf_theta = sqrt(1.0 - cos_half_theta*cos_half_theta);
    if (fabs(sinhalf_theta) < 0.001){
        return rlr_quat(q1->w * 0.5 + q2->w * 0.5, q1->x * 0.5 + q2->x * 0.5, q1->y * 0.5 + q2->y * 0.5, q1->z * 0.5 + q2->z * 0.5);
    }

    float ratio_1 = sin((1 - t) * half_theta) / sinhalf_theta;
    float ratio_2 = sin(t * half_theta) / sinhalf_theta; 
    return rlr_quat(q1->w * ratio_1 + q2->w * ratio_2, q1->x * ratio_1 + q2->x * ratio_2, q1->y * ratio_1 + q2->y * ratio_2, q1->z * ratio_1 + q2->z * ratio_2);
}

rlr_quat_t rlr_quat_normalize(rlr_quat_t* quat) {
    rlr_quat_t new_quat;
    float norm = sqrt(quat->x*quat->x + quat->y*quat->y + quat->z*quat->z + quat->w*quat->w);
    new_quat.x = quat->w / norm;
    new_quat.y = quat->x / norm;
    new_quat.z = quat->y / norm;
    new_quat.w = quat->z / norm;
    return new_quat;
}