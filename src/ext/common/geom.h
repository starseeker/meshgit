#ifndef GEOM_H
#define GEOM_H

#include "stdmath.h"
#include "vec.h"
#include "ray.h"
#include "range.h"

template <typename R>
inline R distanceSqr(const vec3<R> &a, const vec3<R> &b) {
    return lengthSqr(a - b);
}

template<typename R>
inline vec3<R> triangleNormal(const vec3<R>& v0, 
                              const vec3<R>& v1, const vec3<R>& v2) {
	return normalize((v1-v0) ^ (v2-v0));
}

template<typename R>
inline void xyFromZ(const vec3<R>& z, vec3<R>& x, vec3<R>& y) {
    if (fabs(z.x) > fabs(z.y)) {
        float invLen = 1.f / sqrtf(z.x*z.x + z.z*z.z);
        x = makevec3f(-z.z * invLen, 0.f, z.x * invLen);
    }
    else {
        float invLen = 1.f / sqrtf(z.y*z.y + z.z*z.z);
        x = makevec3f(0.f, z.z * invLen, -z.y * invLen);
    }
    y = cross(z, x);
}

template<typename R>
inline bool intersectBoundingBox(const range3<R>& bbox, const ray3<R>& ray, 
                                 const vec3<R>& invDir) {
        R tmin, tmax;
		// implementation http://goo.gl/tsAZ8
		float tymin, tymax, tzmin, tzmax;
		if (invDir.x >= 0) {
			tmin = (bbox.min.x - ray.o.x) * invDir.x;
			tmax = (bbox.max.x - ray.o.x) * invDir.x;
		} else {
			tmin = (bbox.max.x - ray.o.x) * invDir.x;
			tmax = (bbox.min.x - ray.o.x) * invDir.x;
		}

		if (invDir.y >= 0) {
			tymin = (bbox.min.y - ray.o.y) * invDir.y;
			tymax = (bbox.max.y - ray.o.y) * invDir.y;
		} else {
			tymin = (bbox.max.y - ray.o.y) * invDir.y;
			tymax = (bbox.min.y - ray.o.y) * invDir.y;
		}
		if ( (tmin > tymax) || (tymin > tmax) ) return false;
		if (tymin > tmin) tmin = tymin;
		if (tymax < tmax) tmax = tymax;

		if (invDir.z >= 0) {
			tzmin = (bbox.min.z - ray.o.z) * invDir.z;
			tzmax = (bbox.max.z - ray.o.z) * invDir.z;
		} else {
			tzmin = (bbox.max.z - ray.o.z) * invDir.z;
			tzmax = (bbox.min.z - ray.o.z) * invDir.z;
		}
		if ((tmin > tzmax) || (tzmin > tmax)) return false;
		if (tzmin > tmin) tmin = tzmin;
		if (tzmax < tmax) tmax = tzmax;

		return ((tmin < ray.maxt) && (tmax > ray.mint));
}

template<typename R>
inline bool intersectBoundingBox(const range3<R>& bbox, const ray3<R>& ray, 
                                 const vec3<R>& invDir, R& mint, R& maxt) {
		mint = ray.mint; maxt = ray.maxt;
		vec3<R> neart = (bbox.min - ray.o) * invDir;
		vec3<R> fart = (bbox.max - ray.o) * invDir;
		if(neart.x > fart.x) { float aux = neart.x; neart.x = fart.x; fart.x = aux; }
		if(neart.y > fart.y) { float aux = neart.y; neart.y = fart.y; fart.y = aux; }
		if(neart.z > fart.z) { float aux = neart.z; neart.z = fart.z; fart.z = aux; }

		mint = (neart.x > mint) ? neart.x : mint;
		maxt = (fart.x < maxt) ? fart.x : maxt;
		if(mint > maxt) return false;

		mint = (neart.y > mint) ? neart.y : mint;
		maxt = (fart.y < maxt) ? fart.y : maxt;
		if(mint > maxt) return false;

		mint = (neart.z > mint) ? neart.z : mint;
		maxt = (fart.z < maxt) ? fart.z : maxt;
		if(mint > maxt) return false;
		return true;
}

template<typename R>
inline bool intersectBoundingBox(const range3<R>& bbox, 
                                 const ray3<R>& ray, R& tmin, R& tmax) {
	vec3<R> invDir = (R)1 / ray.d;
	return intersectBoundingBox(bbox, ray, invDir, tmin, tmax);
}

template<typename R>
inline bool intersectBoundingBox(const range3<R>& bbox, const ray3<R>& ray) {
	vec3<R> invDir = (R)1 / ray.d;
	return intersectBoundingBox(bbox, ray, invDir);
}

// TODO: check this for numerical precision issues
template<typename R>
inline bool intersectSphere(const vec3<R>& center, const R& radius, 
                            const ray3<R>& ray, float& t) {
    R a = lengthSqr(ray.d);
    R b = 2 * (ray.d % (ray.o - center));
    R c = lengthSqr(ray.o - center) - radius*radius;
    
    R d = b*b - 4*a*c;
    if(d <= 0.0f) return false;
    
    R tm = (-b - sqrt(d)) / (2*a);
    R tM = (-b + sqrt(d)) / (2*a);
    
    if (tm > ray.mint && tm < ray.maxt) {
        t = tm;
    } else if (tM > ray.mint && tM < ray.maxt) {
        t = tM;
    } else {
        return false;
    }
    
    return true;
}

template<typename R>
inline bool intersectSphere(const vec3<R>& center, const R& radius, 
                            const ray3<R>& ray, float& t, vec2f& uv) {
    bool hit = intersectSphere(center, radius, ray, t);
    if(hit) {
        vec3<R> v = (evalray3(ray,t) - center) / radius;
        uv.x = (atan2(v.x, v.y) + consts<R>::pi) / (2*consts<R>::pi);
        uv.y = acos(v.z) / consts<R>::pi;
    }
    return hit;
}

template<typename R>
inline bool intersectSegment(const vec3<R>& v0, const vec3<R>& v1, 
                             float r0, float r1,
                             const ray3<R>& ray) {
    vec3<R> axis = v1 - v0;
    float len = length(axis);
    axis /= len;
    
    vec3<R> rc = ray.o - v0;
    vec3<R> n = cross(ray.d, axis);
    float ln = length(n);
    n /= ln;
    
    float r = fabs(dot(n, rc));
    float u = ln / len;
    float radius = (1 - u) * r0 + u * r1;
    
    if (r > radius) return false;
    return true;
}

template<typename R>
inline bool intersectSegment(const vec3<R>& v0, const vec3<R>& v1, 
                          float r0, float r1,
                          const ray3<R>& ray, R& t, vec2<R>& uv) {
    vec3<R> axis = v1 - v0;
    float len = length(axis);
    axis /= len;
    
    vec3<R> rc = ray.o - v0;
    vec3<R> n = cross(ray.d, axis);
    float ln = length(n);
    n /= ln;
    
    float r = fabs(dot(n, rc));
    float u = ln / len;
    float radius = (1 - u) * r0 + u * r1;
    
    if (r > radius) return false;

    
    vec3f o = cross(rc, axis);
    float tt = -dot(o, n) / ln;
    o = cross(n, axis);
    o = normalize(o);
    
    float s = fabs(sqrtf(radius * radius - r * r) / dot(o, ray.d));
    
    float t0 = tt - s;
    float t1 = tt + s;
    
    bool cylinderHit = false;
    if(t0 > ray.mint && t0 < ray.maxt) {
        o = evalray3(ray, t0) - v0;
        ln = dot(o, axis);
        if (ln > 0 && ln < len) {
            t = t0;
            cylinderHit = true;
        }
    }
    
    if (t1 > ray.mint && t1 < ray.maxt && !cylinderHit) {
        o = evalray3(ray, t1) - v0;
        ln = dot(o, axis);
        if (ln > 0 && ln < len) {
            t = t1;
            cylinderHit = true;
        }
    }
    
    if(!cylinderHit) return false;
    
    //back face culling?
//    n = axis * (-ln) + o;
//    n = n / radius;
//    if (dot(n, ray.d) > 0) return false;
    
    uv = makevec2(u, (R)0.5);
    
    return true;
}

template<typename R>
inline bool intersectInfiniteSphere(const ray3<R>& ray, float& t, vec2f& uv) {
    bool hit = ray.maxt == ray3<R>::infinity;
    if(hit) {
        t = ray3<R>::infinity;
        vec3<R> v = normalize(ray.d);
        uv.x = (atan2(v.x, v.y) + consts<R>::pi) / (2*consts<R>::pi);
        uv.y = acos(v.z) / consts<R>::pi;        
    }
    return hit;
}
// from pbrt
template<typename R>
inline bool intersectTriangle(const vec3<R>& v0, const vec3<R>& v1, const vec3<R>& v2, 
                              const ray3<R>& ray, R& t, vec2<R>& uv) {
    // setup
    vec3<R> e1 = v1-v0;
    vec3<R> e2 = v2-v0;
    vec3<R> s1 = ray.d ^ e2;
    R divisor = s1 % e1;
    if(divisor == 0) return false;
    R invDivisor = 1/divisor;
    
    // first baricentric coord
    vec3<R> d = ray.o - v0;
    uv.x = (d % s1) * invDivisor;
    if(uv.x < 0 || uv.x > 1) return false;
    
    // second baricentric coord
    vec3<R> s2 = d ^ e1;
    uv.y = (ray.d % s2) * invDivisor;
    if(uv.y < 0 || uv.x + uv.y > 1) return false;
    
    // t
    t = (e2 % s2) * invDivisor;
    if(t < ray.mint || t > ray.maxt) return false;
    
    return true;
}

template<typename R>
inline bool intersectQuad(const R& width, const R& height, const ray3<R>& ray, 
                          R&t, vec2<R>& uv) {
	// not half width and height.
    if(ray.d.z == 0) return false;
    
    t = -ray.o.z/ray.d.z;
    if(t < ray.mint || t > ray.maxt) return false;
    
    uv.x = ((ray.o.x+ray.d.x*t)/width)+R(0.5);
    if(uv.x < 0 || uv.x > 1) return false;    
    
    uv.y = ((ray.o.y+ray.d.y*t)/height)+R(0.5);
    if(uv.y < 0 || uv.y > 1) return false;    
    
    return true;    
}

template<typename R>
inline range3<R> triangleBoundingBox(const vec3<R>& v0, const vec3<R>& v1, const vec3<R>& v2) {
	range3<R> ret = invalidrange3<R>();
    ret = runion(ret, v0); 
    ret = runion(ret, v1); 
    ret = runion(ret, v2); 
    return ret;
}
    
template<typename R>
inline range3<R> segmentBoundingBox(const vec3<R>& v0, const vec3<R>& v1, float r0, float r1) {
    range3<R> ret = invalidrange3<R>();
    const vec3<R> vr0 = makevec3(r0, r0, r0);
    const vec3<R> vr1 = makevec3(r1, r1, r1);
    const range3<R> b0 = makerange3(v0 - vr0, v0 + vr0);
    const range3<R> b1 = makerange3(v1 - vr1, v1 + vr1);
    ret = runion(ret, b0); 
    ret = runion(ret, b1); 
    return ret;
}

template<typename R>
inline R triangleArea(const vec3<R>& v0, const vec3<R>& v1, const vec3<R>& v2) {
	vec3<R> e0 = v1 - v0;
    vec3<R> e1 = v2 - v0;
    return length(e0 ^ e1) / 2;
}

template<typename R>
inline vec3<R> unitSphericalToDirection(const vec2<R>& angles) {
    const R& phi = angles.x;
    const R& theta = angles.y;
    R s = sin(theta); R c = cos(theta);
    return makevec3<R>(s * cos(phi), s * sin(phi), c);
}

template<typename R>
inline vec2<R> directionToUnitSpherical(const vec3<R>& w) {
    float p = atan2(w.y, w.x);
    float phi = (p < 0) ? p + consts<R>::twopi : p;
    float theta = acos(clamp(w.z, R(-1), R(1)));
    return makevec2<R>(phi,theta);
}

template<typename R>
inline vec3<R> latlongToDirection(const vec2<R>& uv) {
    return unitSphericalToDirection(makevec2f(uv.x * consts<R>::twopi, 
                                              uv.y * consts<R>::pi));
}

template<typename R>
inline vec2<R> directionToLatlong(const vec3<R>& w) {
    vec2<R> a = directionToUnitSpherical(w);
    return makevec2<R>(a.x * consts<R>::inv_twopi, a.y * consts<R>::inv_pi);
}

#endif
