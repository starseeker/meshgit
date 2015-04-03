#ifndef _COLOR_GRADIENT_H_
#define _COLOR_GRADIENT_H_

#include "debug.h"
#include "std.h"
#include "ext/common/vec.h"


struct ColorGradient {
    vector<vec3f> l_colors;
    
    vec3f get_color( float v, float minv, float maxv ) {
        v = min( maxv, max( minv, v ) );
        float p = (v-minv) / (maxv-minv) * (float)(l_colors.size()-1);
        p = min( p, (float)(l_colors.size()-1.0f) );
        float pf = floor(p);
        float pid = p - pf;
        int i0 = (int)pf; i0 = (i0<0)?0:((i0>=l_colors.size())?l_colors.size()-1:i0);
        int i1 = i0+1; i1 = (i1<0)?0:((i1>=l_colors.size())?l_colors.size()-1:i1);
        return l_colors[i0] * (1.0f-pid) + l_colors[i1] * pid;
    }
};

// NOT ROBUST!
struct ColorGradient_NonLinear {
    vector<vec3f> l_colors;
    vector<float> l_vals;
    
    void add( float v, vec3f c ) {
        error_if_not( l_vals.size() == 0 || v > l_vals.back(), "vals must be inserted in order!" );
        l_vals.push_back(v); l_colors.push_back(c);
    }
    
    vec3f get_color( float v ) {
        v = min( l_vals.back(), max( l_vals.front(), v ) );
        int i = 0;
        while( i<l_vals.size()-1 && v>l_vals[i+1] ) i++;
        vec3f c0 = l_colors[i+0]; float v0 = l_vals[i+0];
        vec3f c1 = l_colors[i+1]; float v1 = l_vals[i+1];
        float p = (v - v0) / (v1 - v0);
        return c0 * (1.0f-p) + c1*p;
    }
};

#endif
