#ifndef _QUATERNION_H_
#define _QUATERNION_H_

#include "stdmath.h"
#include "vec.h"

template<typename R>
struct quaternion {
    R x;
    R y;
    R z;
    R w;
};

template <typename R>
quaternion<R> makequaternion3(R x, R y, R z, R w) {
    quaternion<R> q;
    q.x = x;
    q.y = y;
    q.z = z;
    q.w = w;
    return q;
}

template <typename R>
quaternion<R> toquaternion(const vec3<R> & axis, R angle) {
    quaternion<R> q;
    R s = sin(angle * 0.5f);
    q.w = cos(angle * 0.5f);
    q.x = axis.x * s;
    q.y = axis.y * s;
    q.z = axis.z * s;
    return q;
}

template <typename R>
frame3<R> toframe(const quaternion<R> &q) {
    float xx = q.x * q.x, yy = q.y * q.y, zz = q.z * q.z;
    float xy = q.x * q.y, xz = q.x * q.z, yz = q.y * q.z;
    float wx = q.x * q.w,   wy = q.y * q.w, wz = q.z * q.w;
    frame3<R> f;
    f.o = makevec3<R>(0, 0, 0);
    f.x = makevec3<R>(1.0 - 2.0 * (yy + zz), 2.0 * (xy + wz),       2.0 * (xz - wy));
    f.y = makevec3<R>(2.0 * (xy - wz),       1.0 - 2.0 * (xx + zz), 2.0 * (yz + wx));
    f.z = makevec3<R>(2.0 * (xz + wy),       2.0 * (yz - wx),       1.0 - 2.0 * (xx + yy));
    return f;
};

template <typename R>
R dot(const quaternion<R> &a, const quaternion<R> &b) {
    return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
}

template <typename R>
quaternion<R> operator/(const quaternion<R> &qq, R l) {
    quaternion<R> q;
    q.x = qq.x / l; q.y = qq.y / l; q.z = qq.z / l; q.w = qq.w / l;
    return q;
}

template <typename R>
quaternion<R> operator*(const quaternion<R> &qq, float f) {
    quaternion<R> q;
    q.x = qq.x * f; q.y = qq.y * f; q.z = qq.z * f; q.w = qq.w * f;
    return q;
}

template <typename R>
quaternion<R> operator+(const quaternion<R> &q1, const quaternion<R> &q2) {
    quaternion<R> q;
    q.x = q1.x + q2.x; q.y = q1.y + q2.y; q.z = q1.z + q2.z; q.w = q1.w + q2.w;
    return q;
}

template <typename R>
quaternion<R> operator-(const quaternion<R> &q1, const quaternion<R> &q2) {
    quaternion<R> q;
    q.x = q1.x - q2.x; q.y = q1.y - q2.y; q.z = q1.z - q2.z; q.w = q1.w - q2.w;
    return q;
}

template <typename R>
quaternion<R> normalize(const quaternion<R> &qq) {
    R l = sqrt(dot(qq, qq));
    if(l != 0) return qq / l;
    else return qq;
}

template <typename R>
quaternion<R> slerp(R t, const quaternion<R> &q1, const quaternion<R> &q2) {
    float cost = dot(q1, q2);
    if (cost > .9995f) return normalize(q1 * (1.0 - t) + q2 * t);
    else {
        float theta = acos(clamp(cost, (R)-1, (R)1));
        float thetap = theta * t;
        quaternion<R> qperp = normalize(q2 - q1 * cost);
        return q1 * cos(thetap) + qperp * sin(thetap);
    }
}


typedef quaternion<float> quaternionf;
typedef quaternion<double> quaterniond;

#endif // _QUATERNION_H_
