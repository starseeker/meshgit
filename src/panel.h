#ifndef _PANEL_H_
#define _PANEL_H_

#include "debug.h"
#include "vec.h"
#include "object.h"
#include "std.h"
#include "geom.h"
#include "gls.h"
#include "draw_utils.h"
#include "light.h"
#include "ext/common/frame.h"
#include "std_utils.h"
#include "camera.h"

#include "panelbackground.h"
#include "property.h"
#include "gls.h"
#include "std.h"

using namespace std;

struct Shape;
struct TriangleMesh;

struct ViewPanel;
struct VP_CallBacks;
struct Viewer;

extern Viewer *viewer;

//typedef void (*fn_callback)(VP_CallBacks *panel);
typedef function<void(VP_CallBacks*)> fn_callback;





struct ViewPanel {
    int x, y, w, h;
    string name;
    PanelBackground *background = pbg_Black;
    VPCamera *camera = nullptr;
    
    const int c_w = 8, c_h = 13;
    vector<string> s_display; bool b_display = false;
    float i_scroll = 0.0f;
    vec3f c_display = {0.8f,0.8f,0.8f};
    vec2i v_display = {0,0};
    
    vec2i mouse_cur, mouse_pre;
    int mouse_but = 0, mouse_mod = 0;
    MouseAction mouse_action = mouse_none;
    unsigned char key; int keysp; int key_mod = 0;
    
    ViewPanel() { }
    ViewPanel( PanelBackground *background ) : background(background) { }
    ViewPanel( string name ) : name(name) { }
    ViewPanel( string name, PanelBackground *background ) : name(name), background(background) { }
    
    virtual void draw() {
        if( w <= 0 || h <= 0 ) return;
        
        glViewport( x, y, w, h );
        
        if( background ) background->draw();
        if( camera ) {
            camera->draw( w, h );
        } else {
            glMatrixMode(GL_PROJECTION);
            glLoadIdentity();
            gluOrtho2D(0,w,h,0);
            glMatrixMode(GL_MODELVIEW);
            glLoadIdentity();
        }
        
        _draw();
        
        if( s_display.size() && b_display ) {
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
            
            float c_x = v_display.x+2.0f, c_y = v_display.y+10-i_scroll;
            vec4f c_text = { c_display.x, c_display.y, c_display.z, 1.0f };
            vec4f c_bg = { 0.0f, 0.0f, 0.0f, 0.2f };
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
            for( string &s : s_display ) {
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
                    draw_string(c_x + 0.0f, c_y + 1.0f, s);
                    draw_string(c_x + 1.0f, c_y + 1.0f, s);
                    draw_string(c_x + 1.0f, c_y + 0.0f, s);
                }
                glsColor(c_text);
                draw_string(c_x, c_y, s); c_y += c_h;
            }
            glDisable(GL_BLEND);
            
            glMatrixMode(GL_PROJECTION); glPopMatrix();
            glMatrixMode(GL_MODELVIEW); glPopMatrix();
        }
    }
    
    void move( int x, int y, int w, int h ) {
        this->x = x; this->y = y; this->w = w; this->h = h;
        _move();
    }
    
    virtual ViewPanel *get_panel( int mx, int my ) { return this; }
    
    ray3f get_ray() {
        if( camera == nullptr ) { ray3f r; return r; }
        vec2i m = mouse_cur;
        float a = (float)w/(float)h;
        vec2f uv = makevec2f( (float)(m.x-x) / (float)w, (float)(m.y-y) / (float)h );
        return camera->trace_ray( uv, a );
    }
    ray3f get_ray(vec2i m) {
        if( camera == nullptr ) { ray3f r; return r; }
        float a = (float)w/(float)h;
        vec2f uv = makevec2f( (float)(m.x-x) / (float)w, (float)(m.y-y) / (float)h );
        return camera->trace_ray( uv, a );
    }
    ray3f get_ray(vec2f uv) {
        if( camera == nullptr ) { return ray3f(); }
        return camera->trace_ray( uv, (float)w/(float)h );
    }
    ray3f get_ray(float u, float v) {
        if( camera == nullptr ) { return ray3f(); }
        return camera->trace_ray( {u,v}, (float)w/(float)h );
    }
    
    void do_mouse_down( int mx, int my, int mb );
    void do_mouse_up( int mx, int my, int mb );
    void do_mouse_move( int mx, int my );
    void do_keyboard( unsigned char key, int mx, int my );
    void do_keyspecial( int keysp, int mx, int my );
    
    virtual void do_mouse_in() { glutSetCursor( GLUT_CURSOR_INHERIT ); }
    virtual void do_mouse_out() { glutSetCursor( GLUT_CURSOR_INHERIT ); }
    
    virtual void _draw() {};
    virtual void _move() { glutSetCursor( GLUT_CURSOR_INHERIT ); }
    virtual void _do_mouse_down() {}
    virtual void _do_mouse_up() {}
    virtual void _do_mouse_move() {}
    virtual void _do_keyboard() {}
    virtual void _do_keyspecial() {}
};

struct VP_ScrollTextView : ViewPanel {
    void _draw();
    
    VP_ScrollTextView() { b_display = true; }
    VP_ScrollTextView( PanelBackground *background ) : ViewPanel(background) { b_display = true; }
    VP_ScrollTextView( string name ) : ViewPanel(name) { b_display = true; }
    VP_ScrollTextView( string name, PanelBackground *background ) : ViewPanel(name,background) { b_display = true; }
    
    virtual void _do_mouse_move() {
        if( mouse_but == MOUSE_MBUTTON ) {
            i_scroll += (mouse_cur.y-mouse_pre.y);
            i_scroll = max(0.0f, i_scroll);
            i_scroll = min(i_scroll, max(0.0f,(float)s_display.size()*(float)c_h-(float)h) );
        }
    }
};



struct VP_PropertyView : ViewPanel {
    vector<Property *> properties;
    Property *active = nullptr;
    int val_width = 5;
    int i_over = -1;
    
#ifdef __APPLE__
    const int y_offset = -5;
#else
    const int y_offset = 0;
#endif
    
    VP_PropertyView() : ViewPanel("Properties",pbg_DarkGray) {}
    
    Property *add(Property*prop) { properties.push_back(prop); return prop; }
    
    void _draw();
    
    int get_i( int my ) {
        int i = ( h + y - (my-(int)i_scroll) +y_offset ) / c_h;
        return ( i < 0 || i >= properties.size() ) ? -1 : i;
    }
    Property *get_property( int my ) {
        int i = get_i(my);
        return ( i == -1 ) ? nullptr : properties[i];
    }
    
    virtual void do_mouse_out() {
        if( i_over == -1 ) return;
        
        properties[i_over]->do_mouse_out();
        i_over = -1;
    }
    virtual void _do_mouse_up() {
        if( active ) active->do_mouse_up( mouse_but );
        active = nullptr;
    }
    virtual void _do_mouse_down() {
        if( active ) return;
        if( mouse_but == MOUSE_LBUTTON || mouse_but == MOUSE_RBUTTON ) {
            Property *click = get_property( mouse_cur.y );
            if( click && click->changable ) {
                active = click;
                active->do_mouse_down( mouse_but );
            }
        }
    }
    virtual void _do_mouse_move() {
        if( mouse_but == MOUSE_MBUTTON ) {
            i_scroll += (mouse_cur.y-mouse_pre.y);
            i_scroll = max(0.0f, i_scroll);
            i_scroll = min(i_scroll, max(0.0f,(float)properties.size()*(float)c_h-(float)h) );
        } else {
            if( active ) active->do_mouse_move( mouse_cur - mouse_pre );
            else {
                if( i_over != -1 ) properties[i_over]->do_mouse_out();
                i_over = get_i(mouse_cur.y);
                if( i_over != -1 ) properties[i_over]->do_mouse_in();
            }
        }
    }
    
    virtual void _move() {
        i_scroll = max(0.0f, i_scroll);
        i_scroll = min(i_scroll, max(0.0f,(float)properties.size()*(float)c_h-(float)h) );
    }
    
    virtual void _do_keyboard() {
        if( active ) {
            active->do_keyboard( key );
        } else if( i_over != -1 ) {
            properties[i_over]->do_keyboard( key );
        }
    }
    
    virtual void _do_keyspecial() {
        if( active ) {
            active->do_keyspecial( keysp );
        } else if( i_over != -1 ) {
            properties[i_over]->do_keyspecial( keysp );
        }
    }
};


struct VP_CallBacks : ViewPanel {
    fn_callback fndraw = nullptr;
    fn_callback fnmousedown = nullptr;
    fn_callback fnmouseup = nullptr;
    fn_callback fnmousemove = nullptr;
    fn_callback fnkeyboard = nullptr;
    fn_callback fnkeyspecial = nullptr;
    fn_callback fnmouseout = nullptr;
    fn_callback fnmousein = nullptr;
    
    function<void()> lfn_draw = nullptr;
    function<void()> lfn_move = nullptr;
    
    void *data = nullptr;
    
    virtual void _draw() {
        glsCheckError();
        if( fndraw ) fndraw( this );
        if( lfn_draw ) lfn_draw();
        glsCheckError();
    }
    
    virtual void _move() {
        if( lfn_move ) lfn_move();
    }
    
    virtual void do_mouse_out() { if( fnmouseout ) fnmouseout(this); }
    virtual void do_mouse_in() { if( fnmousein ) fnmousein(this); }
    
    virtual void _do_mouse_down() { if( fnmousedown ) fnmousedown(this); }
    virtual void _do_mouse_up() { if( fnmouseup ) fnmouseup(this); }
    virtual void _do_mouse_move() { if( fnmousemove ) fnmousemove(this); }
    virtual void _do_keyboard() { if( fnkeyboard ) fnkeyboard(this); }
    virtual void _do_keyspecial() { if( fnkeyspecial ) fnkeyspecial(this); }
};


struct VP_Container : ViewPanel {
    ViewPanel *panel = nullptr;
    
    VP_Container() { }
    VP_Container( string name, ViewPanel *panel ) : ViewPanel(name), panel(panel) { }
    
    virtual void do_mouse_out() { if(panel) panel->do_mouse_out(); }
    virtual void do_mouse_in() { if(panel) panel->do_mouse_in(); }
    
    virtual void draw() { if(panel) panel->draw(); };
    virtual void _move() { if(panel) panel->move(x,y,w,h); }
    virtual void _do_mouse_down() { if(panel) panel->_do_mouse_down(); }
    virtual void _do_mouse_up() { if(panel) panel->_do_mouse_up(); }
    virtual void _do_mouse_move() { if(panel) panel->_do_mouse_move(); }
    virtual void _do_keyboard() { if(panel) panel->_do_keyboard(); }
    virtual void _do_keyspecial() { if(panel) panel->_do_keyspecial(); }
    
    virtual ViewPanel *get_panel( int mx, int my ) { if(panel) return panel->get_panel(mx,my); return this; }
};

struct VP_SwitchableContainer : ViewPanel {
    vector<ViewPanel*> panels;
    int i_panel = 0;
    
    VP_SwitchableContainer() { }
    VP_SwitchableContainer( string name, vector<ViewPanel*> &panels, int i_panel ) :
    ViewPanel(name), panels(panels), i_panel(i_panel) { }
    
    ViewPanel *panel() {
        i_panel = max( 0, min( (int)panels.size()-1, i_panel ) );
        if( i_panel == panels.size() ) return nullptr;
        return panels[i_panel];
    }
    
    virtual void do_mouse_out() { auto p=panel(); if(p) p->do_mouse_out(); }
    virtual void do_mouse_in() { auto p=panel(); if(p) p->do_mouse_in(); }
    
    virtual void draw() { auto p=panel(); if(p) p->draw(); };
    virtual void _move() { for( auto p : panels ) if(p) p->move(x,y,w,h); }
    virtual void _do_mouse_down() { auto p=panel(); if(p) p->_do_mouse_down(); }
    virtual void _do_mouse_up() { auto p=panel(); if(p) p->_do_mouse_up(); }
    virtual void _do_mouse_move() { auto p=panel(); if(p) p->_do_mouse_move(); }
    virtual void _do_keyboard() { auto p=panel(); if(p) p->_do_keyboard(); }
    virtual void _do_keyspecial() { auto p=panel(); if(p) p->_do_keyspecial(); }
    
    virtual ViewPanel *get_panel( int mx, int my ) { auto p=panel(); if(p) return p->get_panel(mx,my); return this; }
};


enum SplitTypes { SPLIT_LEFT = 1, SPLIT_RIGHT = 2, SPLIT_TOP = 4, SPLIT_BOTTOM = 8 };
const int SPLIT_LEFTRIGHT = SPLIT_LEFT|SPLIT_RIGHT;
const int SPLIT_TOPBOTTOM = SPLIT_TOP|SPLIT_BOTTOM;


struct VP_Split : ViewPanel {
    ViewPanel *panel0;
    ViewPanel *panel1;
    PanelBackground *background = pbg_Gray;
    int border = 1, size=50; bool resizable = true;
    SplitTypes type = SPLIT_LEFT;
    
    VP_Split() { }
    
    VP_Split( string name, ViewPanel *panel0, ViewPanel *panel1, int size, SplitTypes type ) :
    ViewPanel(name), panel0(panel0), panel1(panel1), size(size), type(type) { }
    
    virtual void do_mouse_in() {
        if( !resizable ) return;
        if( type & SPLIT_LEFTRIGHT ) glutSetCursor( GLUT_CURSOR_LEFT_RIGHT );
        else glutSetCursor( GLUT_CURSOR_UP_DOWN );
    }
    
    virtual void do_mouse_out() {
        glutSetCursor( GLUT_CURSOR_INHERIT );
    }
    
    virtual void _draw() {
        if( panel0 ) panel0->draw();
        if( panel1 ) panel1->draw();
    }
    
    virtual void _move() {
        switch( type ) {
            case SPLIT_LEFT: {
                if( panel0 ) panel0->move( x, y, size-border, h );
                if( panel1 ) panel1->move( x+size+border, y, w-size-border, h );
            } break;
            case SPLIT_RIGHT: {
                if( panel0 ) panel0->move( x, y, w-size-border, h );
                if( panel1 ) panel1->move( x+w-size+border, y, size-border, h );
            } break;
            case SPLIT_TOP: {
                if( panel0 ) panel0->move( x, y+h-size+border, w, size-border );
                if( panel1 ) panel1->move( x, y, w, h-size-border );
            } break;
            case SPLIT_BOTTOM: {
                if( panel0 ) panel0->move( x, y+size+border, w, h-size-border );
                if( panel1 ) panel1->move( x, y, w, size-border );
            } break;
            default: break;
        }
    }
    
    virtual ViewPanel *get_panel( int mx, int my ) {
        ViewPanel *child = nullptr;
        switch( type ) {
            case SPLIT_LEFT: {
                if( mx-x < size-border ) child = panel0;
                if( mx-x >= size+border ) child = panel1;
            } break;
            case SPLIT_RIGHT: {
                if( mx-x < w-size-border ) child = panel0;
                if( mx-x >= w-size+border ) child = panel1;
            } break;
            case SPLIT_TOP: {
                if( my >= y+h-size+border ) child = panel0;
                if( my < y+h-size-border ) child = panel1;
            } break;
            case SPLIT_BOTTOM: {
                if( my >= y+size+border ) child = panel0;
                if( my < y+size-border ) child = panel1;
            } break;
            default: break;
        }
        if( child ) return child->get_panel( mx, my );
        return this;
    }
    
    virtual void _do_mouse_move() {
        if( mouse_but != MOUSE_LBUTTON || !resizable ) return;
        
        int s = 0;
        switch( type ) {
            case SPLIT_LEFT:   s = mouse_cur.x - mouse_pre.x; break;
            case SPLIT_RIGHT:  s = mouse_pre.x - mouse_cur.x; break;
            case SPLIT_TOP:    s = mouse_pre.y - mouse_cur.y; break;
            case SPLIT_BOTTOM: s = mouse_cur.y - mouse_pre.y; break;
            default: break;
        }
        size = min( ( type & SPLIT_LEFTRIGHT ? w : h ) - 5, max( 5, size+s ) );
        _move();
    }
};

namespace DivideType {
    enum Enum { LEFTRIGHT = 1, TOPBOTTOM = 2 };
};

struct VP_Divide : ViewPanel {
    ViewPanel *panel0;
    ViewPanel *panel1;
    PanelBackground *background = pbg_Gray;
    int border = 1;
    float div = 0.5f;
    DivideType::Enum type = DivideType::TOPBOTTOM;
    bool movable = true;
    
    VP_Divide() { }
    
    VP_Divide( string name, ViewPanel *panel0, ViewPanel *panel1, float div, DivideType::Enum type, bool movable ) :
    ViewPanel(name), panel0(panel0), panel1(panel1), div(div), type(type), movable(movable) { }
    
    virtual void do_mouse_in() {
        if( !movable ) return;
        if( type == DivideType::LEFTRIGHT ) glutSetCursor( GLUT_CURSOR_LEFT_RIGHT );
        else glutSetCursor( GLUT_CURSOR_UP_DOWN );
    }
    
    virtual void do_mouse_out() {
        glutSetCursor( GLUT_CURSOR_INHERIT );
    }
    
    virtual void _draw() {
        if( panel0 ) panel0->draw();
        if( panel1 ) panel1->draw();
    }
    
    virtual void _move() {
        switch( type ) {
            case DivideType::LEFTRIGHT: {
                int size = (int)((float)w*div);
                if( panel0 ) panel0->move( x, y, size-border, h );
                if( panel1 ) panel1->move( x+size+border, y, w-size-border-1, h );
            } break;
            case DivideType::TOPBOTTOM: {
                int size = (int)((float)h*div);
                if( panel0 ) panel0->move( x, y+h-size+border, w, size-border );
                if( panel1 ) panel1->move( x, y, w, h-size-border-1 );
            } break;
            default: break;
        }
    }
    
    virtual ViewPanel *get_panel( int mx, int my ) {
        ViewPanel *child = nullptr;
        switch( type ) {
            case DivideType::LEFTRIGHT: {
                int size = (int)((float)w*div);
                if( mx < x+size-border ) child = panel0;
                if( mx >= x+size+border ) child = panel1;
            } break;
            case DivideType::TOPBOTTOM: {
                int size = (int)((float)h*div);
                if( my >= y+h-size+border ) child = panel0;
                if( my < y+h-size-border ) child = panel1;
            } break;
            default: break;
        }
        if( child ) return child->get_panel( mx, my );
        return this;
    }
    
    virtual void _do_mouse_move() {
        if( mouse_but != MOUSE_LBUTTON || !movable ) return;
        
        switch( type ) {
            case DivideType::LEFTRIGHT: {
                div = (float)(mouse_cur.x - x) / (float)w;
            } break;
            case DivideType::TOPBOTTOM: {
                div = (float)(y - mouse_cur.y) / (float)h;
            } break;
            default: break;
        }
        div = max( 0.01f, min( 0.99f, div ) );
        _move();
    }
};

struct VP_Divide_N : ViewPanel {
    vector<ViewPanel*> panels;
    PanelBackground *background = pbg_Gray;
    int border = 1;
    DivideType::Enum type = DivideType::TOPBOTTOM;
    
    VP_Divide_N() { }
    
    VP_Divide_N( string name, vector<ViewPanel*> &panels, DivideType::Enum type ) :
    ViewPanel(name), panels(panels), type(type) { }
    
    virtual void _draw() {
        for( auto panel : panels ) if( panel ) panel->draw();
    }
    
    virtual void _move() {
        int n = panels.size();
        if( n == 0 ) return;
        switch( type ) {
            case DivideType::LEFTRIGHT: {
                int size = (int)floor((float)w/(float)n);
                int _x = 0;
                for( auto panel : panels ) {
                    if(panel) panel->move( x+_x, y, size-border, h );
                    _x += size;
                }
            } break;
            case DivideType::TOPBOTTOM: {
                int size = (int)floor((float)h/(float)n);
                int _y = 0;
                for( auto panel : panels ) {
                    if(panel) panel->move( x, y+_y, w, size-border );
                    _y += size;
                }
            } break;
            default: break;
        }
    }
    
    virtual ViewPanel *get_panel( int mx, int my ) {
        int n = panels.size();
        if( n == 0 ) return this;
        
        ViewPanel *child = nullptr;
        switch( type ) {
            case DivideType::LEFTRIGHT: {
                int size = (int)floor((float)w/(float)n);
                int i = min( n-1, (int)floor((float)(mx-x)/(float)size) );
                child = panels[i];
            } break;
            case DivideType::TOPBOTTOM: {
                int size = (int)floor((float)h/(float)n);
                int i = min( n-1, (int)floor((float)(my-y)/(float)size) );
                child = panels[i];
            } break;
            default: break;
        }
        if( child ) return child->get_panel( mx, my );
        return this;
    }
};

struct VP_HScrollbar : ViewPanel {
    float vmin = 0.0f;
    float vmax = 0.0f;
    float val = 0.0f;
    float vsz = 0.0f;
    
    vec3f c_line = {0.1,0.1,0.1};
    vec3f c_bar = {1.0,1.0,1.0};
    
    function<void(VP_HScrollbar*)> f_change = nullptr;
    
    VP_HScrollbar() : ViewPanel(pbg_DarkGray) { }
    VP_HScrollbar( string name, int vmin, int vmax, int val, int vsz ) : ViewPanel(name,pbg_DarkGray), vmin(vmin), vmax(vmax), val(val), vsz(vsz) { }
    
    virtual void _draw() {
        glMatrixMode(GL_PROJECTION);
        glPushMatrix();
        glLoadIdentity();
        gluOrtho2D(0,w,h,0);
        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
        glLoadIdentity();
        
        glPushAttrib(GL_ENABLE_BIT);
        glDisable(GL_LIGHTING); glDisable(GL_LINE_SMOOTH); glDisable(GL_DEPTH_TEST); glDisable(GL_CULL_FACE);
        glLineWidth( 1.0f );
        
        float xm = (float)(w-6) / (vmax-vmin);
        float xv0 = xm * (val-vmin) + 3.0f;
        float xv1 = xm * (val+vsz-vmin) + 3.0f;
        float x0 = xm * (vmin-vmin) + 3.0f;
        float x1 = xm * (vmax-vmin) + 3.0f;
        xv0 = max(x0,min(x1,xv0));
        xv1 = max(x0,min(x1,xv1));
        float y0 = h/2;
        float yhl = max( 1.0f, y0 - 3.0f );
        float yhb = max( 1.0f, y0 - 2.0f );
        
        glLineWidth( 1.0f );
        glBegin(GL_LINES);
        glsColor(c_line);
        for( int i = 0; i < yhl; i++ ) {
            glVertex2f(x0+i,y0-i); glVertex2f(x1-i,y0-i);
            glVertex2f(x0+i,y0+i); glVertex2f(x1-i,y0+i);
        }
        glsColor(c_bar);
        for( int i = 0; i < yhb; i++ ) {
            glVertex2f(xv0+i,y0-i); glVertex2f(xv1-i,y0-i);
            glVertex2f(xv0+i,y0+i); glVertex2f(xv1-i,y0+i);
        }
        glEnd();
        
        glPopAttrib();
        
        glMatrixMode(GL_PROJECTION); glPopMatrix();
        glMatrixMode(GL_MODELVIEW); glPopMatrix();
    }
    
    void handle_scroll( float xdelta, bool scale=true ) {
        float xm = (scale ? ((float)(w-6) / (float)(vmax-vmin)) : 1.0f );
        val = max(vmin,min(vmax-vsz,val+(xdelta/xm)));
        if( f_change ) f_change(this);
    }
    
    virtual void do_mouse_out() { glutSetCursor( GLUT_CURSOR_INHERIT ); }
    virtual void do_mouse_in() { glutSetCursor( GLUT_CURSOR_LEFT_RIGHT ); }
    virtual void _do_mouse_move() {
        if( mouse_but == MOUSE_LBUTTON || mouse_but == MOUSE_MBUTTON ) handle_scroll( mouse_cur.x-mouse_pre.x );
    }
    
    virtual void _do_keyspecial() {
        switch( keysp ) {
            case GLUT_KEY_HOME: {
                val = vmin;
            } break;
            case GLUT_KEY_END: {
                val = vmax-vsz;
            } break;
        }
    }
};
struct VP_VScrollbar : ViewPanel {
    float vmin = 0.0f;
    float vmax = 0.0f;
    float val = 0.0f;
    float vsz = 0.0f;
    
    bool snap_to_int = false; int snapping = 0;
    bool snap_smooth = true; int snap_moving = 0;
    
    vec3f c_line = {0.1,0.1,0.1};
    vec3f c_bar = {1.0,1.0,1.0};
    
    function<void(VP_VScrollbar*)> f_change = nullptr;
    
    VP_VScrollbar() : ViewPanel(pbg_DarkGray) { }
    VP_VScrollbar( string name, int vmin, int vmax, int val, int vsz ) : ViewPanel(name,pbg_DarkGray), vmin(vmin), vmax(vmax), val(val), vsz(vsz) { }
    
    virtual void _draw() {
        glMatrixMode(GL_PROJECTION);
        glPushMatrix();
        glLoadIdentity();
        gluOrtho2D(0,w,h,0);
        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
        glLoadIdentity();
        
        glPushAttrib(GL_ENABLE_BIT);
        glDisable(GL_LIGHTING); glDisable(GL_LINE_SMOOTH); glDisable(GL_DEPTH_TEST); glDisable(GL_CULL_FACE);
        glDisable(GL_BLEND);
        glLineWidth( 1.0f );
        
        float ym = (float)(h-6) / (vmax-vmin);
        float yv0 = ym * (val-vmin) + 3.0f;
        float yv1 = ym * (val+vsz-vmin) + 3.0f;
        float y0 = ym * (vmin-vmin) + 3.0f;
        float y1 = ym * (vmax-vmin) + 3.0f;
        yv0 = max(y0,min(y1,yv0));
        yv1 = max(y0,min(y1,yv1));
        float x0 = w/2;
        float xhl = max( 1.0f, x0 - 3.0f );
        float xhb = max( 1.0f, x0 - 2.0f );
        
        glLineWidth( 1.0f );
        glBegin(GL_LINES);
        glsColor(c_line);
        for( int i = 0; i < xhl; i++ ) {
            glVertex2f(x0-i,y0+i); glVertex2f(x0-i,y1-i);
            glVertex2f(x0+i,y0+i); glVertex2f(x0+i,y1-i);
        }
        glsColor(c_bar);
        for( int i = 0; i < xhb; i++ ) {
            glVertex2f(x0-i,yv0+i); glVertex2f(x0-i,yv1-i);
            glVertex2f(x0+i,yv0+i); glVertex2f(x0+i,yv1-i);
        }
        glEnd();
        
        glPopAttrib();
        
        glMatrixMode(GL_PROJECTION); glPopMatrix();
        glMatrixMode(GL_MODELVIEW); glPopMatrix();
        
        
        if( snap_to_int && snap_smooth && snap_moving ) {
            snap_moving = (snap_moving+1)%20;
            float n = ( snapping == 0 ? round(val) : ( snapping < 0 ? floor(val) : ceil(val) ) );
            n = max(vmin,min(vmax-vsz,n));
            if( snap_moving ) n = n*0.10f + val*0.90f;
            if( val != n ) { val = n; if(f_change) f_change(this); }
        }
    }
    
    void set_scroll( float val ) {
        val = max(vmin,min(vmax-vsz,val));
        if( f_change ) f_change(this);
    }
    void handle_scroll( float ydelta, bool scale=true ) {
        float ym = (scale ? ((float)(h-6) / (float)(vmax-vmin)) : 1.0f );
        val = max(vmin,min(vmax-vsz,val+(ydelta/ym)));
        if( f_change ) f_change(this);
    }
    
    virtual void do_mouse_out() { glutSetCursor( GLUT_CURSOR_INHERIT ); }
    virtual void do_mouse_in() { glutSetCursor( GLUT_CURSOR_UP_DOWN ); }
    virtual void _do_mouse_move() {
        if( mouse_but == MOUSE_LBUTTON || mouse_but == MOUSE_MBUTTON ) handle_scroll( mouse_pre.y-mouse_cur.y );
    }
    virtual void _do_mouse_down() { snap_moving = 0; }
    virtual void _do_mouse_up() {
        if( !snap_to_int ) return;
        if( snap_smooth ) {
            snap_moving = 1;
            return;
        }
        float n = ( snapping == 0 ? round(val) : ( snapping < 0 ? floor(val) : ceil(val) ) );
        n = max(vmin,min(vmax-vsz,n));
        if( val != n ) { val = n; if( f_change ) f_change(this); }
    }
    
    virtual void _do_keyspecial() {
        switch( keysp ) {
            case GLUT_KEY_HOME: {
                val = vmin;
            } break;
            case GLUT_KEY_END: {
                val = vmax-vsz;
            } break;
        }
    }
};



struct VP_Text : ViewPanel {
    VP_Split *p_main;
    ViewPanel *p_text;
    VP_VScrollbar *p_scroll;
    
    vector<string> l_text;
    
    VP_Text() {
        b_display = true;
        p_text = new ViewPanel();
        p_scroll = new VP_VScrollbar("text_scroll", 0.0f, 0.0f, 0.0f, 0.0f);
        p_main = new VP_Split("text_main", p_text, p_scroll, 11, SplitTypes::SPLIT_RIGHT);
    }

    void update_text() {
        float h_t = l_text.size() * c_h;
        float h_m = floor((float)h / (float)c_h);
        p_scroll->vmax = h_t;
        p_scroll->vsz = h_m;
    }
    
    virtual void _draw();
};





#endif







