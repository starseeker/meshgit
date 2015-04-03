#ifndef _GLOBALS_H_
#define _GLOBALS_H_

#include "std_utils.h"
#include "camera.h"
#include "panel.h"

namespace CameraType {
    enum Enum { Custom, Data };
    const vector<string> Names = { "Custom", "Data" };
}

namespace BackgroundType {
    enum Enum { Gradient, Black, White };
    const vector<string> Names = { "Gradient", "Black", "White" };
}


struct global_data {
    string filename;
    
    timer uptimer;
    unsigned long i_frame = 0;
    
    string *_status = nullptr;
    void set_status( const char *fmt, ... ) {
        if( _status == nullptr ) return;
        char b[256];
        va_list args; va_start(args, fmt); vsprintf( b, fmt, args ); va_end(args);
        _status->assign(b);
    }
    void set_status( string s ) {
        if( _status == nullptr ) return;
        _status->assign(s);
    }
    
    int   auto_play = 0;
    bool  play_stopped = true;
    bool  play_stopat_move = false;
    bool  play_save_screenshots = false;
    int   play_save_index = 0;
    
    int   view3d_turntable_rotz = 0;
    int   view3d_background = BackgroundType::Gradient;
    bool  view3d_toy = false;
    
    bool  view3d_faces = true;
    bool  view3d_faces_onlyadded = false;
    bool  view3d_faces_onlysame = false;
    bool  view3d_edges = false;
    bool  view3d_edges_add = false;
    bool  view3d_edges_del = false;
    bool  view3d_verts = false;
    bool  view3d_previous = false;
    
    bool  view3d_cameras = false;
    bool  view3d_changes_add = false;
    bool  view3d_changes_del = false;
    bool  view3d_highlight_next = false;
    bool  view3d_highlight_prev = false;
    float view3d_morph_fps = 1.0f;
    bool  view3d_axis = false;
    bool  view3d_grid = false;
    int   view3d_camtype = CameraType::Custom;
    VPCamera *view3d_camera;
};

extern global_data glob;


#endif

