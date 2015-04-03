#ifndef _LIGHT_H_
#define _LIGHT_H_

#include "gls.h"
#include "debug.h"
#include "object.h"
#include "vec.h"
#include "xform.h"

extern vector<int> GLLights;

struct Source : object {
    unsigned int _glid = -1;
    bool on = true;
    
    Source() {
        assert( !GLLights.empty() );
        _glid = GLLights.back(); GLLights.pop_back();
        //printf( "**** new light: %d ****\n", _glid );
    }
    ~Source() {
        glDisable(GL_LIGHT0+_glid);
        GLLights.push_back(_glid);
    }
    
    virtual void draw() { /*not_implemented_warning();*/ }
};

struct PointSource : Source {
    vec3f pos = zero3f;
    vec3f intensity = one3f;
    
    virtual void draw();
};

struct DirectionalSource : Source {
    vec3f dir = z3f;
    vec3f intensity = one3f;
    
    virtual void draw();
};

struct AmbientSource : Source {
    vec3f intensity = { 0.2f, 0.2f, 0.2f };
    
    virtual void draw();
};

struct Light : object {
    frame3f f;
    Source* source = 0;
    
    virtual void init() { }
    
    virtual void draw();
};

#endif
