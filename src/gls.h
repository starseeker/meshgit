#ifndef _GLS_H_
#define _GLS_H_

#ifdef __APPLE__
#include <OpenGL/gl.h>
#include <GLUT/glut.h>
#else
#include <GL/gl.h>
#include <GL/glut.h>
#endif

#include "ext/common/vec.h"

#include "mat.h"
#include "image.h"
#include "debug.h"

#define GLS_CHECK_ERROR 1

#if GLS_CHECK_ERROR
inline void _checkGlError() {
    auto errcode = glGetError();
    if(not (errcode == GL_NO_ERROR)) {
        warning_va("gl error: %s", gluErrorString(errcode));
    } 
}
#else
inline void _checkGlError() { }
#endif



struct material {
    vec3f ambient = one3f;
    vec3f diffuse = {0.7f,0.7f,0.7f};
    vec3f specular = zero3f;
    float shininess = 64.0f;
    vec3f emission = zero3f;
};




const int MOUSE_LBUTTON = 1;
const int MOUSE_MBUTTON = 2;
const int MOUSE_RBUTTON = 4;



inline void glsCheckError() { _checkGlError(); }


inline void glsVertex(const vec2f& v) { glVertex2fv(&v.x); }
inline void glsVertex(const vec2d& v) { glVertex2dv(&v.x); }
inline void glsVertex(const vec3f& v) { glVertex3fv(&v.x); }
inline void glsVertex(const vec3d& v) { glVertex3dv(&v.x); }
inline void glsVertex(const vec4f& v) { glVertex4fv(&v.x); }
inline void glsVertex(const vec4d& v) { glVertex4dv(&v.x); }

inline void glsNormal(const vec3f& v) { glNormal3fv(&v.x); }
inline void glsNormal(const vec3d& v) { glNormal3dv(&v.x); }

inline void glsColor(const vec3f& v) { glColor3fv(&v.x); }
inline void glsColor(const vec3f& v, float a) { glColor4f(v.x,v.y,v.z,a); }
inline void glsColor(const vec3d& v) { glColor3dv(&v.x); }
inline void glsColor(const vec4f& v) { glColor4fv(&v.x); }
inline void glsColor(const vec4d& v) { glColor4dv(&v.x); }

inline void glsMaterialfv( GLenum face, GLenum pname, const vec3f &v ) {
    float v_ [] = { v.x, v.y, v.z, 1.0f };
    glMaterialfv(face,pname,v_);
}
inline void glsMaterialfv( GLenum face, GLenum pname, const vec4f &v ) {
    glMaterialfv(face,pname,&v.x);
}
inline void glsMaterialfv( GLenum pname, const vec3f &cfront, const vec3f &cback ) {
    float cf_ [] = { cfront.x, cfront.y, cfront.z, 1.0f };
    float cb_ [] = { cback.x, cback.y, cback.z, 1.0f };
    glMaterialfv( GL_FRONT, pname, cf_ );
    glMaterialfv( GL_BACK, pname, cb_ );
}
inline void glsMaterialfv( GLenum pname, const vec4f &cfront, const vec4f &cback ) {
    glMaterialfv( GL_FRONT, pname, &cfront.x );
    glMaterialfv( GL_BACK, pname, &cback.x );
}
inline void glsMaterial( GLenum face, const material &mat ) {
    glsMaterialfv( face, GL_DIFFUSE,   mat.diffuse   );
    glsMaterialfv( face, GL_SPECULAR,  mat.specular  );
    glMaterialf(   face, GL_SHININESS, mat.shininess );
    glsMaterialfv( face, GL_AMBIENT,   mat.ambient   );
    glsMaterialfv( face, GL_EMISSION,  mat.emission  );
}
inline void glsMaterial( const material &mfront, const material &mback ) {
    glsMaterial( GL_FRONT, mfront );
    glsMaterial( GL_BACK, mback );
}

inline void glsTexCoord(const vec2f& v) { glTexCoord2fv(&v.x); }
inline void glsTexCoord(const vec2d& v) { glTexCoord2dv(&v.x); }

inline void glsRasterPos(const vec2f& v) { glRasterPos2fv(&v.x); }
inline void glsRasterPos(const vec2d& v) { glRasterPos2dv(&v.x); }

inline void glsMultMatrix(const mat4f& m) { _checkGlError(); glMultTransposeMatrixf(&m.x.x); _checkGlError(); }
inline void glsLoadMatrix(const mat4f& m) { _checkGlError(); glLoadTransposeMatrixf(&m.x.x); _checkGlError(); }
inline void glsMultMatrix(const mat4d& m) { _checkGlError(); glMultTransposeMatrixd(&m.x.x); _checkGlError(); }
inline void glsLoadMatrix(const mat4d& m) { _checkGlError(); glLoadTransposeMatrixd(&m.x.x); _checkGlError(); }

inline void glsVertexPointer(const vector<vec2f>& v) { glVertexPointer(2,GL_FLOAT,0,&v[0].x); }
inline void glsVertexPointer(const vector<vec2d>& v) { glVertexPointer(2,GL_DOUBLE,0,&v[0].x); }
inline void glsVertexPointer(const vector<vec3f>& v) { glVertexPointer(3,GL_FLOAT,0,&v[0].x); }
inline void glsVertexPointer(const vector<vec3d>& v) { glVertexPointer(3,GL_DOUBLE,0,&v[0].x); }
inline void glsVertexPointer(const vector<vec4f>& v) { glVertexPointer(4,GL_FLOAT,0,&v[0].x); }
inline void glsVertexPointer(const vector<vec4d>& v) { glVertexPointer(4,GL_DOUBLE,0,&v[0].x); }
inline void glsNormalPointer(const vector<vec3f>& v) { glNormalPointer(GL_FLOAT,0,&v[0].x); }
inline void glsNormalPointer(const vector<vec3d>& v) { glNormalPointer(GL_DOUBLE,0,&v[0].x); }
inline void glsColorPointer(const vector<vec3f>& v) { glColorPointer(3,GL_FLOAT,0,&v[0].x); }
inline void glsColorPointer(const vector<vec3d>& v) { glColorPointer(3,GL_DOUBLE,0,&v[0].x); }
inline void glsColorPointer(const vector<vec4f>& v) { glColorPointer(4,GL_FLOAT,0,&v[0].x); }
inline void glsColorPointer(const vector<vec4d>& v) { glColorPointer(4,GL_DOUBLE,0,&v[0].x); }
inline void glsTexCoordPointer(const vector<vec2f>& v) { glTexCoordPointer(2,GL_FLOAT,0,&v[0].x); }
inline void glsTexCoordPointer(const vector<vec2d>& v) { glTexCoordPointer(2,GL_DOUBLE,0,&v[0].x); }

inline void glsDrawElements(const vector<vec3i>& v) { glDrawElements(GL_TRIANGLES,v.size()*3,GL_UNSIGNED_INT,&v[0].x); }
inline void glsDrawElements(const vector<vec4i>& v) { glDrawElements(GL_QUADS,v.size()*4,GL_UNSIGNED_INT,&v[0].x); }


#endif
