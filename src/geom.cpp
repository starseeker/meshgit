#include "geom.h"
#include "mesh_utils.h"


using namespace std;


float compute_triangle_area_in_sphere_edge1( const vec3f &v0, const vec3f &v1, const vec3f &v2, const vec3f &o, const float &r ) {
    float a = compute_triangle_area(v0,v1,v2);
    if( a < 0.01f ) return 0.0f;
    
    vec3f v02 = v0, v12 = v1;
    intersect_sphere(v2,v0,o,r,v02);
    intersect_sphere(v2,v1,o,r,v12);
    
    bool i02 = lengthSqr(v02-v0) > 0.0001f;
    bool i12 = lengthSqr(v12-v1) > 0.0001f;
    
    if( i02 && i12 ) {
        a = compute_quad_area(v0,v1,v12,v02);
        a += compute_triangle_area_in_sphere_edge1(v12,v02,v2,o,r);
    } else if( i02 ) {
        a = compute_triangle_area(v0,v1,v02);
        a += compute_triangle_area_in_sphere_edge1(v02,v1,v2,o,r);
    } else if( i12 ) {
        a = compute_triangle_area(v0,v1,v12);
        a += compute_triangle_area_in_sphere_edge1(v0,v12,v2,o,r);
    } else {
        vec3f vm = (v0+v1)/2.0f;
        vec3f v10 = v1-v0, v20 = v2-v0;
        vec3f vp = normalize(cross(normalize(cross(v10,v20)),v10));
        vec3f v3;
        error_if_not_va( intersect_sphere(vm,vm+vp,o,r,v3), "sphere_edge: no hit!" );
        a = compute_triangle_area(v0,v1,v3);
        a += compute_triangle_area_in_sphere_edge1(v0,v3,v2+(v0-v1),o,r);
        a += compute_triangle_area_in_sphere_edge1(v3,v1,v2+(v1-v0),o,r);
    }
    
    return a;
}
inline float compute_circle_segment_area( const vec3f &c, const float &r, const vec3f &v0, const vec3f &v1 ) {
    vec3f v0c = normalize(v0-c), v1c = normalize(v1-c);
    float d = min( 1.0f, dot(v0c,v1c) );
    float theta = acosf(d);
    float a = 0.5f * (theta-sinf(theta)) * r * r;
    error_if_not_va( a >= -0.01f, "negative? %f", a );
    return max(0.0f,a);
}
inline float compute_pie_area( const vec3f &c, const float &r, const vec3f &v0, const vec3f &v1 ) {
    vec3f v0c = normalize(v0-c), v1c = normalize(v1-c);
    return acosf(dot(v0c,v1c)) * r * r;
}

inline float compute_piecrust_area( const vec3f &c, const float &r, const vec3f &v0, const vec3f &v1, bool inside=false ) {
    if( lengthSqr(v0-v1) < 0.0001f ) return inside ? PIf*r*r : 0.0f;
    float a = compute_circle_segment_area(c,r,v0,v1);
    if( !inside ) return a;
    return PIf*r*r - a;
}

inline bool sameside( const vec3f &p1, const vec3f &p2, const vec3f &a, const vec3f &b ) {
    return dot( cross(b-a,p1-a)*10000.0f, cross(b-a,p2-a)*10000.0f ) >= -0.00001f;
}
inline bool is_point_inside( const vec3f &p0, const vec3f &p1, const vec3f &p2, const vec3f &p ) {
    float abc = compute_triangle_area(p0,p1,p2);
    if( abc < 0.0001f ) {
        return sameside(p,p0,p1,p2) && sameside(p,p1,p0,p2) && sameside(p,p2,p0,p1);
    } else {
        float a = compute_triangle_area(p0,p1,p);
        float b = compute_triangle_area(p1,p2,p);
        float c = compute_triangle_area(p2,p0,p);
        return a+b+c <= abc+0.0001f;
    }
}

inline void get_uv_coords( const vec3f &p0, const vec3f &p1, const vec3f &p2, const vec3f &p, float &u, float &v ) {
    vec3f v0 = p2 - p0, v1 = p1 - p0, v2 = p - p0;
    float d00 = dot(v0,v0), d01 = dot(v0,v1), d02 = dot(v0,v2);
    float d11 = dot(v1,v1), d12 = dot(v1,v2);
    float i = 1.0f / (d00 * d11 - d01 * d01);
    u = (d11 * d02 - d01 * d12) * i;
    v = (d00 * d12 - d01 * d02) * i;
}

float compute_triangle_area_in_sphere( const vec3f &v0, const vec3f &v1, const vec3f &v2, const vec3f &o, const float &r ) {
    const float epsilon = 0.001f;
    float r2 = r*r;
    bool o0 = lengthSqr(o-v0) >= r2;// - epsilon;
    bool o1 = lengthSqr(o-v1) >= r2;// - epsilon;
    bool o2 = lengthSqr(o-v2) >= r2;// - epsilon;
    return compute_triangle_area_in_sphere(v0,o0,v1,o1,v2,o2,o,r);
}
float compute_triangle_area_in_sphere( const vec3f &v0, const bool o0, const vec3f &v1, const bool o1, const vec3f &v2, const bool o2, const vec3f &o, const float &r ) {
    const float epsilon = 0.001f;
    float r2 = r*r;
    
    vec3f vclosest = closest_point_point_triangle(o,v0,v1,v2);
    float d_oclosest2 = lengthSqr(vclosest-o); if( d_oclosest2 > r2 ) return 0.0f;
    float a_tri = compute_triangle_area(v0,v1,v2); if( a_tri < 0.000001f ) return 0.0f;
    
    int outside = (o0?1:0) + (o1?1:0) + (o2?1:0);
    
    if( outside == 0 ) {
        // all three verts are inside
        return a_tri;
    }
    if( outside == 3 ) {
        // all three verts are outside.  tesselate and recurse (only once)
        float a01 = compute_triangle_area_in_sphere(vclosest,false,v0,true,v1,true,o,r);
        float a12 = compute_triangle_area_in_sphere(vclosest,false,v1,true,v2,true,o,r);
        float a20 = compute_triangle_area_in_sphere(vclosest,false,v2,true,v0,true,o,r);
        float a = a01 + a12 + a20;
        error_if_not( a <= a_tri+epsilon, "outside=3, returning too much area?" );
        return min(a,a_tri);
    }
    
    // find intersection of sphere and plane of triangle (circle)
    vec3f n = triangle_normal(v0,v1,v2);
    float oc = dot(v0-o,n);
    error_if_not_va( oc*oc <= r2, "origin of circle (intersection of sphere, plane) is outside of sphere?" );
    vec3f ocirc = o + (n * oc);                    // center of circle
    float rcirc = sqrtf( r2 - oc*oc );             // radius of circle
    
    if( outside == 1 ) {
        // two verts inside
        
        vec3f vA = ( o0 ? v1 : v0 );
        vec3f vB = ( o2 ? v1 : v2 );
        vec3f vC = ( o0 ? v0 : (o1 ? v1 : v2) );
        
        assert( lengthSqr(o-vA) <= r2 );
        assert( lengthSqr(o-vB) <= r2 );
        assert( lengthSqr(o-vC) >= r2 );
        
        vec3f vCA = vA, vCB = vB, vAC = vA, vBC = vB;
        intersect_sphere(vC,vA,o,r,vCA); intersect_sphere(vA,vC,o,r,vAC);
        intersect_sphere(vC,vB,o,r,vCB); intersect_sphere(vB,vC,o,r,vBC);
        vec3f vA_ = ( lengthSqr(vC-vCA)<lengthSqr(vC-vAC) ? vCA : vAC );
        vec3f vB_ = ( lengthSqr(vC-vCB)<lengthSqr(vC-vBC) ? vCB : vBC );
        
        float aq = compute_quad_area(vA, vB, vB_, vA_);
        float as = compute_piecrust_area(ocirc,rcirc,vB_,vA_);
        
        float a = aq + as;
        error_if_not( a <= a_tri+epsilon, "outside=1, returning too much area?" );
        
        return min(a,a_tri);
    }
    
    assert( outside == 2 );
    
    // one vert inside
    
    vec3f vA = ( !o0 ? v0 : ( !o1 ? v1 : v2 ) );
    vec3f vB = ( o0 ? v0 : v1 );
    vec3f vC = ( o2 ? v2 : v1 );
    
    assert( lengthSqr(o-vA) <= r2 );
    assert( lengthSqr(o-vB) >= r2 );
    assert( lengthSqr(o-vC) >= r2 );
    
    vec3f vCA = vA, vAB = vA, vAC = vC, vBA = vB;
    intersect_sphere(vC,vA,o,r,vCA); intersect_sphere(vA,vC,o,r,vAC);
    intersect_sphere(vB,vA,o,r,vBA); intersect_sphere(vA,vB,o,r,vAB);
    vec3f vC_ = ( lengthSqr(vA-vCA)<lengthSqr(vA-vAC) ? vCA : vAC );
    vec3f vB_ = ( lengthSqr(vA-vBA)<lengthSqr(vA-vAB) ? vBA : vAB );
    
    vec3f vCB, vBC;
    bool iBC = intersect_sphere( vB, vC, o, r, vBC );
    bool iCB = intersect_sphere( vC, vB, o, r, vCB );
    
    float a_tris = compute_triangle_area(vA,vB_,vC_);
    
    if( iBC && iCB ) {
        float a_quad = compute_quad_area( vB_, vC_, vCB, vBC );
        float a_pie0 = compute_piecrust_area(ocirc,rcirc,vB_,vBC);
        float a_pie1 = compute_piecrust_area(ocirc,rcirc,vC_,vCB);
        
        float a = a_tris + a_quad + a_pie0 + a_pie1;
        
        if( a > a_tri+epsilon ) {
            a = a_tris+a_quad;
            error_if_not(a <= a_tri+epsilon, "still!?");
        }
        return min(a,a_tri);
    }
    
    vec3f vBBA  = vA - vB_; //vB - vB_;
    vec3f vCABA = vC_ - vB_; //vC_ - vB_;
    vec3f vOBA  = ocirc - vB_; //ocirc - vB_;
    vec3f vBBAx = cross(vBBA,vCABA)*1000.0f;
    vec3f vOBAx = cross(vOBA,vCABA)*1000.0f;
    
    float dB = dot(vBBAx,vOBAx);
    
    bool inside = (dB<0.0f) && (a_tris>epsilon);
    
    float a_pie = compute_piecrust_area(ocirc,rcirc,vB_,vC_,inside);
    
    float a = a_tris + a_pie;
    
    if( a > a_tri+epsilon ) {
        float a_pie0 = compute_piecrust_area(ocirc,rcirc,vB_,vC_,!inside);
        a = a_tris + a_pie0;
    }
    
    return min(a,a_tri);
}

float compute_mean_curvature_( const vec3f &v, const float &r, const vector<vec3f> &pos, const vector<vec3i> &faces, const unordered_set<int> &s_if ) {
    float sa = 0.0f;
    for( int i_f : s_if ) {
        const vec3i &f = faces[i_f];
        sa += compute_triangle_area_in_sphere(pos[f.x],pos[f.y],pos[f.z],v,r);
    }
    float mc = max( 0.0f, min( 2.0f, 2.0f-sa/(PIf*r*r) ) );
    float rnd = 10000.0f;
    return roundf(mc*rnd)/rnd;
}

vec3f closest_point_point_triangle( const vec3f &p, const vec3f &v0, const vec3f &v1, const vec3f &v2 ) {
    vec3f n = triangle_normal(v0,v1,v2);
    
    vec3f p012 = n*dot(v0-p,n) + p;
    if( sameside(p012,v0,v1,v2) && sameside(p012,v1,v0,v2) && sameside(p012,v2,v0,v1) ) {
        return p012;
    }
    
    vec3f v10=v1-v0, v21=v2-v1, v02=v0-v2;
    float l10=length(v10), l21=length(v21), l02=length(v02);
    vec3f v10n=v10/l10, v21n=v21/l21, v02n=v02/l02;
    
    float dp0=dot(p-v0,v10n), dp1=dot(p-v1,v21n), dp2=dot(p-v2,v02n);
    dp0 = max(0.0f,min(l10,dp0));
    dp1 = max(0.0f,min(l21,dp1));
    dp2 = max(0.0f,min(l02,dp2));
    vec3f p01=v10n*dp0+v0, p12=v21n*dp1+v1, p20=v02n*dp2+v2;
    
    float dp012=lengthSqr(p01-p);
    float dp122=lengthSqr(p12-p);
    float dp202=lengthSqr(p20-p);
    
    if( dp012 <= dp122 && dp012 <= dp202 ) return p01;
    if( dp122 <= dp012 && dp122 <= dp202 ) return p12;
    return p20;
}




