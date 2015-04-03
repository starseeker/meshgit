#include "light.h"
#include "gls.h"
#include "vec.h"

vector<int> GLLights = {7,6,5,4,3,2,1,0};

void Light::draw() {
    glPushMatrix();
    glsMultMatrix(frame_to_matrix(f));
    source->draw();
    glPopMatrix();
}

void PointSource::draw() {
    if( !on ) { glDisable(GL_LIGHT0+_glid); return; }
    
    vec4f _pos = makevec4f(pos.x,pos.y,pos.z,1);
    vec4f _intensity = makevec4f(intensity.x,intensity.y,intensity.z,1);
    
    glEnable(GL_LIGHT0 + _glid);
    glLightfv(GL_LIGHT0 + _glid, GL_AMBIENT, &zero4f.x);
    glLightfv(GL_LIGHT0 + _glid, GL_DIFFUSE, &_intensity.x);
    glLightfv(GL_LIGHT0 + _glid, GL_SPECULAR, &_intensity.x);
    glLightfv(GL_LIGHT0 + _glid, GL_POSITION, &_pos.x);
    glLightf(GL_LIGHT0 + _glid, GL_CONSTANT_ATTENUATION, 0);
    glLightf(GL_LIGHT0 + _glid, GL_LINEAR_ATTENUATION, 0);
    glLightf(GL_LIGHT0 + _glid, GL_QUADRATIC_ATTENUATION, 1);
}

void DirectionalSource::draw() {
    if( !on ) { glDisable(GL_LIGHT0+_glid); return; }
    
    vec4f _dir = makevec4f(-dir.x,-dir.y,-dir.z,0);
    vec4f _intensity = makevec4f(intensity.x,intensity.y,intensity.z,1);
    
    glEnable(GL_LIGHT0 + _glid);
    glLightfv(GL_LIGHT0 + _glid, GL_AMBIENT, &zero4f.x);
    glLightfv(GL_LIGHT0 + _glid, GL_DIFFUSE, &_intensity.x);
    glLightfv(GL_LIGHT0 + _glid, GL_SPECULAR, &_intensity.x);
    glLightfv(GL_LIGHT0 + _glid, GL_POSITION, &_dir.x);    
    glLightf(GL_LIGHT0 + _glid, GL_CONSTANT_ATTENUATION, 1);
    glLightf(GL_LIGHT0 + _glid, GL_LINEAR_ATTENUATION, 0);
    glLightf(GL_LIGHT0 + _glid, GL_QUADRATIC_ATTENUATION, 0);
}

void AmbientSource::draw() {
    if( !on ) {
        glLightfv(GL_LIGHT0+_glid, GL_AMBIENT, &zero4f.x);
        glDisable(GL_LIGHT0+_glid);
        return;
    }
    
    vec4f _intensity = { intensity.x, intensity.y, intensity.z, 1.0f };
    
    glEnable(GL_LIGHT0+_glid);
    glLightfv(GL_LIGHT0 + _glid, GL_AMBIENT, &_intensity.x);
    glLightfv(GL_LIGHT0 + _glid, GL_DIFFUSE, &zero4f.x);
    glLightfv(GL_LIGHT0 + _glid, GL_SPECULAR, &zero4f.x);
}
