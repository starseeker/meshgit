#ifndef RANDOM_ENGINE_H
#define RANDOM_ENGINE_H

// this code was adapted from TRNG: http://tina.berlios.de

// Copyright (c) 2000-2010, Heiko Bauke
// All rights reserved.
// 
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions
// are met:
// 
//   * Redistributions of source code must retain the above copyright
//     notice, this list of conditions and the following disclaimer.  
// 
//   * Redistributions in binary form must reproduce the above
//     copyright notice, this list of conditions and the following
//     disclaimer in the documentation and/or other materials provided
//     with the distribution.  
// 
//   * Neither the name of the copyright holder nor the names of its
//     contributors may be used to endorse or promote products derived
//     from this software without specific prior written permission.
// 
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
// FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
// COPYRIGHT HOLDERS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
// INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
// HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
// STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
// OF THE POSSIBILITY OF SUCH DAMAGE.

#include "stdmath.h"
#include "debug.h"

namespace random_engine {

template<bool shift>
class lcg64_fast_impl {
public:
    // Uniform random number generator concept
    typedef unsigned long long result_type;
    static const result_type min=0;
    static const result_type max=18446744073709551615ull;
    static const result_type a = 18145460002477866997ull;
    static const result_type b = 1ull;

    // Random number engine concept
    explicit lcg64_fast_impl() : S() { }
    explicit lcg64_fast_impl(unsigned long s) : S() { seed(s); }
    template<typename gen>
    explicit lcg64_fast_impl(gen &g) : S() { seed(g); }
    
    void seed() { (*this)=lcg64_fast_impl(); }
    void seed(unsigned long s) { S=s; }
    void seed(result_type s) { S=s; }
    template<typename gen>
    void seed(gen &g) {
        result_type r=0;
        for (int i=0; i<2; ++i) { r<<=32; r+=g(); }
        S=r;
    }
    
    result_type operator()() { 
        step(); 
        if(shift) {
            unsigned long long t=S;
            t^=(t>>17);
            t^=(t<<31);
            t^=(t>>8);
            return t;
        } else return S; 
    }
    
    bool operator==(const lcg64_fast_impl &R) { return S==R.S; }
    bool operator!=(const lcg64_fast_impl &R) { return !(S==R.S); }
    
    // void split(unsigned int, unsigned int);    
    void jump(unsigned long long s) {
        if (s<16) { for (unsigned int i = 0; i<s; ++i) step(); } 
        else {
            unsigned int i = 0;
            while (s>0) {
                if (s%2==1) jump2(i);
                ++i;
                s>>=1;
            }
        }
    }
    
private:
    result_type S;
    
    void step() { S=a*S+b; }

    // compute floor(log_2(x))
    static unsigned int log2_floor(result_type x) {
        unsigned int y(0);
        while (x>0) { x>>=1; ++y; }
        --y;
        return y;
    }
    // compute pow(x, n)
    static result_type pow(result_type x, result_type n) {
        result_type result=1;
        while (n>0) {
            if ((n&1)>0)
                result=result*x;
            x=x*x;
            n>>=1;
        }
        return result;
    }
    // compute prod(1+a^(2^i), i=0..l-1)
    static result_type g(unsigned int l) {
        result_type p=a, res=1;
        for (unsigned i=0; i<l; ++i) {
            res*=1+p;
            p*=p;
        }
        return res;
    }
    // compute sum(a^i, i=0..s-1)
    static result_type f(result_type s) {
        if (s==0) return 0;
        unsigned int l=log2_floor(s);
        result_type p=a;
        for (unsigned i=0; i<l; ++i) p*=p;
        return g(l)+p*f(s-(1ull<<l));
    }
 
    void jump2(unsigned int s) { S=S*pow(a, 1ull<<s)+f(1ull<<s); }
};

typedef lcg64_fast_impl<false> lcg64_fast;
typedef lcg64_fast_impl<true> lcg64_shift_fast;

template<bool shift>
class lcg64_impl {
public:
    // Uniform random number generator concept
    typedef unsigned long long result_type;
    result_type operator()() const; 
    static const result_type min=0;
    static const result_type max=18446744073709551615ull;
public:
    struct parameter_type {
        result_type a, b;
        parameter_type() : a(0), b(0) { };
        parameter_type(result_type a, result_type b) : a(a), b(b) { };
        bool operator==(const parameter_type &P) { return a==P.a&&b==P.b; }
    };
    
    struct status_type {
        result_type r;
        status_type() : r(0) { };
        explicit status_type(result_type r) : r(r) { };        
        bool operator==(const status_type &S) { return r==S.r; } 
    };
    
    static parameter_type getParams0() {
        return parameter_type(18145460002477866997ull, 1ull);
    }
    static parameter_type getParams1() {
        return parameter_type(2862933555777941757ull, 1ull);
    }
    static parameter_type getParams2() {
        return parameter_type(3202034522624059733ull, 1ull);
    }
    static parameter_type getParams3() {
        return parameter_type(3935559000370003845ull, 1ull);
    }
    
    // Random number engine concept
    explicit lcg64_impl(parameter_type P=getParams0()) : P(P), S() { }
    explicit lcg64_impl(unsigned long s, parameter_type P=getParams0()) : P(P), S() { seed(s); }
    template<typename gen>
    explicit lcg64_impl(gen &g, parameter_type P=getParams0()) : P(P), S() { seed(g); }
    
    void seed() { (*this)=lcg64_impl(); }
    void seed(unsigned long s) { S.r = s; } 
    template<typename gen>
    void seed(gen &g) {
        result_type r=0;
        for (int i=0; i<2; ++i) {
            r<<=32;
            r+=g();
        }
        S.r=r;
    }
    void seed(result_type s) { S.r = s; } 
    
    result_type operator()() {
        step();
        if(!shift) return S.r;
        else {
            unsigned long long t = S.r;
            t^=(t>>17);
            t^=(t<<31);
            t^=(t>>8);
            return t;
        }
    }
    
    // Equality comparable concept
    bool operator==(const lcg64_impl& R) { return P==R.P && S==R.S; }
    bool operator!=(const lcg64_impl& R) { return !operator==(R); }
    
    // Parallel random number generator concept
    void split(unsigned int s, unsigned int n) {
        error_if_not(!(s<1 || n>=s), "invalid argument for trng::lcg64::split");
        if (s>1) {
            jump(n+1);
            P.b*=f(s, P.a);
            P.a=pow(P.a, s);
            backward();
        }
    }
    
    void jump2(unsigned int s) {
        S.r=S.r*pow(P.a, 1ull<<s)+f(1ull<<s, P.a)*P.b;
    }
    
    void jump(unsigned long long s) {
        if (s<16) {
            for (unsigned int i(0); i<s; ++i) step();
        } else {
            unsigned int i(0);
            while (s>0) {
                if (s%2==1) jump2(i);
                ++i;
                s>>=1;
            }
        }
    }
    
private:
    parameter_type P;
    status_type S;
    
    void backward() {
        for (unsigned int i(0); i<64; ++i) jump2(i);
    }
    
    void step() {
        S.r=(P.a*S.r+P.b);
    }

    static unsigned int log2_floor(result_type x) {
        unsigned int y(0);
        while (x>0) {
            x>>=1;
            ++y;
        };
        --y;
        return y;
    }
    
    // compute pow(x, n)
    static result_type pow(result_type x, result_type n) {
        result_type result=1;
        while (n>0) {
            if ((n&1)>0) result=result*x;
            x=x*x;
            n>>=1;
        }
        return result;
    }
    
    // compute prod(1+a^(2^i), i=0..l-1)
    static result_type g(unsigned int l, result_type a) {
        result_type p=a, res=1;
        for (unsigned i=0; i<l; ++i) {
            res*=1+p;
            p*=p;
        }
        return res;
    }
    
    // compute sum(a^i, i=0..s-1)
    static result_type f(result_type s, result_type a) {
        if (s==0) return 0;
        unsigned int l=log2_floor(s);
        result_type p=a;
        for (unsigned i=0; i<l; ++i) p*=p;
        return g(l, a)+p*f(s-(1ull<<l), a);
    }
    
};

typedef lcg64_impl<false> lcg64;
typedef lcg64_impl<true> lcg64_shift;

namespace utility {
    inline long modulo_invers(long a, long m) {
        error_if_not(!(a<=0l || m<=1), "invalid argument in trng::utility::modulo_invers");
        long temp, q, flast(0), f(1), m1(m);
        while (a>1) {
            temp=m1%a;
            q=m1/a;
            m1=a;  a=temp;  temp=f;
            f=flast-q*f;
            flast=temp;
        }
        error_if_not(!(a==0), "no inversive in trng::utility::modulo_invers");
        return f>=0 ? f : f+m;
    }
    
    //------------------------------------------------------------------
    
    template<int n, int m>
    inline void gauss(long a[n*n], long b[n]) {
        int rank(0);
        long p[n];
        for (int i(0); i<n; ++i)
            p[i]=i;
        // make matrix triangular
        for (int i(0); i<n; ++i) {
            // search for a pivot element
            if (a[n*p[i]+i]==0l) {
                // swap rows
                int j=i+1;
                while (j<n && a[n*p[j]+i]==0l)
                    j++;
                if (j<n) {
                    long t=p[i];  p[i]=p[j];  p[j]=t;
                }
            }
            // is rank small?
            if (a[n*p[i]+i]==0l)
                break;
            ++rank;
            long t=modulo_invers(a[n*p[i]+i], m);
            for (int j=i; j<n; ++j)
                a[n*p[i]+j]=static_cast<long>
                ((static_cast<long long>(a[n*p[i]+j])*
                  static_cast<long long>(t))%m);
            b[p[i]]=static_cast<long>
            ((static_cast<long long>(b[p[i]])*
              static_cast<long long>(t))%m);
            for (int j=i+1; j<n; ++j) {
                if (a[n*p[j]+i]!=0l) {
                    t=modulo_invers(a[n*p[j]+i], m);
                    for (int k=i; k<n; ++k) {
                        a[n*p[j]+k]=
                        static_cast<long>
                        ((static_cast<long long>(a[n*p[j]+k])*
                          static_cast<long long>(t))%m);
                        a[n*p[j]+k]-=a[n*p[i]+k];
                        if (a[n*p[j]+k]<0l)
                            a[n*p[j]+k]+=m;
                    }
                    b[p[j]]=static_cast<long>
                    ((static_cast<long long>(b[p[j]])*
                      static_cast<long long>(t))%m);
                    b[p[j]]-=b[p[i]];
                    if (b[p[j]]<0l)
                        b[p[j]]+=m;
                }
            }
        }
        // test if a solution exists
        for (int i=rank; i<n; ++i)
            error_if_not(!(b[p[i]]!=0l),"equations system has no solution trng::utility::gauss");
        // solve triangular system
        for (int i=n-2; i>=0; --i)
            for (int j=i+1; j<n; ++j) {
                b[p[i]]-=static_cast<long>
                ((static_cast<long long>(a[n*p[i]+j])*
                  static_cast<long long>(b[p[j]]))%m);
                if (b[p[i]]<0l)
                    b[p[i]]+=m;
            }
        // sort
        for (int i(0); i<n; ++i) p[i]=b[p[i]];
        for (int i(0); i<n; ++i) b[i]=p[i];
    }
    
    //------------------------------------------------------------------
    
    template<int n>
    inline void matrix_mult(long a[n*n], long b[n*n],
                            long c[n*n], long m) {
        for (int i(0); i<n; ++i)
            for (int j(0); j<n; ++j) {
                long long t(0ll);
                for (int k(0); k<n; ++k) {
                    t+=(static_cast<long long>(a[j*n+k])*
                        static_cast<long long>(b[k*n+i]))%m;
                    if (t>=m)
                        t-=m;
                }
                c[j*n+i]=static_cast<long>(t);
            }
    }
    
    //------------------------------------------------------------------
    template<int n>
    inline void matrix_vec_mult(long a[n*n], long b[n],
                                long c[n], long m) {
        for (int j(0); j<n; ++j) {
            long long t(0ll);
            for (int k(0); k<n; ++k) {
                t+=(static_cast<long long>(a[j*n+k])*
                    static_cast<long long>(b[k]))%m;
                if (t>=m) t-=m;
            }
            c[j]=static_cast<long>(t);
        }
    }
    
    template<long m>
    struct log2_floor {
        enum { value = 1 + log2_floor<m/2>::value };
    };
    
    template<>
    struct log2_floor<0> { enum { value = 0 }; };
    
    template<>
    struct log2_floor<1> { enum { value = 0 }; };
    
    template<long m>
    struct log2_ceil {
        enum { value = (1ul<<log2_floor<m>::value) < m ? log2_floor<m>::value+1 : log2_floor<m>::value }; 
    };
    
    template<long m, long r> 
    class modulo_helper;
    
    template<long m> 
    class modulo_helper<m, 0> {
        static const long e=log2_ceil<m>::value;
        static const long k=(1ul<<e)-m;
        static const unsigned long mask=(1ul<<e)-1ul;
    public:
        inline static long modulo(unsigned long long x) {
            if (mask==m) {
                unsigned long y=(x&mask)+(x>>e);
                if (y>=m)
                    y-=m;
                return y;
            } else if (static_cast<long long>(k)*static_cast<long long>(k+2)<=m) {
                x=(x&mask)+(x>>e)*k;
                x=(x&mask)+(x>>e)*k;
                if (x>=m)
                    x-=m;
                return x;
            } else {
                return x%m;
            }
        }
    };
    
    template<long m> 
    class modulo_helper<m, 1> {
        static const long e=log2_ceil<m>::value;
        static const long k=(1ul<<e)-m;
        static const unsigned long mask=(1ul<<e)-1ul;
    public:
        inline static long modulo(unsigned long long x) {
            if (mask==m) {
                unsigned long long y=(x&mask)+(x>>e);
                if (y>=2ull*m)
                    y-=2ull*m;
                if (y>=m)
                    y-=m;
                return y;
            } else if (static_cast<long long>(k)*static_cast<long long>(k+2)<=m) {
                x=(x&mask)+(x>>e)*k;
                x=(x&mask)+(x>>e)*k;
                if (x>=2ull*m) x-=2ull*m; 
                if (x>=m)
                    x-=m;
                return x;
            } else {
                return x%m;
            }
        }
    };
    
    template<long m> 
    class modulo_helper<m, 2> {
        static const long e=log2_ceil<m>::value;
        static const long k=(1ul<<e)-m;
        static const unsigned long mask=(1ul<<e)-1ul;
    public:
        inline static long modulo(unsigned long long x) {
            if (mask==m) {
                unsigned long long y=(x&mask)+(x>>e);
                if (y>=4ull*m) y-=4ull*m;
                if (y>=2ull*m) y-=2ull*m;
                if (y>=m)
                    y-=m;
                return y;
            } else if (static_cast<long long>(k)*static_cast<long long>(k+2)<=m) {
                x=(x&mask)+(x>>e)*k;
                x=(x&mask)+(x>>e)*k;
                if (x>=4ull*m) x-=4ull*m; 
                if (x>=2ull*m) x-=2ull*m; 
                if (x>=m)
                    x-=m;
                return x;
            } else {
                return x%m;
            }
        }
    };
    
    template<long m, long r> 
    inline long modulo(unsigned long long x) {
        return modulo_helper<m, log2_floor<r>::value >::modulo(x);
    }
    
    template<long m, long b>
    class power {
        unsigned long b_power0[0x10000l], b_power1[0x08000l];
        
        inline long pow(long n) {
            long long p(1ll), t(b);
            while (n>0) {
                if ((n&0x1)==0x1)
                    p=(p*t)%m;
                t=(t*t)%m;
                n/=2;
            }
            return static_cast<long>(p);
        }
                
    public:
        power() {
            for (long i(0l); i<0x10000l; ++i)
                b_power0[i]=pow(i);
            for (long i(0l); i<0x08000l; ++i)
                b_power1[i]=pow(i*0x10000l);
        }
        inline long operator()(long n) const {
            return modulo<m, 1>(static_cast<unsigned long long>(b_power1[n>>16])*
                                static_cast<unsigned long long>(b_power0[n&0xffff]));
        }
        power & operator=(const power &);
        power(const power &);        
    };
    
}

template<int N, bool shift, bool mp>
class mrg_impl {
public:
    // Uniform random number generator concept
    typedef long result_type;
    static const result_type modulus=(!mp) ? 2147483647l : ((N!=5)?2147462579l:2147461007l);
    static const result_type gen=(!mp) ? 123567893l : ((N!=5)?1616076847l:889744251l);
    static const result_type min=0l;
    static const result_type max=modulus-1;
    static const utility::power<modulus,gen> g;
    
    struct parameter_type {
        result_type a[N];
        parameter_type() { for(int i=0;i<N;i++) a[i]=0; };
        parameter_type(result_type a_[N]) { for(int i=0;i<N;i++) a[i]=a_[i]; }
        parameter_type(result_type a1_, result_type a2_) {
            error_if_not(N == 2, "incorrect number of taps");
            result_type a_[2] = { a1_, a2_ };
            for(int i=0;i<N;i++) a[i]=a_[i];
        };        
        parameter_type(result_type a1_, result_type a2_, result_type a3_) {
            error_if_not(N == 3, "incorrect number of taps");
            result_type a_[3] = { a1_, a2_, a3_ };
            for(int i=0;i<N;i++) a[i]=a_[i];
        };        
        parameter_type(result_type a1_, result_type a2_, result_type a3_, result_type a4_) {
            error_if_not(N == 4, "incorrect number of taps");
            result_type a_[4] = { a1_, a2_, a3_, a4_ };
            for(int i=0;i<N;i++) a[i]=a_[i];
        };        
        parameter_type(result_type a1_, result_type a2_, result_type a3_, result_type a4_, result_type a5_) {
            error_if_not(N == 5, "incorrect number of taps");
            result_type a_[5] = { a1_, a2_, a3_, a4_, a5_ };
            for(int i=0;i<N;i++) a[i]=a_[i];
        };        
        bool operator==(const parameter_type& P) {
            for(int i=0;i<N;i++) if(a[i]!=P.a[i]) return false;
            return true;
        }
    };
    
    struct status_type {
        result_type r[N];
        status_type() { r[0]=0; for(int i=1;i<N;i++) r[i]=1; };
        status_type(result_type r_[N]) { for(int i=0;i<N;i++) r[i]=r_[i]; }
        status_type(result_type r1_, result_type r2_, result_type r3_) {
            result_type r_[N] = { r1_, r2_, r3_ };
            for(int i=0;i<N;i++) r[i]=r_[i];
        };        
        bool operator==(const status_type& S) {
            for(int i=0;i<N;i++) if(r[i]!=S.r[i]) return false;
            return true;
        }
    };
    
    static parameter_type getParams0() {
        if(N == 5) {
            if(!mp) return parameter_type(107374182l, 0l, 0l, 0l, 104480l);
            else return parameter_type(1053223373l, 1530818118l,	1612122482l, 133497989l, 573245311l);
        } else if(N == 4) {
            return parameter_type(2001982722l, 1412284257l, 1155380217l, 1668339922l);
        } else if(N == 3) {
            if(!mp) return parameter_type(2021422057l, 1826992351l, 1977753457l);
            else return parameter_type(2025213985l, 1112953677l, 2038969601l);
        } else if(N == 2) {
            return parameter_type(1498809829l, 1160990996l);
        } else {
            error("wrong number of taps"); return parameter_type();
        }
    }
    static parameter_type getParams1() {
        if(N == 5) {
            if(!mp) { error("wrong params"); return parameter_type(); }
            else return parameter_type(2068619238l, 2138332912l, 671754166l, 1442240992l, 1526958817l);
        } else if(N == 4) {
            return parameter_type(64886l, 0l, 0l, 64322l);
        } else if(N == 3) {
            if(!mp) return parameter_type(1476728729l, 0l, 1155643113l);
            else return parameter_type(1287767370l, 1045931779l, 58150106l);
        } else if(N == 2) {
            return parameter_type(46325l, 1084587l);
        } else {
            error("wrong number of taps"); return parameter_type();
        }
    }
    static parameter_type getParams2() {
        if(N == 4) {
            error("wrong params"); return parameter_type();
        } else if(N == 4) {
            error("wrong params"); return parameter_type();
        } else if(N == 3) {
            if(!mp) return parameter_type(65338l, 0l, 64636l);
            else { error("wrong params"); return parameter_type(); }
        } else if(N == 2) {
            error("wrong params"); return parameter_type();
        } else {
            error("wrong number of taps"); return parameter_type();
        }
    }
    
    // Random number engine concept
    explicit mrg_impl(parameter_type P =getParams0()) : P(P), S() { }
    explicit mrg_impl(unsigned long s, parameter_type P=getParams0()) : P(P), S() { seed(s); }
    template<typename gen>
    explicit mrg_impl(gen &g, parameter_type P=getParams0()) : P(P), S() { seed(g); }
    
    void seed() { (*this)=mrg_impl(); }
    void seed(unsigned long s) {
        long long t=s;
        t%=modulus;
        if (t<0) t+=modulus;
        S.r[0]=static_cast<result_type>(t);
        for(int i=1; i < N; i ++) S.r[i]=1;
    }
    
    template<typename gen>
    void seed(gen &g) {
        for(int i = 0; i < N; i ++) {
            result_type r = static_cast<unsigned long>(g())%
                            static_cast<unsigned long>(modulus);
            S.r[i]=r;
        }
    }
    void seed(result_type s[N]) {
        for(int i = 0; i < N; i ++) {
            S.r[i]=s[i]%modulus;
            if (S.r[i]<0) S.r[i]+=modulus;
        }
    }
    
    result_type operator()() { 
        step(); 
        if(shift) { return (S.r[0]==0) ? 0 : g(S.r[0]); }
        else return S.r[0]; 
    }
    
    // Equality comparable concept
    bool operator==(const mrg_impl& R) { return P==R.P && S==R.S; }
    bool operator!=(const mrg_impl& R) { return !operator==(R); }
    
    // Parallel random number generator concept
    void split(unsigned int s, unsigned int n) {
        error_if_not(!(s<1 || n>=s), "invalid argument for trng::mrg3::split");
        if (s>1) {
            long q[2*N];
            jump(n+1);  q[0]=S.r[0];
            for(int i = 1; i < 2*N; i ++) { jump(s); q[i]=S.r[0]; }
            long a[N], b[N*N];
            for(int i = 0; i < N; i ++) a[i]=q[i+N];
            for(int j = 0; j < N; j ++) {
                for(int i = 0; i < N; i ++) {
                    b[j*N+i] = q[j+N-1-i];
                }
            }
            utility::gauss<N,modulus>(b, a);
            for(int i = 0; i < N; i ++) P.a[i]=a[i];
            for(int i = 0; i < N; i ++) S.r[i]=q[N-1-i];
            for(int i = 0; i < N; i ++) backward();
        }
    }

    void jump2(unsigned int s) {
        long b[N*N], c[N*N], d[N], r[N];
        for(int j = 0; j < N; j ++) {
            for(int i = 0; i < N; i ++) {
                if(j == 0) b[j*N+i]=P.a[i];
                else b[j*N+i] = (j-1==i)?1:0;
            }
        }
        for (unsigned int i = 0; i<s; ++i) {
            if ((i&1)==0) utility::matrix_mult<N>(b, b, c, modulus);
            else utility::matrix_mult<N>(c, c, b, modulus);
        }
        for(int i = 0; i < N; i ++) r[i]=S.r[i];
        if ((s&1)==0) utility::matrix_vec_mult<N>(b, r, d, modulus);
        else utility::matrix_vec_mult<N>(c, r, d, modulus);
        for(int i = 0; i < N; i ++) S.r[i]=d[i];
    }
    
    void jump(unsigned long long s) {
        if (s<16) {
            for (unsigned int i(0); i<s; ++i) step();
        } else {
            unsigned int i(0);
            while (s>0) {
                if (s%2==1) jump2(i);
                ++i;
                s>>=1;
            }
        }
    }
    
private:
    parameter_type P;
    status_type S;
    
    void backward() {
        long t = 0;
        for(int i = 0; i < N; i ++) {
            if (P.a[N-1-i]!=0l) {
                t=S.r[i]; int skip=i+1;
                for(int j = 0; j+skip < N; j ++) {
                    t-=static_cast<long>((static_cast<long long>(P.a[j])*
                                          static_cast<long long>(S.r[j+skip]))%modulus);
                    if (t<0l) t+=modulus;
                }
                t=static_cast<long>((static_cast<long long>(t)*
                                     static_cast<long long>(utility::modulo_invers(P.a[N-1-i], modulus)))%modulus);
                break;
            }
        }
        for(int i = 0; i < N-1; i ++) S.r[i]=S.r[i+1];
        S.r[N-1]=t;
    }
    
    void step() {
        unsigned long long t = 0;
        for(int i = 0; i < N; i ++) {
            t += static_cast<unsigned long long>(P.a[i])*
                 static_cast<unsigned long long>(S.r[i]);
            if(N == 5 && i == 3) {
                if (t>=2ull*modulus*modulus) t-=2ull*modulus*modulus;
            }
        }
        for(int i = N-1; i > 0; i --) {
            S.r[i]=S.r[i-1];  
        }
        S.r[0]=utility::modulo<modulus, N>(t);
    }
};

typedef mrg_impl<2, false, false> mrg2;
typedef mrg_impl<2, true, false> yarn2;
typedef mrg_impl<3, false, false> mrg3;
typedef mrg_impl<3, true, false> yarn3;
typedef mrg_impl<3, false, true> mrg3s;
typedef mrg_impl<3, true, true> yarn3s;
typedef mrg_impl<4, false, false> mrg4;
typedef mrg_impl<4, true, false> yarn4;
typedef mrg_impl<5, false, false> mrg5;
typedef mrg_impl<5, true, false> yarn5;
typedef mrg_impl<5, false, true> mrg5s;
typedef mrg_impl<5, true, true> yarn5s;

template<typename float_t, typename engine_t>
float_t uniform01(engine_t& r) {
    typename engine_t::result_type rr = r()-engine_t::min;
    typename engine_t::result_type dm = engine_t::max-engine_t::min;
    float_t de = float_t(1)/dm;
    float_t ne = numeric_limits<float_t>::epsilon();
    float_t eps = (ne>=de) ? ne : de;
    return float_t(rr) * ((1-eps)*(1/float_t(dm)));
}

template<typename float_t, typename engine_t>
float_t uniform(engine_t& r, float_t m, float_t M) {
    return (M-m)*uniform01(r)+m;
}

template<typename int_t, typename engine_t>
int_t uniform_int(engine_t& r, int_t m, int_t M) {
    return int_t((M-m)*uniform01<double>(r)+m);
}

};

#endif
