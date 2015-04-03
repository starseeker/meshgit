#include "camera.h"

#include "gls.h"
#include "draw_utils.h"

#define PIf 3.14159265f

inline float dist(const vec3f& a, const vec3f& b) { return length(a-b); }
inline float distSqr(const vec3f& a, const vec3f& b) { return lengthSqr(a-b); }
inline float clamp(float x, float m, float M) { return min(M,max(m,x)); }
inline void orthonormalize(vec3f& x, vec3f& y, vec3f& z) { z = normalize(z); x = normalize(cross(y,z)); y = normalize(cross(z,x)); }
inline frame3f orthonormalize(const frame3f& f) { frame3f ret = f; orthonormalize(ret.x, ret.y, ret.z); return ret; }




mat4f VPCamera::view() {
    return frame_to_matrix_inverse(f);
}

mat4f VPCamera::projection( float aspect ) {
    float w = aspect * h;
    if(not orthographic) return frustum(-w/2,w/2,-h/2,h/2,d,1000000.0f);
    //float s = 0.1f / max( 0.00001f, l );
    float s = d / max( 0.00001f, l );
    return ortho(-w/2, w/2, -h/2, h/2, d, 1000.0f)* scale( makevec3f( s, s, s ) ) * translate( makevec3f( 0, 0, -100 ) );
}

void VPCamera::draw( float aspect ) {
    
    if( flying ) {
        vec3f eye = f.o;
        vec3f center = f.o - f.z * l;
        vec3f up = f.y;
        
        eye = ( eye * 0.95f ) + ( flying_eye * 0.05f );
        center = ( center * 0.95f ) + ( flying_center * 0.05f );
        up = normalize( ( up * 0.95f ) + ( flying_up * 0.05f ) );
        
        f.o = eye;
        f.z = normalize(eye-center);
        f.y = up;
        f.x = f.z ^ f.y;
        f = orthonormalize(f);
        l = dist(eye,center);
    }
    
	glMatrixMode(GL_PROJECTION);
	glsLoadMatrix( projection( aspect ) );
    
	glMatrixMode(GL_MODELVIEW);
	glsLoadMatrix( view() );
    
    //glPushAttrib(GL_LIGHTING_BIT);
    //glPushAttrib(GL_ENABLE_BIT);
    glEnable(GL_DEPTH_TEST); glDepthFunc( GL_LEQUAL );
    glEnable(GL_LIGHTING);
    glLightModelf(GL_LIGHT_MODEL_COLOR_CONTROL, GL_SEPARATE_SPECULAR_COLOR);
    glLightModelf(GL_LIGHT_MODEL_LOCAL_VIEWER, 1);
    glLightModelf(GL_LIGHT_MODEL_TWO_SIDE, 1 );
    //glLightModelf(GL_LIGHT_MODEL_TWO_SIDE, (opts->doublesided) ? 1 : 0);
    if( use_lights ) {
        //draw_ambient_light( light_ambient );
        glPushMatrix();
        update_lights();
        for( auto light : lights ) light->draw();
        glPopMatrix();
    }
}

void VPCamera::draw( int w, int h ) {
    draw( (float)w / (float) h );
}

ray3f VPCamera::trace_ray( const vec2f& uv, float aspect ) {
    float w = aspect * h;
    ray3f r;
    
    if(not orthographic) {
        auto q = makevec3f((uv.x-0.5f)*w, (uv.y-0.5f)*h, -d);
        r = makeray3f(zero3f,normalize(q));
    } else {
        float s = 10.0f * l;
        auto o = makevec3f((uv.x-0.5f)*w*s, (uv.y-0.5f)*h*s, 0);
        r = makeray3f(o,-z3f);
    }
    
    return transformRay( f, r );
}

void VPCamera::center() {
    vec3f e = f.o;
    vec3f u = f.y;
    vec3f b = f.z;
    vec3f f = e + l * b;
    
    vec3f e_n = f - e;
    vec3f f_n = zero3f;
    vec3f u_n = u;
    
    lookat(e_n,f_n,u_n);
}
void VPCamera::lookat(const vec3f& eye, const vec3f& center, const vec3f& up) {
    flying = false;
    vec3f c_ = max_component(center_limit.min, min_component( center_limit.max, center ) );
    f.o = eye + (c_-center);
    f.z = normalize(eye-c_);
    f.y = up;
    f.x = f.z ^ f.y;
    f = orthonormalize(f);
    l = dist(eye,c_);
}
void VPCamera::lookat(const frame3f& f, const float dist) {
    flying = false;
    this->f = f;
    this->l = d;
    l = d;
}

void VPCamera::flyto(const vec3f &eye, const vec3f &center, const vec3f &up ) {
    flying_eye = eye;
    flying_center = center;
    flying_up = up;
    flying = true;
}

void VPCamera::flyto(const vec3f &center) {
    vec3f e = f.o;
    vec3f u = f.y;
    vec3f b = f.z;
    vec3f f = e + l * b;
    
    vec3f e_n = f - e + center;
    vec3f f_n = center;
    vec3f u_n = u;
    
    flyto( e_n, f_n, u_n );
}

// assume world up is z
void VPCamera::turntable_rotate(float delta_phi, float delta_theta) {
    if( !allow_rotate ) return;
    
    float phi = atan2(f.z.y,f.z.x);
    float theta = acos(f.z.z);
    
    phi += delta_phi;
    theta += delta_theta;
    theta = clamp(theta,0.001f,PIf-0.001f);
    
    vec3f nz = makevec3f(sin(theta)*cos(phi),
                         sin(theta)*sin(phi),
                         cos(theta));
    vec3f nc = f.o-f.z*l;
    vec3f no = nc + nz * l;
    
    lookat(no, nc, z3f);
    flying = false;
}

void VPCamera::turntable_dolly(float delta_f) {
    vec3f c = f.o - f.z * l;
    l = max(l+delta_f*dolly_scale,ray3f::epsilon);
    f.o = c + f.z * l;
    flying = false;
}

void VPCamera::turntable_pan(float delta_x, float delta_y) {
    vec3f o = f.o + move_scale*(f.x*delta_x + f.y*delta_y);
    vec3f c = o - f.z*l;
    vec3f c_ = max_component(center_limit.min, min_component( center_limit.max, c ) );
    f.o = o + (c_-c);;
    flying = false;
}

// strafes relative to current camera orientation
void VPCamera::fps_move(float forward, float right, float up) {
    f.o += -f.z*forward + f.x*right + -f.y*up;
    return;
    auto up_v = f.z; //z3f;
    auto forward_v = -f.z;
    auto right_v = f.x;
    orthonormalize(right_v, forward_v, up_v);
    f.o += forward*forward_v + right*right_v + up*up_v;
    flying = false;
}

void VPCamera::fps_rotate(float delta_right, float delta_up) {
    if( !allow_rotate ) return;
    
    // do it
    auto my = rotation(z3f, delta_right);
    auto mx = rotation(f.x, delta_up);
    auto m = my*mx;
    f.x = transform_direction(m, f.x);
    f.y = transform_direction(m, f.y);
    f.z = transform_direction(m, f.z);
    flying = false;
}

void VPCamera::handle_mouse_down( int mx, int my, MouseAction action ) {
    mouse_pre = makevec2i(mx,my);
    mouse_act = action;
    flying = false;
}
void VPCamera::handle_mouse_up() { mouse_act = mouse_none; }
void VPCamera::handle_mouse_move( int mx, int my ) {
    vec2i mouse_cur = makevec2i( mx, my );
    vec2i mouse_del = mouse_cur - mouse_pre;
    switch(mouse_act) {
        case mouse_turntable_rotate: {
            float delta_phi = -mouse_del.x * 0.01;
            float delta_theta = mouse_del.y * 0.01;
            turntable_rotate(delta_phi, delta_theta);
        } break;
        case mouse_turntable_dolly: {
            float delta_f = mouse_del.y * 0.1f; // 0.04;
            turntable_dolly(delta_f);
        } break;
        case mouse_turntable_pan: {
            float delta_x = -mouse_del.x * 0.004 * l;
            float delta_y = -mouse_del.y * 0.004 * l;
            turntable_pan(delta_x,delta_y);
        } break;
        case mouse_fps_rotate: {
            float delta_phi = -mouse_del.x * 0.01;
            float delta_theta = mouse_del.y * 0.01;
            fps_rotate(delta_phi, delta_theta);
        } break;
        default: break;
    }
    mouse_pre = mouse_cur;
}



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



void Camera::lookat(const vec3f& eye, const vec3f& center, const vec3f& up) {
    f.o = eye;
    f.z = normalize(eye-center);
    f.y = up;
    f = orthonormalize(f);
    l = dist(eye,center);
}

// assume world up is z
void Camera::turntable_rotate(float delta_phi, float delta_theta) {
    float phi = atan2(f.z.y,f.z.x);
    float theta = acos(f.z.z);
    
    phi += delta_phi;
    theta += delta_theta;
    theta = clamp(theta,0.001f,PIf-0.001f);
    
    vec3f nz = makevec3f(sin(theta)*cos(phi),
                         sin(theta)*sin(phi),
                         cos(theta));
    vec3f nc = f.o-f.z*l;
    vec3f no = nc + nz * l;
    
    lookat(no, nc, z3f);
}

void Camera::turntable_dolly(float delta_f) {
    vec3f c = f.o - f.z * l;
    l = max(l+delta_f,ray3f::epsilon);
    f.o = c + f.z * l;
}

void Camera::turntable_pan(float delta_x, float delta_y) {
    f.o += f.x * delta_x + f.y * delta_y;
}



// strafes relative to current camera orientation
void Camera::fps_move(float forward, float right, float up) {
    f.o += -f.z*forward + f.x*right + -f.y*up;
    return;
    auto up_v = f.z; //z3f;
    auto forward_v = -f.z;
    auto right_v = f.x;
    orthonormalize(right_v, forward_v, up_v);
    f.o += forward*forward_v + right*right_v + up*up_v;
}

void Camera::fps_rotate(float delta_right, float delta_up) {
    // do it
    auto my = rotation(z3f, delta_right);
    auto mx = rotation(f.x, delta_up);
    auto m = my*mx;
    f.x = transform_direction(m, f.x);
    f.y = transform_direction(m, f.y);
    f.z = transform_direction(m, f.z);
}

void Camera::draw() {
	glMatrixMode(GL_PROJECTION);
	glsLoadMatrix(lens->projection());
	glMatrixMode(GL_MODELVIEW);
	glsLoadMatrix(view());
}