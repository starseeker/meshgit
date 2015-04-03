#ifndef RANDOM_H
#define RANDOM_H

#include "vec.h"
#include "array.h"
#include "debug.h"
#include "random_engine.h"

#ifdef WIN32
#include <random>
using std::minstd_rand0;
using std::uniform_int;
#elif __cplusplus == 201103L
// http://gcc.gnu.org/gcc-4.7/changes.html
#include <random>
using std::minstd_rand0;
using std::uniform_int_distribution; //<>:uniform_int;
#else
//#elif __cplusplus == 199711L
#include <tr1/random>
using std::tr1::minstd_rand0
using std::tr1::uniform_int;
//#else
//#include <tr1/random>
//#include <tr1/uniform_int.hpp>
//#include <tr1/linear_congruential.hpp>
#endif

struct Rng {
    random_engine::lcg64_fast engine;
    
    Rng(unsigned long s = 0) {
        engine.seed(s);
    }
    
    float  next1f() { return random_engine::uniform01<float>(engine); }    
    double next1d() { return random_engine::uniform01<double>(engine); }   
    vec2f next2f() {
        float f1 = next1f();
        float f2 = next1f();
        return makevec2f(f1,f2); 
    }
    vec2d next2d() {
        double f1 = next1d();
        double f2 = next1d();
        return makevec2d(f1,f2); 
    }
    vec3f next3f() {
        float f1 = next1f();
        float f2 = next1f();
        float f3 = next1f();
        return makevec3f(f1,f2,f3);
    }
    vec3d next3d() {
        double d1 = next1d();
        double d2 = next1d();
        double d3 = next1d();
        return makevec3d(d1,d2,d3);
    }
    void jump(unsigned long long s) { engine.jump(s); }
    
    int next1i(int min, int max) {
        return random_engine::uniform_int(engine,min,max+1);
    }
};

// not thread safe
inline unsigned int nextRandomSeed() {
    static minstd_rand0 seeder;
    static uniform_int<unsigned long> distu(0,
        numeric_limits<unsigned long>::max()-1);
    return distu(seeder);
}

// stratified sample 2d
inline void stratify2d(vec2f* v, int n1, int n2, Rng& rng) {
    vec2f ss = makevec2f(1.0f/n1,1.0f/n2);
    for(int j = 0; j < n2; j ++) {
        for(int i = 0; i < n1; i ++) {
            v[j*n1+i] = (makevec2f((float)i,(float)j) + rng.next2f()) * ss;
        }
    }
}

// stratified sample 1d
inline void stratify1d(float* v, int n, Rng& rng) {
    float ss = 1.0f/n;
    for(int i = 0; i < n; i ++) {
        v[i] = (i + rng.next1f()) * ss;
    }
}

template<class T>
inline void scramble(T* v, int n, Rng& rng) {
    for(int i = 0; i < n; i ++) {
        int j = rng.next1i(0,n-1);
        std::swap(v[i], v[j]);
    }
}

template<class T>
inline void scramble(sarray<T> &v, Rng& rng) {
    int n = v.size();
    for(int i = 0; i < n; i ++) {
        int j = rng.next1i(0,n-1);
        std::swap(v[i], v[j]);
    }
}

#ifdef WIN32
#pragma warning( push )
#pragma warning( disable : 4244 )
#endif
// from pbrt implementation
// TODO: check atuo conversion to double - is it right?
inline double qmcRadicalInverse(unsigned int n, unsigned int base) {
    double val = 0;
    double invBase = 1. / base, invBi = invBase;
    while (n > 0) {
        // Compute next digit of radical inverse
        int d_i = (n % base);
        val += d_i * invBi;
        n = (unsigned int)(n*invBase); // TODO: check this here
        invBi *= invBase;
    }
    return val;
}

// from pbrt implementation
inline double qmcPermutedRadicalInverse(uint32_t n, uint32_t base,
                                        const uint32_t *p) {
    double val = 0;
    double invBase = 1. / base, invBi = invBase;
    
    while (n > 0) {
        uint32_t d_i = p[n % base];
        val += d_i * invBi;
        n = (uint32_t)(n*invBase); // TODO: check type conversion
        invBi *= invBase;
    }
    return val;
}
#ifdef WIN32
#pragma warning( pop ) 
#endif

extern int _primes[]; // the first n primes
inline double qmcHalton(unsigned int n, unsigned int d) {
    error_if_not(d < 1000, "only 1000 primes available");
    return qmcRadicalInverse(n, _primes[d]);
}

extern unsigned int _sobolV32[1024][33];
inline double qmcSobol32(unsigned int n, unsigned int d) {
    unsigned int x = 0;
    // avoid grey code ordering
    // n = n ^ (n >> 1); // go to gray code
    for(unsigned int k = 0; n; n >>= 1, k++) {
        if(n & 1) x ^= _sobolV32[d][k+1];
    }
    return (double)x/pow(2.0,32);
}
inline double qmcScrambledSobol32(unsigned int n, unsigned int d, unsigned int s) {
    unsigned int x = s;
    // avoid grey code ordering
    // n = n ^ (n >> 1); // go to gray code
    for(unsigned int k = 0; n; n >>= 1, k++) {
        if(n & 1) x ^= _sobolV32[d][k+1];
    }
    return (double)x/pow(2.0,32);
}

extern unsigned long long _sobolV64[1024][65];
inline double qmcSobol64(unsigned long long n, unsigned int d) {
    unsigned long long x = 0;
    // avoid grey code ordering
    // n = n ^ (n >> 1); // go to gray code
    for(unsigned int k = 0; n; n >>= 1, k++) {
        if(n & 1) x ^= _sobolV64[d][k+1];
    }
    return (double)x/pow(2.0,64);
}
inline double qmcScrambledSobol64(unsigned long long n, unsigned int d, unsigned long long s) {
    unsigned long long x = s;
    // avoid grey code ordering
    // n = n ^ (n >> 1); // go to gray code
    for(unsigned int k = 0; n; n >>= 1, k++) {
        if(n & 1) x ^= _sobolV64[d][k+1];
    }
    return (double)x/pow(2.0,64);
}

// from pbrt
inline unsigned int qmcVanDerCorput32u(unsigned int n) {
    // Reverse bits of _n_
    n = (n << 16) | (n >> 16);
    n = ((n & 0x00ff00ff) << 8) | ((n & 0xff00ff00) >> 8);
    n = ((n & 0x0f0f0f0f) << 4) | ((n & 0xf0f0f0f0) >> 4);
    n = ((n & 0x33333333) << 2) | ((n & 0xcccccccc) >> 2);
    n = ((n & 0x55555555) << 1) | ((n & 0xaaaaaaaa) >> 1);
    return n;
}
inline unsigned int qmcSobol232u(unsigned int n) {
    unsigned int x = 0;
    for (unsigned int v = 1u << 31; n; n >>= 1, v ^= v >> 1)
        if (n & 1) x ^= v;
    return x;
}

#endif
