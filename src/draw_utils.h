#ifndef _DRAW_UTILS_H_
#define _DRAW_UTILS_H_

#define PIf 3.14159265f

#include "gls.h"
#include "geom.h"
#include "vec.h"
#include "ext/common/quaternion.h"

inline void draw_clear(float r, float g, float b, float a) {
    glsCheckError();
    glClearColor(r,g,b,a);
    glClearDepth(1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glsCheckError();
}
inline void draw_clear(const vec4f& c) { draw_clear(c.x,c.y,c.z,c.w); }
inline void draw_clear() { draw_clear(0,0,0,0); }

inline void draw_vertical_gradient(const vec4f &c_top, const vec4f &c_bottom) {
    glsCheckError();
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    
    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);
    
    glBegin(GL_QUADS);
    
    glColor4f(c_top.x,c_top.y,c_top.z,c_top.w);
    glVertex2f(1,1);
    glVertex2f(-1,1);
    glColor4f(c_bottom.x,c_bottom.y,c_bottom.z,c_bottom.w);
    glVertex2f(-1,-1);
    glVertex2f(1,-1);
    
    glEnd();
    
    glEnable(GL_DEPTH_TEST);
    
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glsCheckError();
}
inline void draw_clear_horizontal(const vec4f &c_left, const vec4f &c_right) {
    glsCheckError();
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
    glDisable(GL_LIGHTING);
    
    glBegin(GL_QUADS);
    
    glColor4f(c_left.x,c_left.y,c_left.z,c_left.w);
    glVertex2f(-1,1);
    glVertex2f(-1,-1);
    glColor4f(c_right.x,c_right.y,c_right.z,c_right.w);
    glVertex2f(1,-1);
    glVertex2f(1,1);
    
    glEnd();
    
    glClearDepth(1);
    glClear(GL_DEPTH_BUFFER_BIT);
    
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glsCheckError();
}

inline void draw_identitymatrices() {
    glsCheckError();
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glsCheckError();
}

inline image3f draw_readpixels(int x, int y, int w, int h, bool front) {
    glsCheckError();
    image3f img(w,h);
    if(front) glReadBuffer(GL_FRONT); else glReadBuffer(GL_BACK);
    glReadPixels(x, y, w, h, GL_RGB, GL_FLOAT, img.data());
    glsCheckError();
    return img;
}

inline image3f draw_readpixels(int w, int h, bool front) {
    return draw_readpixels(0,0,w,h,front);
}

inline void draw_line(const vec3f& a, const vec3f& b) {
    glsCheckError();
    glBegin(GL_LINES);
    glsVertex(a);
    glsVertex(b);
    glEnd();
    glsCheckError();
}

inline void draw_point(const vec3f& p) {
    glsCheckError();
    glBegin(GL_POINTS);
    glsVertex(p);
    glEnd();
    glsCheckError();
}

inline void draw_grid(float size = 2, int steps = 10, float linewidth=1.0,
                      const vec3f& c = makevec3f(0.3,0.3,0.3),
                      const vec3f& o = zero3f, const vec3f& u = x3f, const vec3f& v = y3f
                       ) {
    glsCheckError();
    glLineWidth(linewidth);
    glsColor(c);
    glBegin(GL_LINES);
    for(int i = -steps/2; i <= steps/2; i ++) {
        float t = i / float(steps/2);
        glsVertex(o+(u*t+v)*(size/2));
        glsVertex(o+(u*t-v)*(size/2));
        glsVertex(o+(v*t+u)*(size/2));
        glsVertex(o+(v*t-u)*(size/2));
    }
    glEnd();
    glsCheckError();
}

inline void draw_triangle(const vec3f& v0, const vec3f& v1, const vec3f& v2) {
    glsCheckError();
    glBegin(GL_TRIANGLES);
    glsNormal(triangle_normal(v0, v1, v2));
    glsTexCoord(zero2f);
    glsVertex(v0);
    glsTexCoord(x2f);
    glsVertex(v1);
    glsTexCoord(y2f);
    glsVertex(v2);
    glEnd();
    glsCheckError();
}

inline void draw_quad(const vec3f& o, const vec3f& u, const vec3f& v, float w, float h) {
    glsCheckError();
    glBegin(GL_QUADS);
    glsNormal(normalize(cross(u,v)));
    glsTexCoord(makevec2f(-1,-1));
    glsVertex(o-u*w*0.5f-v*h*0.5f);
    glsTexCoord(makevec2f(-1, 1));
    glsVertex(o-u*w*0.5f+v*h*0.5f);
    glsTexCoord(makevec2f( 1, 1));
    glsVertex(o+u*w*0.5f+v*h*0.5f);
    glsTexCoord(makevec2f( 1,-1));
    glsVertex(o+u*w*0.5f-v*h*0.5f);
    glEnd();
    glsCheckError();
}

inline void draw_cross(const vec3f& o, const vec3f& u, const vec3f& v, float size) {
    glsCheckError();
    draw_line(o-u*size*0.5f,o+u*size*0.5f);
    draw_line(o-v*size*0.5f,o+v*size*0.5f);
    glsCheckError();
}

inline void draw_cross3( const vec3f &o, float size ) {
    glsCheckError();
    vec3f u = { size*0.5f, 0.0f, 0.0f };
    vec3f v = { 0.0f, size*0.5f, 0.0f };
    vec3f w = { 0.0f, 0.0f, size*0.5f };
    draw_line( o - u, o + u );
    draw_line( o - v, o + v );
    draw_line( o - w, o + w );
    glsCheckError();
}
inline void draw_cross3( const vec3f &o, float size, float rot ) {
    glsCheckError();
    vec3f r = normalize(makevec3f( 0.5f, 0.5f, 0.5f ));
    frame3f f = rotationframe3f( r, rot );
    draw_line( o - f.x, o + f.x );
    draw_line( o - f.y, o + f.y );
    draw_line( o - f.z, o + f.z );
    glsCheckError();
}

inline void draw_parametric_vert(const vec2f& uv,
                                 const function<vec3f (const vec2f&)>& pos, 
                                 const function<vec3f (const vec2f&)>& norm) {
    glsTexCoord(uv);
    glsNormal(norm(uv));
    glsVertex(pos(uv));    
}

inline void draw_parametric_face(int ur, int vr, bool ccw,
                                 const function<vec3f (const vec2f&)>& pos, 
                                 const function<vec3f (const vec2f&)>& norm) {
    glsCheckError();
    int oi[] = { 0, 0, 1, 1 };
    int oj[] = { 0, 1, 1, 0 };
    if(not ccw) { swap(oi[1],oi[3]); swap(oj[1],oj[3]); }
    glBegin(GL_QUADS);
    for(int i = 0; i < ur; i ++) {
        for(int j = 0; j < vr; j ++) {
            for(int k = 0; k < 4; k ++) {
                draw_parametric_vert(makevec2f((i+oi[k]) / float(ur), (j+oj[k]) / float(vr)), pos, norm);
            }
        }
    }
    glEnd();
    glsCheckError();
}

inline void draw_parametric_line(int ul, int vl, int ur, int vr, 
                                 const function<vec3f (const vec2f&)>& pos, 
                                 const function<vec3f (const vec2f&)>& norm) {
    glsCheckError();
    for(int i = 0; i < ul; i ++) {
        glBegin(GL_LINE_LOOP);
        for(int j = 0; j < vr; j ++) {
            draw_parametric_vert(makevec2f(i / float(ul), j / float(vr)), pos, norm);
        }
        glEnd();
    }
    for(int j = 0; j < vl; j ++) {
        glBegin(GL_LINE_LOOP);
        for(int i = 0; i < ur; i ++) {
            draw_parametric_vert(makevec2f(i / float(ur), j / float(vl)), pos, norm);
        }
        glEnd();
    }
    glsCheckError();
}

inline vec3f _sphere_pos(const vec3f& o, float r, const vec2f& uv) {
    float phi = 2 * PIf * uv.x;
    float theta = PIf * uv.y;
    return o + r * makevec3f(sin(theta)*cos(phi),sin(theta)*sin(phi),cos(theta));
}
inline vec3f _sphere_norm(const vec3f& o, float r, const vec2f& uv) {
    float phi = 2 * PIf * uv.x;
    float theta = PIf * uv.y;
    return makevec3f(sin(theta)*cos(phi),sin(theta)*sin(phi),cos(theta));
}
inline vec3f _unitsphere_pos(const vec2f& uv) {
    float phi = 2 * PIf * uv.x;
    float theta = PIf * uv.y;
    return makevec3f(sin(theta)*cos(phi),sin(theta)*sin(phi),cos(theta));
}
inline vec3f _unitsphere_norm(const vec2f& uv) {
    float phi = 2 * PIf * uv.x;
    float theta = PIf * uv.y;
    return makevec3f(sin(theta)*cos(phi),sin(theta)*sin(phi),cos(theta));
}
inline vec3f _cylinder_pos(float r, float h, const vec2f& uv) {
    float phi = 2 * PIf * uv.x;
    return makevec3f(r*cos(phi),r*sin(phi),h*uv.y);
}
inline vec3f _cylinder_norm(float r, float h, const vec2f& uv) {
    float phi = 2 * PIf * uv.x;
    return makevec3f(cos(phi),sin(phi),0);
}
inline vec3f _unitcylinder_pos(const vec2f& uv) {
    float phi = 2 * PIf * uv.x;
    return makevec3f(cos(phi),sin(phi),uv.y);
}
inline vec3f _unitcylinder_norm(const vec2f& uv) {
    float phi = 2 * PIf * uv.x;
    return makevec3f(cos(phi),sin(phi),0);
}
inline void draw_unitsphere(int ur = 128, int vr = 64) { draw_parametric_face(ur, vr, true, _unitsphere_pos, _unitsphere_norm); }
inline void draw_unitsphere_lines(int ul = 16, int vl = 8, int ur = 128, int vr = 64) { draw_parametric_line(ul, vl, ur, vr, _unitsphere_pos, _unitsphere_norm); }

inline void draw_unitcylinder(int ur = 128, int vr = 64) { draw_parametric_face(ur, vr, false, _unitcylinder_pos, _unitcylinder_norm); }
inline void draw_unitcylinder_lines(int ul = 16, int vl = 8, int ur = 128, int vr = 64) { draw_parametric_line(ul, vl, ur, vr, _unitcylinder_pos, _unitcylinder_norm); }

inline void draw_cube( const vec3f &o, float l ) {
    l /= 2.0f;
    vec3f v000 = o + makevec3f( -l, -l, -l );
    vec3f v001 = o + makevec3f( -l, -l,  l );
    vec3f v010 = o + makevec3f( -l,  l, -l );
    vec3f v011 = o + makevec3f( -l,  l,  l );
    vec3f v100 = o + makevec3f(  l, -l, -l );
    vec3f v101 = o + makevec3f(  l, -l,  l );
    vec3f v110 = o + makevec3f(  l,  l, -l );
    vec3f v111 = o + makevec3f(  l,  l,  l );
    
    glsCheckError();
    glBegin(GL_TRIANGLES);
    glNormal3f(-1,0,0);
    glsVertex( v000 ); glsVertex( v001 ); glsVertex( v010 );
    glsVertex( v011 ); glsVertex( v001 ); glsVertex( v010 );
    glNormal3f( 1,0,0);
    glsVertex( v100 ); glsVertex( v101 ); glsVertex( v110 );
    glsVertex( v111 ); glsVertex( v101 ); glsVertex( v110 );
    glNormal3f(0,-1,0);
    glsVertex( v000 ); glsVertex( v001 ); glsVertex( v100 );
    glsVertex( v101 ); glsVertex( v001 ); glsVertex( v100 );
    glNormal3f(0, 1,0);
    glsVertex( v010 ); glsVertex( v011 ); glsVertex( v110 );
    glsVertex( v111 ); glsVertex( v011 ); glsVertex( v110 );
    glNormal3f(0,0,-1);
    glsVertex( v000 ); glsVertex( v010 ); glsVertex( v100 );
    glsVertex( v110 ); glsVertex( v010 ); glsVertex( v100 );
    glNormal3f(0,0, 1);
    glsVertex( v001 ); glsVertex( v011 ); glsVertex( v101 );
    glsVertex( v111 ); glsVertex( v011 ); glsVertex( v101 );
    glEnd();
    glsCheckError();
}
inline void draw_cube_edges( const vec3f &o, float l ) {
    l /= 2.0f;
    vec3f v000 = {o.x - l,o.y - l,o.z - l};
    vec3f v001 = {o.x - l,o.y - l,o.z + l};
    vec3f v010 = {o.x - l,o.y + l,o.z - l};
    vec3f v011 = {o.x - l,o.y + l,o.z + l};
    vec3f v100 = {o.x + l,o.y - l,o.z - l};
    vec3f v101 = {o.x + l,o.y - l,o.z + l};
    vec3f v110 = {o.x + l,o.y + l,o.z - l};
    vec3f v111 = {o.x + l,o.y + l,o.z + l};
    
    glsCheckError();
    glBegin(GL_LINES);
    glsVertex(v000); glsVertex(v001);
    glsVertex(v010); glsVertex(v011);
    glsVertex(v100); glsVertex(v101);
    glsVertex(v110); glsVertex(v111);
    
    glsVertex(v000); glsVertex(v010);
    glsVertex(v001); glsVertex(v011);
    glsVertex(v100); glsVertex(v110);
    glsVertex(v101); glsVertex(v111);
    
    glsVertex(v000); glsVertex(v100);
    glsVertex(v001); glsVertex(v101);
    glsVertex(v010); glsVertex(v110);
    glsVertex(v011); glsVertex(v111);
    glEnd();
    glsCheckError();
}

inline void draw_cube( const vec3f &o, float l, float rot ) {
    l /= 2.0f;
    vec3f r = normalize(makevec3f( 0.5f, 0.5f, 0.5f ));
    frame3f f = rotationframe3f( r, rot );
    
    vec3f v000 = o + ((- f.x - f.y - f.z)*l);
    vec3f v001 = o + ((- f.x - f.y + f.z)*l);
    vec3f v010 = o + ((- f.x + f.y - f.z)*l);
    vec3f v011 = o + ((- f.x + f.y + f.z)*l);
    vec3f v100 = o + ((  f.x - f.y - f.z)*l);
    vec3f v101 = o + ((  f.x - f.y + f.z)*l);
    vec3f v110 = o + ((  f.x + f.y - f.z)*l);
    vec3f v111 = o + ((  f.x + f.y + f.z)*l);
    
    glsCheckError();
    glBegin(GL_TRIANGLES);
    glsNormal( -f.x );
    glsVertex( v000 ); glsVertex( v001 ); glsVertex( v010 );
    glsVertex( v001 ); glsVertex( v010 ); glsVertex( v011 );
    glsNormal( f.x );
    glsVertex( v100 ); glsVertex( v101 ); glsVertex( v110 );
    glsVertex( v101 ); glsVertex( v110 ); glsVertex( v111 );
    glsNormal( -f.y );
    glsVertex( v000 ); glsVertex( v001 ); glsVertex( v100 );
    glsVertex( v001 ); glsVertex( v100 ); glsVertex( v101 );
    glsNormal( f.y );
    glsVertex( v010 ); glsVertex( v011 ); glsVertex( v110 );
    glsVertex( v011 ); glsVertex( v110 ); glsVertex( v111 );
    glsNormal( -f.z );
    glsVertex( v000 ); glsVertex( v010 ); glsVertex( v100 );
    glsVertex( v010 ); glsVertex( v100 ); glsVertex( v110 );
    glsNormal( f.z );
    glsVertex( v001 ); glsVertex( v011 ); glsVertex( v101 );
    glsVertex( v011 ); glsVertex( v101 ); glsVertex( v111 );
    glEnd();
    glsCheckError();
}
inline void draw_cube_edges( const vec3f &o, float l, float rot ) {
    l /= 2.0f;
    vec3f r = normalize(makevec3f( 0.5f, 0.5f, 0.5f ));
    frame3f f = rotationframe3f( r, rot );
    
    vec3f v000 = o + ((- f.x - f.y - f.z)*l);
    vec3f v001 = o + ((- f.x - f.y + f.z)*l);
    vec3f v010 = o + ((- f.x + f.y - f.z)*l);
    vec3f v011 = o + ((- f.x + f.y + f.z)*l);
    vec3f v100 = o + ((  f.x - f.y - f.z)*l);
    vec3f v101 = o + ((  f.x - f.y + f.z)*l);
    vec3f v110 = o + ((  f.x + f.y - f.z)*l);
    vec3f v111 = o + ((  f.x + f.y + f.z)*l);
    
    glsCheckError();
    glBegin(GL_LINES);
    glsVertex(v000); glsVertex(v001);
    glsVertex(v010); glsVertex(v011);
    glsVertex(v100); glsVertex(v101);
    glsVertex(v110); glsVertex(v111);
    
    glsVertex(v000); glsVertex(v010);
    glsVertex(v001); glsVertex(v011);
    glsVertex(v100); glsVertex(v110);
    glsVertex(v101); glsVertex(v111);
    
    glsVertex(v000); glsVertex(v100);
    glsVertex(v001); glsVertex(v101);
    glsVertex(v010); glsVertex(v110);
    glsVertex(v011); glsVertex(v111);
    glEnd();
    glsCheckError();
}

inline void draw_sphere(const vec3f& o, float r, int ur = 64, int vr = 32) {
    glsCheckError();
    glPushMatrix();
    glTranslatef(o.x, o.y, o.z);
    glScalef(r, r, r);
    draw_unitsphere(ur,vr);
    glPopMatrix();
    glsCheckError();
    // TODO
}
inline void draw_sphere_lines(const vec3f& o, float r, int ul = 16, int vl = 8, int ur = 64, int vr = 32) {
    glsCheckError();
    glPushMatrix();
    glTranslatef(o.x, o.y, o.z);
    glScalef(r, r, r);
    draw_unitsphere_lines(ul,vl,ur,vr);
    glPopMatrix();
    glsCheckError();
    // TODO
}
inline void draw_cylinder(float r, float h, int ur = 64, int vr = 32) {
    glsCheckError();
    glPushMatrix();
    glScalef(r, r, h);
    draw_unitcylinder(ur, vr);
    glPopMatrix();
    glsCheckError();
    // TODO
}
inline void draw_cylinder_lines(float r, float h, int ul = 16, int vl = 8, int ur = 64, int vr = 32) {
    glsCheckError();
    glPushMatrix();
    glScalef(r, r, h);
    draw_unitcylinder_lines(ul,vl,ur,vr);
    glPopMatrix();
    glsCheckError();
    // TODO
}
#if 0
inline void draw_cylinder(const frame3f f, float r, float h, int ur = 64, int vr = 32) {
    glsCheckError();
    glPushMatrix();
    glsMultMatrix(frame_to_matrix(f));
    draw_cylinder(r,h,ur,vr);
    glPopMatrix();
    glsCheckError();
}
inline void draw_cylinder_lines(const frame3f f, float r, float h, int ul = 16, int vl = 8, int ur = 64, int vr = 32) {
    glsCheckError();
    glPushMatrix();
    glsMultMatrix(frame_to_matrix(f));
    draw_cylinder_lines(r,h,ul,vl,ur,vr);
    glPopMatrix();
    glsCheckError();
}
inline void draw_cylinder(const vec3f& v0, const vec3f& v1, float r, int ur = 64, int vr = 32) {
    frame3f f;
    f.o = v0;
    f.z = normalize(v1-v0);
    f = orthonormalize(f);
    auto h = length(v1-v0);
    draw_cylinder(f,r,h,ur,vr);
}
#endif

const vec3f color_grid = makevec3f(0.3,0.3,0.3);
const vec3f color_axis = makevec3f(0,0,0);
const vec3f color_axis_x = makevec3f(0.5,0.125,0.125);
const vec3f color_axis_y = makevec3f(0.125,0.5,0.125);
const vec3f color_axis_z = makevec3f(0.125,0.125,0.5);

inline void draw_axis(float size = 2, float linewidth = 2.0) {
    glsCheckError();
    glLineWidth(linewidth);
    glsColor(color_axis_x);
    draw_line(zero3f,x3f*(size/2.0f));
    glsColor(color_axis_y);
    draw_line(zero3f,y3f*(size/2.0f));
    glsColor(color_axis_z);
    draw_line(zero3f,z3f*(size/2.0f));
    glsCheckError();
}

inline void draw_grids(float size = 2) {
    glsCheckError();
    glLineWidth(1);
    glsColor(makevec3f(0.3,0.3,0.3));
    //draw_grid(zero3f,x3f,z3f,size);
    draw_grid(size);
    // draw_grid(zero3f,y3f,z3f,size);
    
    glLineWidth(2);
    glsColor(color_axis_x);
    draw_line(zero3f,(size/2)*x3f);
    glsColor(color_axis_y);
    draw_line(zero3f,(size/2)*y3f);
    glsColor(color_axis_z);
    draw_line(zero3f,(size/2)*z3f);
    glsCheckError();
}

inline void draw_arrow( const vec3f &from, const vec3f &to, float sz_stem_max=0.0, float sz_head_max=0.0 ) {
    vec3f dir = to - from;
    float l = length(dir);
    dir *= ( 1.0f / l );
    vec3f dir_r = -dir;
    vec3f perp = normalize( cross( dir, normalize(makevec3f( dir.y, dir.x-0.0001f, 0.0f )) ) );
    vec3f perp2 = cross(perp,dir);
    //vec3f perp = cross( perp2, dir );
    
    float sz_head = l * 0.3f;
    if( sz_head_max > 0.0 ) sz_head = min(sz_head,sz_head_max);
    vec3f head0 = to + dir_r * sz_head;
    vec3f head00 = (-perp)*(sz_head*0.4f);
    vec3f head01 = (-perp2)*(sz_head*0.4f);
    vec3f head11 = (perp)*(sz_head*0.4f);
    vec3f head10 = (perp2)*(sz_head*0.4f);
    
    float sz_stem = sz_head * 0.15f;
    if( sz_stem_max > 0.0 ) sz_stem = min(sz_stem,sz_stem_max);
    vec3f stem00 = (-perp-perp2)*sz_stem;
    vec3f stem01 = (-perp+perp2)*sz_stem;
    vec3f stem11 = (perp+perp2)*sz_stem;
    vec3f stem10 = (perp-perp2)*sz_stem;
    
    glPushAttrib(GL_ENABLE_BIT);
    glDisable(GL_CULL_FACE);
    glBegin(GL_TRIANGLES);
    if( false ) {
        glsVertex(from+stem00); glsVertex(from+stem01); glsVertex(head0+stem00);
        glsVertex(from+stem01); glsVertex(head0+stem00); glsVertex(head0+stem01);
        glsVertex(from+stem01); glsVertex(from+stem11); glsVertex(head0+stem01);
        glsVertex(from+stem11); glsVertex(head0+stem01); glsVertex(head0+stem11);
        glsVertex(from+stem11); glsVertex(from+stem10); glsVertex(head0+stem11);
        glsVertex(from+stem10); glsVertex(head0+stem11); glsVertex(head0+stem10);
        glsVertex(from+stem10); glsVertex(from+stem00); glsVertex(head0+stem10);
        glsVertex(from+stem00); glsVertex(head0+stem10); glsVertex(head0+stem00);
    } else if(false) {
        glsVertex(from); glsVertex(head0+stem00); glsVertex(head0+stem01);
        glsVertex(from); glsVertex(head0+stem01); glsVertex(head0+stem11);
        glsVertex(from); glsVertex(head0+stem11); glsVertex(head0+stem10);
        glsVertex(from); glsVertex(head0+stem10); glsVertex(head0+stem00);
    } else {
        vec3f stem002 = (-perp-perp2)*(sz_stem*0.5f);
        vec3f stem012 = (-perp+perp2)*(sz_stem*0.5f);
        vec3f stem112 = (perp+perp2)*(sz_stem*0.5f);
        vec3f stem102 = (perp-perp2)*(sz_stem*0.5f);
        glsVertex(from+stem002); glsVertex(from+stem012); glsVertex(head0+stem00);
        glsVertex(from+stem012); glsVertex(head0+stem00); glsVertex(head0+stem01);
        glsVertex(from+stem012); glsVertex(from+stem112); glsVertex(head0+stem01);
        glsVertex(from+stem112); glsVertex(head0+stem01); glsVertex(head0+stem11);
        glsVertex(from+stem112); glsVertex(from+stem102); glsVertex(head0+stem11);
        glsVertex(from+stem102); glsVertex(head0+stem11); glsVertex(head0+stem10);
        glsVertex(from+stem102); glsVertex(from+stem002); glsVertex(head0+stem10);
        glsVertex(from+stem002); glsVertex(head0+stem10); glsVertex(head0+stem00);
    }
    
    glsVertex(head0+head00); glsVertex(head0+head01); glsVertex(to);
    glsVertex(head0+head01); glsVertex(head0+head11); glsVertex(to);
    glsVertex(head0+head11); glsVertex(head0+head10); glsVertex(to);
    glsVertex(head0+head10); glsVertex(head0+head00); glsVertex(to);
    glEnd();
    glPopAttrib();
}

inline void draw_arrow( const vec3f &from, const vec3f &to, const vec3f &view, float sz_stem_max=0.05f, float sz_head_max=0.5f ) {
    vec3f dir = to - from;
    float l = length(dir);
    dir /= l;
    vec3f dir_r = -dir;
    float dd = dot(view,dir);
    vec3f perp = normalize( cross( dir, view ) );
    
    float sz_head = l * ( 0.7f - 0.4f * fabs(dd) ); sz_head = min( sz_head_max,sz_head);
    float sz_head2 = sz_head*0.4f;
    float sz_stem = sz_head * 0.15f; sz_stem = min( sz_stem_max, sz_stem );
    
    vec3f head = dir_r*sz_head + to;
    vec3f head0 = perp*(-sz_head2);
    vec3f head1 = perp*(sz_head2);
    
    vec3f stem00 = perp*(-sz_stem*0.5f);
    vec3f stem01 = perp*(sz_stem*0.5f);
    vec3f stem10 = perp*(-sz_stem);
    vec3f stem11 = perp*sz_stem;
    
    glPushAttrib(GL_ENABLE_BIT);
    glDisable(GL_CULL_FACE);
    glBegin(GL_TRIANGLES);
    
    glsVertex(from+stem00); glsVertex(from+stem01); glsVertex(head+stem11);
    glsVertex(from+stem00); glsVertex(head+stem10); glsVertex(head+stem11);
    glsVertex(head+head0); glsVertex(head+head1); glsVertex(to);
    glEnd();
    glPopAttrib();
}
inline void draw_arrow_edges( const vec3f &from, const vec3f &to, const vec3f &view, float sz_stem_max=0.05f, float sz_head_max=0.5f ) {
    vec3f dir = to - from;
    float l = length(dir);
    dir /= l;
    vec3f dir_r = -dir;
    float dd = dot(view,dir);
    vec3f perp = normalize( cross( dir, view ) );
    
    float sz_head = l * ( 0.7f - 0.4f * fabs(dd) ); sz_head = min( sz_head_max,sz_head);
    float sz_head2 = sz_head*0.4f;
    float sz_stem = sz_head * 0.15f; sz_stem = min( sz_stem_max, sz_stem );
    
    vec3f head = dir_r*sz_head + to;
    vec3f head0 = perp*(-sz_head2);
    vec3f head1 = perp*(sz_head2);
    
    vec3f stem00 = perp*(-sz_stem*0.5f);
    vec3f stem01 = perp*(sz_stem*0.5f);
    vec3f stem10 = perp*(-sz_stem);
    vec3f stem11 = perp*sz_stem;
    
    glBegin(GL_LINE_STRIP);
    
    glsVertex(from+stem00); glsVertex(from+stem01); glsVertex(head+stem11);
    glsVertex(head+head1); glsVertex(to); glsVertex(head+head0);
    glsVertex(head+stem10); glsVertex(from+stem00);
    glEnd();
}

inline void draw_axis_gizmo(int x, int y, int w, int h) {
    glsCheckError();
    glPushAttrib(GL_VIEWPORT_BIT);
    
    glViewport(x, y, w, h);
    
    glLineWidth(2);
    glsColor(color_axis_x);
    draw_line(zero3f,x3f);
    glsColor(color_axis_y);
    draw_line(zero3f,y3f);
    glsColor(color_axis_z);
    draw_line(zero3f,z3f);
    
    glPopAttrib();
    glsCheckError();
}

inline void draw_light(int idx, const vec4f& pos, const vec4f& ka, const vec4f& kd, const vec4f& ks) {
    glsCheckError();
    glEnable(idx);
    glLightfv(idx, GL_POSITION, &pos.x);
    glLightfv(idx, GL_AMBIENT, &ka.x);
    glLightfv(idx, GL_DIFFUSE, &kd.x);
    glLightfv(idx, GL_SPECULAR, &ks.x);
    glsCheckError();
}

inline void draw_ambient_light(const vec4f& ka) {
    glsCheckError();
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, &ka.x);
    glsCheckError();
}

inline void draw_material(const vec4f& ke, const vec4f& ka, const vec4f& kd, const vec4f& ks, float n) {
    glsCheckError();
    glMaterialfv(GL_FRONT_AND_BACK,GL_EMISSION,&ke.x);
    glMaterialfv(GL_FRONT_AND_BACK,GL_AMBIENT,&ka.x);
    glMaterialfv(GL_FRONT_AND_BACK,GL_DIFFUSE,&kd.x);
    glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,&ks.x);
    glMaterialf(GL_FRONT_AND_BACK,GL_SHININESS,n);
    glsCheckError();
}

inline void draw_material(const vec3f& ke, const vec3f& ka, const vec3f& kd, const vec3f& ks, float n) {
    auto _ke = makevec4f(ke.x,ke.y,ke.z,1);
    auto _ka = makevec4f(ka.x,ka.y,ka.z,1);
    auto _kd = makevec4f(kd.x,kd.y,kd.z,1);
    auto _ks = makevec4f(ks.x,ks.y,ks.z,1);
    draw_material(_ke, _ka, _kd, _ks, n);
}

template<typename F, typename P, typename N, typename T>
inline void draw_faces(const vector<F>& face,
                       const vector<P>& pos, const vector<N>& norm,
                       const vector<T>& uv, const vector<vec3f>& col) {
    warning_if_not(not face.empty(), "face should not be empty");
    warning_if_not(not pos.empty(), "pos should not be empty");
    
    glsCheckError();
    
    glPushClientAttrib(GL_CLIENT_VERTEX_ARRAY_BIT);
    
    //glColorMaterial( GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE | GL_SPECULAR );
    //glEnable( GL_COLOR_MATERIAL );
    
    if(not pos.empty()) { glEnableClientState(GL_VERTEX_ARRAY); glsVertexPointer(pos); }
    if(not norm.empty()) { glEnableClientState(GL_NORMAL_ARRAY); glsNormalPointer(norm); }
    if(not uv.empty()) { glEnableClientState(GL_TEXTURE_COORD_ARRAY); glsTexCoordPointer(uv); }
    if(not col.empty()) {
        glEnableClientState(GL_COLOR_ARRAY); glsColorPointer(col);
    } else {
        glColor3f(0.5,0.5,0.5);
    }
    
    glsDrawElements(face);
    
    glPopClientAttrib();

    glsCheckError();
}

inline void draw_string(const string& s) {
    for(auto c : s) glutBitmapCharacter( GLUT_BITMAP_8_BY_13, c);
}
inline void draw_string(const char *str) {
    for( int i = 0; str[i] != 0; i++ ) glutBitmapCharacter(GLUT_BITMAP_8_BY_13, str[i] );
}

inline void draw_string(float x, float y, const string& s) {
    glRasterPos2f(x,y);
    draw_string(s);
}


#endif
