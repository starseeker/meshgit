#ifndef _CAMERA_H_
#define _CAMERA_H_

#include "debug.h"

#include "ext/common/vec.h"
#include "ext/common/ray.h"
#include "ext/common/frame.h"

#include "object.h"
#include "mat.h"
#include "gls.h"
#include "xform.h"
#include "light.h"


enum MouseAction {
    mouse_none,
    mouse_turntable_rotate,
    mouse_turntable_dolly,
    mouse_turntable_pan,
    mouse_fps_rotate,
    mouse_edit,
    mouse_set_frame,
};



struct VPCamera {
    frame3f f = defaultframe3f;     // camera position
    float   l = 1.0f;                  // camera focus distance
    bool    orthographic = false;
    
    // closely matches blender
    float   h = 0.15f;               // image plane height
    float   d = 0.1f;               // image plane distance
    
    vec3f flying_eye, flying_center, flying_up;
    bool  flying = false;
    
    range3f center_limit = { {-1000.0f,-1000.0f,-1000.0f}, {1000.0f,1000.0f,1000.0f} };
    vec3f   move_scale = { 1.0f, 1.0f, 1.0f };
    float   dolly_scale = 1.0f;
    bool    allow_rotate = true;
    
    vec2i mouse_pre;
    MouseAction mouse_act;
    
    bool    use_lights = true;
    vector<Light*> lights;
    
    
    
    void set_default_lights() {
        vec3f   ambient = { 0.05, 0.05, 0.05 };
        vector<vec3f> cameralights_dir = { {1,-1,-1}, {-1,-1,-1}, {-1,1,0.2}, {0,1,0} };
        vector<vec3f> cameralights_col = { {0.6,0.6,0.65}, {0.45,0.4,0.4}, {0.20,0.20,0.20}, {0.20,0.15,0.20} };
        
        assert( lights.empty() );
        
        auto s = new AmbientSource();
        auto l = new Light;
        l->source = s;
        l->f = f;
        s->intensity = ambient;
        lights.push_back(l);
        
        for( int i = 0; i < cameralights_dir.size(); i++ ) {
            auto s = new DirectionalSource();
            auto l = new Light;
            l->source = s;
            
            l->f = f;
            s->dir = normalize(cameralights_dir[i]);
            s->intensity = cameralights_col[i];
            
            lights.push_back(l);
        }
    }
    void update_lights() {
        for( auto light : lights ) {
            light->f = f;
        }
    }
    
    mat4f projection( float aspect );
    mat4f view();
    void draw( float aspect );
    void draw( int w, int h );
    
    ray3f trace_ray( const vec2f& uv, float aspect );
    
    void center();
    void lookat(const vec3f &eye, const vec3f &center, const vec3f &up);
    void lookat(const frame3f& f, const float dist);
    
    void flyto(const vec3f &eye, const vec3f &center, const vec3f &up );
    void flyto(const vec3f &center);
    
    void turntable_rotate(float delta_phi, float delta_theta);
    void turntable_dolly(float delta_f);
    void turntable_pan(float delta_x, float delta_y);
    
    void fps_move(float forward, float right, float up);
    void fps_rotate(float delta_right, float delta_up);
    
    virtual void handle_mouse_down( int mx, int my, MouseAction action );
    virtual void handle_mouse_up();
    virtual void handle_mouse_move( int mx, int my );
};


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


struct Lens : object {
    float w = 0.1f;
    float h = 0.1f;
    float d = 0.1f; //1;
    
    virtual void init() { }
    
    virtual mat4f projection() {
		not_implemented_error();
		return identity_mat4f;
	}
    
    int image_width(int r) { return (int)round(r * w / h); }
    int image_height(int r) { return r; }
    
    void set_aspectratio(int imw, int imh) { w = (h * imw) / imh; }
    float aspectratio() { return h/w; }
    
    virtual ray3f trace_ray(const vec2f& uv) {
		not_implemented_error();
		return makeray3f(zero3f,-z3f);
	}
};

struct PinholeLens : Lens {
    bool orthographic = false;
    
    // TODO: implement ortho
    virtual mat4f projection() {
        if(not orthographic) return frustum(-w/2,w/2,-h/2,h/2,d,1000000.0f);
        else return ortho(-w/2, w/2, -h/2, h/2, d, 1000000.0f);
    }
    
    virtual ray3f trace_ray(const vec2f& uv) {
        if(not orthographic) {
            auto q = makevec3f((uv.x-0.5f)*w, (uv.y-0.5f)*h, -d);
            return makeray3f(zero3f,normalize(q));
        } else {
            auto l = makevec3f((uv.x-0.5f)*w, (uv.y-0.5f)*h, 0);
            return makeray3f(l,-z3f);
        }
    }
};

struct Camera : object {
    frame3f f = defaultframe3f;
    float   l = 1;
    Lens*   lens = nullptr;
    
    void lookat(const vec3f& eye, const vec3f& center, const vec3f& up);
    void turntable_rotate(float delta_phi, float delta_theta);
    void turntable_dolly(float delta_f);
    void turntable_pan(float delta_x, float delta_y);
    void fps_move(float forward, float right, float up);
    void fps_rotate(float delta_right, float delta_up);
    
    virtual mat4f view() { return frame_to_matrix_inverse(f); }
    
    virtual void init() { lens->init(); }
    
    virtual void draw();

    virtual ray3f trace_ray(const vec2f& uv) {
        return transformRay(f, lens->trace_ray(uv));
    }
};

#endif
