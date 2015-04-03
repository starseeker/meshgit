#ifndef _GEOM_H_
#define _GEOM_H_

#include "ext/common/vec.h"
#include "ext/common/ray.h"
#include "ext/common/range.h"
#include "debug.h"

#include <algorithm>
#include <set>
#include <unordered_set>
#include <vector>

#define PIf 3.14159265f


using std::swap;
using namespace std;

inline vec3f triangle_normal(const vec3f& v0, const vec3f& v1, const vec3f& v2) {
    return normalize(cross(v1-v0,v2-v0));
}

inline float sphere_volume(float r) {
    return 4.0f/3.0f * PIf * r*r*r;
}

// from pbrt
inline bool intersect_bbox(const ray3f& ray, const range3f& bbox, float& t0, float& t1) {
    t0 = ray.mint; t1 = ray.maxt;
    
    const float *bm = &bbox.min.x;
    const float *bM = &bbox.max.x;
    const float *re = &ray.o.x;
    const float *rd = &ray.d.x;
    
    for (int i = 0; i < 3; ++i) {
        float invRayDir = 1.f / rd[i];
        float tNear = (bm[i] - re[i]) * invRayDir;
        float tFar  = (bM[i] - re[i]) * invRayDir;
        if (tNear > tFar) swap(tNear, tFar);
        t0 = tNear > t0 ? tNear : t0;
        t1 = tFar  < t1 ? tFar  : t1;
        if (t0 > t1) return false;
    }
    return true;
}

inline bool intersect_bbox(const ray3f& ray, const range3f& bbox) {
    float t0, t1;
    return intersect_bbox(ray,bbox,t0,t1);
}


inline float compute_signed_tetrahedron_volume( const vec3f &a, const vec3f &b, const vec3f &c ) {
    vec3f n = triangle_normal(a,b,c);
    float s = ((a%n) > 0.0f) ? 1.0f : -1.0f;
    float v = (1.0f/6.0f) * fabs(a%(b^c));
    return s*v;
}

inline float compute_triangle_area( const vec3f &v0, const vec3f &v1, const vec3f &v2 ) {
    return length(cross(v1-v0,v2-v0)) * 0.5f;
}

inline float compute_quad_area( const vec3f &v0, const vec3f &v1, const vec3f &v2, const vec3f &v3 ) {
    return compute_triangle_area(v0,v1,v2) + compute_triangle_area(v0,v2,v3);
}
float compute_triangle_area_in_sphere( const vec3f &v0, const vec3f &v1, const vec3f &v2, const vec3f &o, const float &r );
float compute_triangle_area_in_sphere( const vec3f &v0, const bool o0, const vec3f &v1, const bool o1, const vec3f &v2, const bool o2, const vec3f &o, const float &r );

float compute_mean_curvature_( const vec3f &v, const float &r, const vector<vec3f> &pos, const vector<vec3i> &faces, const unordered_set<int> &s_if );

inline bool intersect_triangle(const ray3f& ray, const
                               vec3f& v0, const vec3f& v1, const vec3f& v2,
                               float& t, float& ba, float& bb) {
    vec3f a = v0 - v2;
    vec3f b = v1 - v2;
    vec3f e = ray.o - v2;
    vec3f i = ray.d;
    
    float d = dot(cross(i,b),a);
    if(d == 0) return false;
    
    t =  dot(cross(e,a),b) / d;
    if(t < ray.mint or t > ray.maxt) return false;
    
    ba = dot(cross(i,b),e) / d;
    bb = dot(cross(a,i),e) / d;
    if(ba < 0 or bb < 0 or ba+bb > 1) return false;
    
    return true;
}

inline bool intersect_triangle(const ray3f& ray, const vec3f& v0, const vec3f& v1, const vec3f& v2) {
    float t, ba, bb;
    return intersect_triangle(ray, v0, v1, v2, t, ba, bb);
}

bool sameside( const vec3f &p1, const vec3f &p2, const vec3f &a, const vec3f &b);
bool is_point_inside( const vec3f &p0, const vec3f &p1, const vec3f &p2, const vec3f &p );
void get_uv_coords( const vec3f &p0, const vec3f &p1, const vec3f &p2, const vec3f &p, float &u, float &v );

inline bool intersect_sphere( const vec3f &v0, const vec3f &v1, const vec3f &o, const float &r, vec3f &v, bool no_behind=true ) {
    vec3f v10 = v1-v0;
    vec3f v0o = v0-o;
    float a = lengthSqr(v10);
    float b = 2.0f*dot(v10,v0o);
    float c = lengthSqr(v0o) - r*r;
    float d = b*b-4.0f*a*c;
    if( d < -0.000001f ) return false;              // no intersection
    float sqrtd = sqrt(max(0.0f,d));
    float tmin = (-b-sqrtd) / (2.0f*a);
    float tmax = (-b+sqrtd) / (2.0f*a);
    if( no_behind ) {
        if( tmax <= -0.00001f ) return false;           // hit behind
        if( tmin >= -0.00001f ) v = v0 + v10 * max(tmin,0.0f);
        else v = v0 + v10 * max(tmax,0.0f);
    } else {
        v = v0 + v10 * tmin;
    }
    return true;
}

inline bool intersect_sphere(const ray3f& ray, const vec3f& o, float r, float& t) {
    float a = lengthSqr(ray.d);
    float b = 2*dot(ray.d,ray.o-o);
    float c = lengthSqr(ray.o-o) - r*r;
    float d = b*b-4*a*c;
    if(d < 0) return false;
    float tmin = (-b-sqrt(d)) / (2*a);
    float tmax = (-b+sqrt(d)) / (2*a);
    if (tmin >= ray.mint && tmin <= ray.maxt) {
        t = tmin;
        return true;
    }
    if (tmax >= ray.mint && tmax <= ray.maxt) {
        t = tmax;
        return true;
    }
    return false;
}

inline bool intersect_sphere(const ray3f& ray, const vec3f& o, float r) {
    float t;
    return intersect_sphere(ray, o, r, t);
}

inline float bernstein(float u, int i, int d) {
    if(i < 0 or i > d) return 0;
    if(d == 0) return 1;
    else if(d == 1) {
        if(i == 0) return 1-u;
        else if(i == 1) return u;
    } else if(d == 2) {
        if(i == 0) return (1-u)*(1-u);
        else if(i == 1) return 2*u*(1-u);
        else if(i == 2) return u*u;
    } else if(d == 3) {
        if(i == 0) return (1-u)*(1-u)*(1-u);
        else if(i == 1) return 3*u*(1-u)*(1-u);
        else if(i == 2) return 3*u*u*(1-u);
        else if(i == 3) return u*u*u;
    } else {
        return (1-u)*bernstein(u, i, d-1) + u*bernstein(u, i-1, d-1);
    }
    return 0;
}

inline float bernstein_derivative(float u, int i, int d) {
    return d * (bernstein(u, i-1, d-1) - bernstein(u, i, d-1));
}

vec3f closest_point_point_triangle( const vec3f &p, const vec3f &v0, const vec3f &v1, const vec3f &v2 );


#endif
