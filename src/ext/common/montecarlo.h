#ifndef MONTECARLO_H
#define MONTECARLO_H

#include "stdmath.h"
#include "vec.h"
#include "func.h"
#include "array.h"

template<typename R>
struct Distribution1D {
    sarray<R> func;
    sarray<R> cdf;
    R funcInt;
};

template<typename R>
inline void makeDistribution1D(Distribution1D<R> &dist, const sarray<R>& func) {
    dist.func = func;
    dist.cdf.resize(func.size()+1);
    
    dist.cdf[0] = 0;
    for (int i = 1; i < dist.cdf.size(); ++i) {
        dist.cdf[i] = dist.cdf[i-1] + dist.func[i-1] / dist.func.size();
    }
    
    dist.funcInt = dist.cdf[dist.func.size()];
    if (dist.funcInt == 0.f) {
        for (int i = 1; i < dist.cdf.size(); ++i) dist.cdf[i] = R(i) / R(dist.func.size());
    } else {
        for (int i = 1; i < dist.cdf.size(); ++i) dist.cdf[i] /= dist.funcInt;
    }
}

template<typename R>
inline R sampleDistribution1D(const Distribution1D<R>& dist, 
                          R u, R *pdf, int *off = 0) {
    const R* cdf = &dist.cdf[0];
    const R* ptr = std::upper_bound(cdf, cdf+dist.cdf.size(), u);
    int offset = max(0, int(ptr-cdf-1));
    if (off) *off = offset;
    error_if_not(offset < dist.func.size(), "incorrect cdf sampling");
    error_if_not(u >= cdf[offset] && u < cdf[offset+1], "incorrect cdf sampling");
    
    R du = (u - cdf[offset]) / (cdf[offset+1] - cdf[offset]);
    error_if_not(!isnan(du), "problem with du");
    
    if (pdf) *pdf = dist.func[offset] / dist.funcInt;
    
    return (offset + du) / dist.func.size();
}

template<typename R>
struct Distribution2D {
    sarray<Distribution1D<R> > conditionalV;
    Distribution1D<R> marginal;
};

template<typename R>
void makeDistribution2D(Distribution2D<R> &dist, const sarray2<R>& func) {
    dist.conditionalV.resize(func.height());
    sarray<R> condBuffer(func.width());
    for (int v = 0; v < func.height(); v++) {
        for(int u = 0; u < func.width(); u++)
            condBuffer.at(u) = func.at(u,v);
        makeDistribution1D(dist.conditionalV[v], condBuffer);
    }
    // Compute marginal sampling distribution $p[\tilde{v}]$
    sarray<R> marginalFunc(func.height());
    for (int v = 0; v < func.height(); ++v)
        marginalFunc[v] = dist.conditionalV[v].funcInt;
    makeDistribution1D(dist.marginal, marginalFunc);
}

template<typename R>
vec2f sampleDistribution2D(const Distribution2D<R>& dist, const vec2<R>& s, R *pdf) {
    vec2f pdfs;
    vec2f uv; int v;
    uv.y = sampleDistribution1D(dist.marginal, s.y, &pdfs.y, &v);
    uv.x = sampleDistribution1D(dist.conditionalV[v], s.x, &pdfs.x);
    *pdf = pdfs.x * pdfs.y;
    return uv;
}


template<typename R>
R pdfDistribution2D(const Distribution2D<R>& dist, const vec2f& uv) {
    int iu = clamp(int(uv.x * dist.conditionalV[0].func.size()), 0,
                   dist.conditionalV[0].func.size()-1);
    int iv = clamp(int(uv.y * dist.marginal.func.size()), 0,
                   dist.marginal.func.size()-1);
    if (dist.conditionalV[iv].funcInt * dist.marginal.funcInt == 0.f) return 0.f;
    return (dist.conditionalV[iv].func[iu] * dist.marginal.func[iv]) /
        (dist.conditionalV[iv].funcInt * dist.marginal.funcInt);
}

template<typename R>
inline R powerHeuristic(R fPdf, R gPdf) {
    R nf = 1; R ng = 1;
    R f = nf * fPdf; R g = ng * gPdf;
	return (f*f) / (f*f + g*g);
}

template<typename R>
inline int sampleUniform(R s, int size) {
    int l = clamp((int)(s * size), 0, size-1);
    return l;
}

template<typename R>
inline R sampleUniformPdf(int size) {
    return R(1) / size;
}

template<typename R>
inline R sampleHemisphericalCosPdf(const vec3<R>& w) {
    if(w.z <= 0) return 0;
    else return w.z/consts<R>::pi;
}

template<typename R>
inline vec3<R> sampleHemisphericalCos(const vec2<R>& ruv) {
    R z = sqrt(ruv.y);
    R r = sqrt(1-z*z);
    R phi = 2 * consts<R>::pi * ruv.x;
    return makevec3<R>(r*cos(phi), r*sin(phi), z);
}

template<typename R>
inline R sampleHemisphericalCosPowerPdf(const vec3<R>& w, R n) {
    if(w.z <= 0)return 0;
    else return pow(w.z,n) * (n+1) / (2*consts<R>::pi);
}

template<typename R>
inline vec3<R> sampleHemisphericalCosPower(const vec2<R>& ruv, R n) {
    R z = pow(ruv.y,1/(n+1));
    R r = sqrt(1-z*z);
    R phi = 2 * consts<R>::pi * ruv.x;
    return makevec3<R>(r*cos(phi), r*sin(phi), z);
}

template<typename R>
inline R sampleSphericalPdf(const vec3<R>& w) {
    return consts<R>::inv_fourpi;
}

template<typename R>
inline vec3<R> sampleSpherical(const vec2<R>& ruv) {
    R z = 2*ruv.y-1;
    R r = sqrt(1-z*z);
    R phi = 2 * consts<R>::pi * ruv.x;
    return makevec3<R>(r*cos(phi), r*sin(phi), z);
}

template<typename R>
inline R sampleHemisphericalPdf(const vec3<R>& w) {
    if(w.z <= 0 ) return 0;
    else return consts<R>::inv_twopi;
}

template<typename R>
inline vec3<R> sampleHemispherical(const vec2<R>& ruv) {
    R z = ruv.y;
    R r = sqrt(1-z*z);
    R phi = 2 * consts<R>::pi * ruv.x;
    return vec3<R>(r*cos(phi), r*sin(phi), z);
}

// from pbrt
template<typename R>
inline vec2<R> sampleConcentricDisk(const vec2<R> &ruv) {
    R r, theta;
    // Map uniform random numbers to $[-1,1]^2$
    R sx = 2 * ruv.x - 1;
    R sy = 2 * ruv.y - 1;

    // Map square to $(r,\theta)$
    // Handle degeneracy at the origin
    if (sx == 0 && sy == 0)
        return zero2f;
    if (sx >= -sy) {
        if (sx > sy) {
            // Handle first region of disk
            r = sx;
            if (sy > 0) theta = sy/r;
            else        theta = 8 + sy/r;
        }
        else {
            // Handle second region of disk
            r = sy;
            theta = 2 - sx/r;
        }
    }
    else {
        if (sx <= sy) {
            // Handle third region of disk
            r = -sx;
            theta = 4 - sy/r;
        }
        else {
            // Handle fourth region of disk
            r = -sy;
            theta = 6 + sx/r;
        }
    }
    theta *= consts<R>::pi / 4;
    return makevec2<R>(r * cos(theta), r * sin(theta));
}

#endif
