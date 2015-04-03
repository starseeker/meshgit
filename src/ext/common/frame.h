#ifndef FRAME_H
#define FRAME_H

#include "stdmath.h"
#include "vec.h"
#include "ray.h"
#include "range.h"

template <typename R>
struct frame3 {
	vec3<R> o, x, y, z;
};

template<typename R>
inline frame3<R> makeframe3(const vec3<R>& o, const vec3<R>& x, 
                            const vec3<R>& y, const vec3<R>& z) {
    frame3<R> f;
    f.o = o; 
    f.x = x;
    f.y = y;
    f.z = z;
    return f;
}

template<typename R>
inline frame3<R> translationframe3(const vec3<R>& t) {
    return makeframe3<R>(t, makevec3<R>(1,0,0), makevec3<R>(0,1,0), makevec3<R>(0,0,1));
}

template<typename R>
inline frame3<R> rotationframe3(const vec3<R>& axis, R angle) {
    vec3<R> v = normalize(axis);
	R c = cos(angle); R s = sin(angle);
    return makeframe3<R>(makevec3<R>(0,0,0), 
                         makevec3<R>(c+(1-c)*v.x*v.x,(1-c)*v.x*v.y+v.z*s,(1-c)*v.x*v.z-v.y*s), 
                         makevec3<R>((1-c)*v.x*v.y-v.z*s,c+(1-c)*v.y*v.y,(1-c)*v.y*v.z+v.x*s), 
                         makevec3<R>((1-c)*v.x*v.z+v.y*s,(1-c)*v.y*v.z-v.x*s,c+(1-c)*v.z*v.z));
}

template<typename R>
inline frame3<R> invert(const frame3<R>& f) {
    frame3<R> i = makeframe3<R>(zero3f, 
                                makevec3<R>(f.x.x,f.y.x,f.z.x), 
                                makevec3<R>(f.x.y,f.y.y,f.z.y), 
                                makevec3<R>(f.x.z,f.y.z,f.z.z));
    i.o = -transformPoint(i, f.o);
    return i;
}

template <typename R>
inline frame3<R> flipFrame(const frame3<R>& f) {
    frame3<R> ff;
    ff.o = f.o;
    ff.x = -f.x;
    ff.z = -f.z;
    return ff;
}

template<typename R>
inline vec3<R> transformVector(const frame3<R>& f, const vec3<R>& v) {
    return f.x * v.x + f.y * v.y + f.z * v.z;
}

template<typename R>
inline vec3<R> transformNormal(const frame3<R>& f, const vec3<R>& v) {
    return f.x * v.x + f.y * v.y + f.z * v.z;
}

template<typename R>
inline vec3<R> transformPoint(const frame3<R>& f, const vec3<R>& v) {
    return f.o + f.x * v.x + f.y * v.y + f.z * v.z;
}

template<typename R>
inline ray3<R> transformRay(const frame3<R>& f, const ray3<R>& r) {
    return makeray3<R>(transformPoint(f,r.o),transformVector(f,r.d),r.mint,r.maxt);
}

template<typename R>
inline vec3<R> transformVectorInverse(const frame3<R>& f, const vec3<R>& v) {
    return makevec3<R>(f.x % v, f.y % v, f.z % v);
}

template<typename R>
inline vec3<R> transformNormalInverse(const frame3<R>& f, const vec3<R>& v) {
    return makevec3<R>(f.x % v, f.y % v, f.z % v);
}

template<typename R>
inline vec3<R> transformPointInverse(const frame3<R>& f, const vec3<R>& v) {
    vec3<R> vv = v - f.o;
    return makevec3<R>(f.x % vv, f.y % vv, f.z % vv);
}

template<typename R>
inline ray3<R> transformRayInverse(const frame3<R>& f, const ray3<R>& r) {
    return makeray3<R>(transformPointInverse(f,r.o),transformVectorInverse(f,r.d),r.mint,r.maxt);
}

template<typename R>
inline frame3<R> transformFrame(const frame3<R>& f, const frame3<R>& v) {
    return makeframe3<R>(transformPoint(f,v.o),transformVector(f,v.x),
                         transformVector(f,v.y),transformVector(f,v.z));
}

template<typename R>
inline frame3<R> transformFrameInverse(const frame3<R>& f, const frame3<R>& v) {
    return makeframe3<R>(transformPointInverse(f,v.o),transformVectorInverse(f,v.x),
                         transformVectorInverse(f,v.y),transformVectorInverse(f,v.z));
}

// this is not a bijection, but conservative
template<typename R>
inline range3<R> transformRange(const frame3<R>& f, const range3<R>& b) {
    range3<R> ret = invalidrange3<R>();
    ret = runion(ret, transformPoint(f,makevec3f(b.min.x, b.min.y, b.min.z)));
    ret = runion(ret, transformPoint(f,makevec3f(b.min.x, b.min.y, b.max.z)));
    ret = runion(ret, transformPoint(f,makevec3f(b.min.x, b.max.y, b.min.z)));
    ret = runion(ret, transformPoint(f,makevec3f(b.min.x, b.max.y, b.max.z)));
    ret = runion(ret, transformPoint(f,makevec3f(b.max.x, b.min.y, b.min.z)));
    ret = runion(ret, transformPoint(f,makevec3f(b.max.x, b.min.y, b.max.z)));
    ret = runion(ret, transformPoint(f,makevec3f(b.max.x, b.max.y, b.min.z)));
    ret = runion(ret, transformPoint(f,makevec3f(b.max.x, b.max.y, b.max.z)));
    return ret;
}

// this is not a bijection, but conservative
template<typename R>
inline range3<R> transformRangeInverse(const frame3<R>& f, const range3<R>& b) {
    range3<R> ret = invalidrange3<R>();
    ret = runion(ret, transformPointInverse(f ,makevec3f(b.min.x, b.min.y, b.min.z)));
    ret = runion(ret, transformPointInverse(f ,makevec3f(b.min.x, b.min.y, b.max.z)));
    ret = runion(ret, transformPointInverse(f ,makevec3f(b.min.x, b.max.y, b.min.z)));
    ret = runion(ret, transformPointInverse(f ,makevec3f(b.min.x, b.max.y, b.max.z)));
    ret = runion(ret, transformPointInverse(f ,makevec3f(b.max.x, b.min.y, b.min.z)));
    ret = runion(ret, transformPointInverse(f ,makevec3f(b.max.x, b.min.y, b.max.z)));
    ret = runion(ret, transformPointInverse(f ,makevec3f(b.max.x, b.max.y, b.min.z)));
    ret = runion(ret, transformPointInverse(f ,makevec3f(b.max.x, b.max.y, b.max.z)));
    return ret;
}

typedef frame3<float> frame3f;
typedef frame3<double> frame3d;

inline frame3f makeframe3f(const vec3f& o, const vec3f& x, const vec3f& y, const vec3f& z) {
    return makeframe3<float>(o,x,y,z);
}

inline frame3f translationframe3f(const vec3f& t) {
    return translationframe3(t);
}

inline frame3f rotationframe3f(const vec3f& axis, float angle) {
    return rotationframe3(axis,angle);
}

const frame3f defaultframe3f = makeframe3f(makevec3f(0,0,0),
                                           makevec3f(1,0,0),
                                           makevec3f(0,1,0),
                                           makevec3f(0,0,1));
#endif
