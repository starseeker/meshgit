#ifndef _XFORM_H_
#define _XFORM_H_

#include "ext/common/vec.h"
#include "ext/common/ray.h"
#include "ext/common/range.h"

#include "object.h"
#include "mat.h"

// TODO: remove xform -> go to straight transforms

struct xform {
    mat4f   m  = identity_mat4f;
    mat4f   mi = identity_mat4f;
    bool    identity = true;
    
    xform() { }
    xform(const mat4f& m, const mat4f& mi, bool identity = false) : m(m), mi(mi), identity(identity) { }
    
    void init_afterload() {
        mi = inverse(m);
        identity = m == identity_mat4f;
    }
};

const xform identityxf = xform();

inline xform translation(const vec3f& t) { return xform(translate(t),translate(-t),t == zero3f); }
inline xform rotation(const vec3f& v, float a) { return xform(rotate(a,v),rotate(-a,v),a == 0); }
inline xform scaling(const vec3f& s) { return xform(scale(s),scale(1/s),s == one3f); }
inline xform operator*(const xform& a, const xform& b) { return xform(a.m*b.m,b.mi*a.mi,a.identity and b.identity); }

inline vec3f transform_point(const xform& xf, const vec3f& v) {
    if(xf.identity) return v;
    auto tv = xf.m * makevec4f(v.x,v.y,v.z,1.0f);
    return makevec3f(tv.x,tv.y,tv.z) / tv.w;
}
inline vec3f transform_vector(const xform& xf, const vec3f& v) { 
    if(xf.identity) return v;
    auto tv = xf.m * makevec4f(v.x,v.y,v.z,0.0f);
    return makevec3f(tv.x,tv.y,tv.z);
}
inline vec3f transform_direction(const xform& xf, const vec3f& v) {
    if(xf.identity) return v;
    return normalize(transform_vector(xf,v));
}
// TODO: inverse transpose
inline vec3f transform_normal(const xform& xf, const vec3f& v) { 
    if(xf.identity) return v; 
    return normalize(transform_vector(xf,v)); 
}
inline ray3f transform_ray(const xform& xf, const ray3f& r) { 
    if(xf.identity) return r; 
    return makeray3f(transform_point(xf,r.o),transform_vector(xf,r.d),r.mint,r.maxt);
}
inline range3f transform_bbox(const xform& xf, const range3f& b) { 
    if(xf.identity) return b;
    range3f bbox;
    bbox = runion(bbox, transform_point(xf,makevec3f(bbox.min.x,bbox.min.y,bbox.min.z)));
    bbox = runion(bbox, transform_point(xf,makevec3f(bbox.min.x,bbox.min.y,bbox.max.z)));
    bbox = runion(bbox, transform_point(xf,makevec3f(bbox.min.x,bbox.max.y,bbox.min.z)));
    bbox = runion(bbox, transform_point(xf,makevec3f(bbox.min.x,bbox.max.y,bbox.max.z)));
    bbox = runion(bbox, transform_point(xf,makevec3f(bbox.max.x,bbox.min.y,bbox.min.z)));
    bbox = runion(bbox, transform_point(xf,makevec3f(bbox.max.x,bbox.min.y,bbox.max.z)));
    bbox = runion(bbox, transform_point(xf,makevec3f(bbox.max.x,bbox.max.y,bbox.min.z)));
    bbox = runion(bbox, transform_point(xf,makevec3f(bbox.max.x,bbox.max.y,bbox.max.z)));
    return bbox;
}

inline vec3f transform_point_inverse(const xform& xf, const vec3f& v) {
    if(xf.identity) return v;
    auto tv = xf.mi * makevec4f(v.x,v.y,v.z,1.0f);
    return makevec3f(tv.x,tv.y,tv.z) / tv.w;
}
inline vec3f transform_vector_inverse(const xform& xf, const vec3f& v) { 
    if(xf.identity) return v;
    auto tv = xf.mi * makevec4f(v.x,v.y,v.z,0.0f);
    return makevec3f(tv.x,tv.y,tv.z);
}
// TODO: inverse transpose
inline vec3f transform_normal_inverse(const xform& xf, const vec3f& v) { 
    if(xf.identity) return v; 
    return normalize(transform_vector_inverse(xf,v)); 
}
inline ray3f transform_ray_inverse(const xform& xf, const ray3f& r) { 
    if(xf.identity) return r; 
    return makeray3f(transform_point_inverse(xf,r.o),transform_vector_inverse(xf,r.d),r.mint,r.maxt);
}
inline range3f transform_bbox_inverse(const xform& xf, const range3f& b) { 
    if(xf.identity) return b;
    range3f bbox;
    bbox = runion(bbox, transform_point_inverse(xf,makevec3f(bbox.min.x,bbox.min.y,bbox.min.z)));
    bbox = runion(bbox, transform_point_inverse(xf,makevec3f(bbox.min.x,bbox.min.y,bbox.max.z)));
    bbox = runion(bbox, transform_point_inverse(xf,makevec3f(bbox.min.x,bbox.max.y,bbox.min.z)));
    bbox = runion(bbox, transform_point_inverse(xf,makevec3f(bbox.min.x,bbox.max.y,bbox.max.z)));
    bbox = runion(bbox, transform_point_inverse(xf,makevec3f(bbox.max.x,bbox.min.y,bbox.min.z)));
    bbox = runion(bbox, transform_point_inverse(xf,makevec3f(bbox.max.x,bbox.min.y,bbox.max.z)));
    bbox = runion(bbox, transform_point_inverse(xf,makevec3f(bbox.max.x,bbox.max.y,bbox.min.z)));
    bbox = runion(bbox, transform_point_inverse(xf,makevec3f(bbox.max.x,bbox.max.y,bbox.max.z)));
    return bbox;
}

inline xform inverse(const xform& xf) { if (xf.identity) return xf; else return xform(xf.mi,xf.m); }

// TODO: cleanup
// TODO: animation interface?
struct Xform : object {
    virtual xform sample(float t = 0) = 0;
    
    //virtual range1f animate_interval() { return range1f(); }
};

struct StaticXform : Xform {
    xform xf;
    
    virtual xform sample(float t = 0) { return xf; }
};

/*
struct DynamicXform : Xform {
    vec3f ts = zero3f;
    Curve3f* tv = 0;
    vec3f ss = one3f;
    Curve3f* sv = 0;
    
    virtual xform sample(float t = 0) {
        vec3f tc = ts + ((tv) ? tv->p(t) : zero3f);
        vec3f sc = ss * ((sv) ? sv->p(t) : one3f);
        return translation(tc) * scaling(sc);
    }

    //virtual range1f animate_interval() { auto r = range1f(); if(tv) r = runion(r,tv->interval()); if(sv) r = runion(r,sv->interval()); return r; }
};
*/

#endif
