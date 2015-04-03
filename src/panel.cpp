#include "panel.h"


#define PIf 3.14159265f

inline float dist(const vec3f& a, const vec3f& b) { return length(a-b); }
inline float distSqr(const vec3f& a, const vec3f& b) { return lengthSqr(a-b); }
inline float clamp(float x, float m, float M) { return min(M,max(m,x)); }
inline void orthonormalize(vec3f& x, vec3f& y, vec3f& z) { z = normalize(z); x = normalize(cross(y,z)); y = normalize(cross(z,x)); }
inline frame3f orthonormalize(const frame3f& f) { frame3f ret = f; orthonormalize(ret.x, ret.y, ret.z); return ret; }

Viewer *viewer = nullptr;


void ViewPanel::do_mouse_down( int mx, int my, int mb ) {
    mouse_cur = makevec2i( mx, my );
    mouse_but |= mb;
    mouse_mod = glutGetModifiers();
    
    if( mouse_action == mouse_none && mouse_but == MOUSE_MBUTTON ) {
        if( mouse_mod & GLUT_ACTIVE_SHIFT ) {
            mouse_action = mouse_turntable_pan;
        } else if( mouse_mod & GLUT_ACTIVE_CTRL ) {
            mouse_action = mouse_turntable_dolly;
        } else {
            mouse_action = mouse_turntable_rotate;
        }
        if( camera ) camera->handle_mouse_down( mx, my, mouse_action );
    }
    
    _do_mouse_down();
    
    mouse_pre = mouse_cur;
}

void ViewPanel::do_mouse_up( int mx, int my, int mb ) {
    mouse_cur = makevec2i( mx, my );
    mouse_but &= ~mb;
    
    if( mouse_action != mouse_none && (mouse_but & MOUSE_MBUTTON) == 0 ) {
        if( camera ) camera->handle_mouse_up();
        mouse_action = mouse_none;
    }
    
    _do_mouse_up();
    
    mouse_pre = mouse_cur;
}

void ViewPanel::do_mouse_move( int mx, int my ) {
    mouse_cur = makevec2i( mx, my );
    if( mouse_action != mouse_none ) {
        if( camera ) camera->handle_mouse_move( mx, my );
    }
    
    _do_mouse_move();
    
    mouse_pre = mouse_cur;
}

void ViewPanel::do_keyboard( unsigned char key, int mx, int my ) {
    mouse_cur = makevec2i( mx, my );
    this->key = key;
    this->key_mod = glutGetModifiers();
    
    if( camera ) {
        float mult = ( key_mod & GLUT_ACTIVE_SHIFT ) ? -1 : 1;
        
        switch( key ) {
            case '.': {
                if( key_mod == 0 || key_mod == GLUT_ACTIVE_SHIFT ) {
                    vec3f e = camera->f.o;
                    vec3f u = camera->f.y;
                    vec3f b = camera->f.z;
                    vec3f f = e + camera->l * b;
                    
                    vec3f e_n = f - e;
                    vec3f f_n = zero3f;
                    vec3f u_n = u;
                    
                    if( key_mod & GLUT_ACTIVE_SHIFT ) camera->flyto(e_n,f_n,u_n);
                    else camera->lookat(e_n,f_n,u_n);
                }
            } break;
            case '5': {
                camera->orthographic = !camera->orthographic;
            } break;
            case '1': {
                vec3f e = camera->f.o;
                vec3f f = e - camera->l * camera->f.z;
                
                vec3f f_n = f;
                vec3f e_n = f - (mult*camera->l * y3f);
                vec3f u_n = z3f;
                camera->lookat(e_n,f_n,u_n);
            } break;
            case '3': {
                vec3f e = camera->f.o;
                vec3f f = e - camera->l * camera->f.z;
                
                vec3f f_n = f;
                vec3f e_n = f + (mult*camera->l * x3f);
                vec3f u_n = z3f;
                camera->lookat(e_n,f_n,u_n);
            } break;
            case '7': {
                vec3f e = camera->f.o;
                vec3f f = e - camera->l * camera->f.z;
                
                vec3f f_n = f;
                vec3f e_n = f + (mult*camera->l * z3f);
                vec3f u_n = mult*y3f;
                camera->lookat(e_n,f_n,u_n);
            } break;
            default: break;
        }
    }
    
    _do_keyboard( );
    
    mouse_pre = mouse_cur;
}

void ViewPanel::do_keyspecial( int keysp, int mx, int my ) {
    mouse_cur = makevec2i( mx, my );
    this->keysp = keysp;
    
    if( camera ) {
        switch( keysp ) {
            case GLUT_KEY_LEFT: {
                camera->turntable_rotate( -0.1f, 0.0f );
            } break;
            case GLUT_KEY_RIGHT: {
                camera->turntable_rotate( 0.1f, 0.0f );
            } break;
        }
    }
    
    _do_keyspecial();
    
    mouse_pre = mouse_cur;
}


void VP_ScrollTextView::_draw() {
    float height = s_display.size()*c_h;
    float top = i_scroll;
    float bot = min( height, i_scroll+(float)h );
    bool b_active = height>h;
    
    glPushAttrib(GL_ENABLE_BIT);
    glDisable(GL_LIGHTING); glDisable(GL_LINE_SMOOTH); glDisable(GL_DEPTH_TEST); glDisable(GL_CULL_FACE);
    glLineWidth( 1.0f );
    glBegin(GL_LINES);
    glColor3f(0.1,0.1,0.1); glVertex2f( w-5, 3 ); glVertex2f( w-5, h-3 );
    if( b_active ) glColor3f(1.0,1.0,1.0);
    else glColor3f(0.5,0.5,0.5);
    glVertex2f( w-5, 3.0f + top / height * (float)(h-6) );
    glVertex2f( w-5, 3.0f + bot / height * (float)(h-6) );
    glEnd();
    glPopAttrib();
}

void VP_PropertyView::_draw() {
    int width = (int)((float)(w-10.0) / (float)c_w);
    
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0,w,h,0);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    
    glPushAttrib(GL_ENABLE_BIT);
    glDisable(GL_LIGHTING); glDisable(GL_LINE_SMOOTH); glDisable(GL_DEPTH_TEST); glDisable(GL_CULL_FACE);
    
    int n = properties.size();
    int c_y = (int)(-i_scroll);
    for( int i = 0; i < n; i++ ) {
        Property *prop = properties[i];
        if( i_over == i && prop->changable ) {
            if( active ) glColor3f( 0.1, 0.1, 0.3 );
            else glColor3f( 0.1, 0.1, 0.1 );
            glBegin( GL_QUADS );
            glVertex2f( 0, c_y );
            glVertex2f( w-10.0f, c_y );
            glVertex2f( w-10.0f, c_y+c_h );
            glVertex2f( 0, c_y+c_h );
            glEnd();
        }
        glsColor(prop->color);
        glRasterPos2f( 0, 10+c_y );
        draw_string( prop->as_str( width ) );
        c_y += c_h;
    }
    
    float height = properties.size()*c_h;
    float top = i_scroll;
    float bot = min( height, i_scroll+(float)h );
    bool b_active = height>h;
    
    glLineWidth( 1.0f );
    glBegin(GL_LINES);
    glColor3f(0.1,0.1,0.1); glVertex2f( w-5, 3 ); glVertex2f( w-5, h-3 );
    if( b_active ) glColor3f(1.0,1.0,1.0);
    else glColor3f(0.5,0.5,0.5);
    glVertex2f( w-5, 3.0f + top / height * (float)(h-6) );
    glVertex2f( w-5, 3.0f + bot / height * (float)(h-6) );
    glEnd();
    
    glPopAttrib();
    
    glMatrixMode(GL_PROJECTION); glPopMatrix();
    glMatrixMode(GL_MODELVIEW); glPopMatrix();
}


void VP_Text::_draw() {
    //if(!b_display) return;
    if( w <= 0 || h <= 0 ) return;
    
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0,w,h,0);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    
    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    
    int c_x = v_display.x+2.0f, c_y = v_display.y+10;
    vec4f c_text = { c_display.x, c_display.y, c_display.z, 1.0f };
    vec4f c_bg = { 0.0f, 0.0f, 0.0f, 0.2f };
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
    for( string &s : s_display ) {
        // draw shadow
        glsColor(c_bg);
        if( false ) {
            glRasterPos2f(c_x-1.0f,c_y+1.0f); draw_string(s);
            glRasterPos2f(c_x+0.0f,c_y+1.0f); draw_string(s);
            glRasterPos2f(c_x+1.0f,c_y+1.0f); draw_string(s);
            glRasterPos2f(c_x-1.0f,c_y+0.0f); draw_string(s);
            //glRasterPos2f(c_x+0.0f,c_y+0.0f); draw_string(s);
            glRasterPos2f(c_x+1.0f,c_y+0.0f); draw_string(s);
            glRasterPos2f(c_x-1.0f,c_y-1.0f); draw_string(s);
            glRasterPos2f(c_x+0.0f,c_y-1.0f); draw_string(s);
            glRasterPos2f(c_x+1.0f,c_y-1.0f); draw_string(s);
        } else {
            glRasterPos2f(c_x+0.0f,c_y+1.0f); draw_string(s);
            glRasterPos2f(c_x+1.0f,c_y+1.0f); draw_string(s);
            glRasterPos2f(c_x+1.0f,c_y+0.0f); draw_string(s);
        }
        // draw text
        glsColor(c_text);
        glRasterPos2f(c_x,c_y); draw_string(s); c_y += c_h;
    }
    glDisable(GL_BLEND);
    
    glMatrixMode(GL_PROJECTION); glPopMatrix();
    glMatrixMode(GL_MODELVIEW); glPopMatrix();
}





