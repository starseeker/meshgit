#ifndef _VIEWER_H_
#define _VIEWER_H_

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

#include "panel.h"
#include "panelbackground.h"
#include "property.h"
#include "gls.h"
#include "std.h"


struct Viewer {
    ViewPanel *main = nullptr; VP_Split *main_props = nullptr;
    ViewPanel *active = nullptr;
    VP_ScrollTextView *help = nullptr;  bool help_show = false;
    ViewPanel *props = nullptr;         bool props_show = false;
    ViewPanel *lastover = nullptr;
    ViewPanel *fullscreen = nullptr;
    VPCamera  *camera = nullptr;
    
    void *data;
    
    function<void(void *data)> fn_idle = nullptr;
    function<void(unsigned char key, void *data)> fn_keyboard = nullptr;
    function<void(int keysp, void *data)> fn_keyspecial = nullptr;
    function<void(void *data)> fn_draw_done = nullptr;
    
    int w,h;
    
    bool save_screenshot = false; bool screenshot_window = true;
    bool screenshot_alpha = false;
    
    Viewer( string title, ViewPanel *main, ViewPanel *props, VP_ScrollTextView *help, int w=1920, int h=1080 );
    ~Viewer();
    
    void run() { glutMainLoop(); }
    void draw();
    void resize( int w, int h );
    void do_mouse_move( int mx, int my );
    void do_mouse_down( int mx, int my, int mb );
    void do_mouse_up( int mx, int my, int mb );
    void do_keyboard( unsigned char key, int mx, int my );
    void do_keyspecial( int keysp, int mx, int my );
    
    ViewPanel *get_under(int mx,int my);
    
    void savePFM3c( const char *filename, int w, int h, vector<unsigned char> &image3b, bool flip );
};




#endif
