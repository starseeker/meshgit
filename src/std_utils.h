#ifndef _STD_UTILS_H_
#define _STD_UTILS_H_

#include "std.h"
#include <math.h>
#include <stdio.h>
#include <cstdarg>
#include <string>
#include <vector>

using namespace std;

struct timer {
    using _clock = high_resolution_clock;
    using _time_point = _clock::time_point;
    
    _time_point _start_time;

    timer() { start(); }
    
    void start() { _start_time = _now(); }

    double operator()() { return elapsed(); }
    double elapsed( const double fps=1.0 ) const {
        return fps * duration_cast<microseconds>(_now()-_start_time).count() / 1000000.0f;
    }
    
    inline double elapsed_saw( const double fps=1.0, const bool reversed=false ) const {
        double t = elapsed(fps);
        double s = 2.0 * ( t - floor(0.5 + t) );
        if( reversed ) return 1.0 - s;
        return s;
    }
    
    inline double elapsed_triangle( const double fps=1.0 ) const {
        double t = elapsed(fps);
        return fabs(2.0*( t - floor(t + 0.5) ));
    }
    
    inline double elapsed_sine( const double fps=1.0 ) const {
        double t = elapsed(fps);
        return sin( t * 3.14159f ) * 0.5 + 0.5;
    }
    
    static _time_point _now() { return _clock::now(); }
};

string to_string(const char* fmt, int value);
string to_string(const char* fmt, float value);
string to_string(const char* fmt, double value);
inline static string to_string(const char *fmt, ...) {
    char buf[1024]; va_list args;
    va_start(args, fmt); vsprintf(buf, fmt, args ); va_end(args);
    return buf;
}


struct timer_avg {
    timer _t;
    double _elapsed = 0;
    unsigned long int _count = 0;

    void start() { _t.start(); }
    void stop() { _elapsed += _t.elapsed(); _count ++; }
    void reset() { _elapsed = 0; _count = 0; }
    
    double elapsed() { return (_count) ? _elapsed / _count : 0; }
};

void sorted_del_add( vector<int> &l, const vector<int> &l_d, const vector<int> &l_a );

void shuffled_indices( int size, vector<int> &l_inds );

template<typename T>
inline void set_to_sorted_list( unordered_set<T> &s, vector<T> &l, function<bool(const T &lhs,const T&rhs)> fn_sort ) {
    l.assign( s.begin(), s.end() ); sort( l.begin(), l.end(), fn_sort );
}

template<typename T>
inline void set_to_list( unordered_set<T> &s, vector<T> &l ) { l.assign(s.begin(),s.end()); }

template<typename T>
inline T pop( unordered_set<T> &s ) { T f = *s.begin(); s.erase(f); return f; }

template<typename T>
inline T pop_back( vector<T> &l ) { T v = l.back(); l.pop_back(); return v; }






#endif
