#ifndef _PANEL_BACKGROUND_H_
#define _PANEL_BACKGROUND_H_

#include "ext/common/vec.h"
#include "gls.h"
#include "draw_utils.h"
#include "std.h"

using namespace std;

struct PanelBackground;


extern PanelBackground *pbg_Black;
extern PanelBackground *pbg_White;
extern PanelBackground *pbg_Gray;
extern PanelBackground *pbg_DarkGray;
extern PanelBackground *pbg_Maya;



struct PanelBackground {
    void draw();
    virtual void _draw() = 0;
};

struct PBG_Solid : PanelBackground {
    vec3f color;
    
    PBG_Solid() : color(zero3f) { }
    PBG_Solid( vec3f c ) : color(c) { }
    
    virtual void _draw();
};

struct PBG_VGrad : PanelBackground {
    vec3f color_top, color_bottom;
    
    PBG_VGrad( ) : color_top(zero3f), color_bottom(zero3f) { }
    PBG_VGrad( vec3f c_top, vec3f c_bot ) : color_top(c_top), color_bottom(c_bot) { }
    
    virtual void _draw();
};


#endif
