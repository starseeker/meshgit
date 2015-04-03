#include "viewer.h"
#include "debug.h"
#include "gls.h"
#include "draw_utils.h"
#include "std_utils.h"
#include "camera.h"
#include "light.h"
#include "globals.h"

#include "ext/lodepng/lodepng.h"

#include <time.h>




// GLUT Callbacks

void viewer_mouse_move( int mx, int my ) {
    if( viewer ) viewer->do_mouse_move(mx,viewer->h-my-1);
}
void viewer_mouse_pmove( int mx, int my ) {
    if( viewer == nullptr ) return;
    if( mx < 0 || my < 0 || mx >= viewer->w || my >= viewer->h ) return;
    viewer->do_mouse_move(mx,viewer->h-my-1);
}
void viewer_mouse_button( int mb, int state, int mx, int my ) {
    mb = ( mb == 0 ? 1 : ( mb == 1 ? 2 : 4 ) );
    if( viewer == nullptr ) return;
    switch( state ) {
        case GLUT_DOWN: viewer->do_mouse_down( mx, viewer->h-my-1, mb ); break;
        case GLUT_UP:   viewer->do_mouse_up( mx, viewer->h-my-1, mb );   break;
        default: break;
    }
}
void viewer_keyboard( unsigned char key, int mx, int my ) {
    if( viewer ) viewer->do_keyboard( key, mx, viewer->h-my-1 );
}
void viewer_keyspecial( int keysp, int mx, int my ) {
    if( viewer ) viewer->do_keyspecial( keysp, mx, viewer->h-my-1 );
}
void viewer_draw() {
    if( viewer ) viewer->draw();
}
void viewer_resize( int w, int h ) {
    if( viewer ) viewer->resize( w, h );
}
void viewer_idle() {
    if( viewer && viewer->fn_idle ) viewer->fn_idle(viewer->data);
    glutPostRedisplay();
}



Viewer::Viewer( string title, ViewPanel *main, ViewPanel *props, VP_ScrollTextView *help, int w, int h ) :
w(w), h(h), main(main), props(props), help(help)
{
    if( viewer ) error( "viewer already initialized!" );
    viewer = this;
    
    main_props = new VP_Split("Properties", main, props, 179, SplitTypes::SPLIT_RIGHT);
    
    camera = new VPCamera();
    camera->lookat( makevec3f(10,-20,10), makevec3f(0,0,0), makevec3f(0,0,1));
    
    char buf[1024];
    sprintf( buf, "rgba depth double samples<%d", 4 );
    //sprintf( buf, "rgba depth double");
    glutInitDisplayString(buf);
    
    int sw = glutGet( GLUT_SCREEN_WIDTH ), sh = glutGet( GLUT_SCREEN_HEIGHT );
    glutInitWindowPosition( (sw-w)/2, (sh-h)/2 );
    glutInitWindowSize(w, h);
    glutCreateWindow( title.c_str() );
    
    glutDisplayFunc(viewer_draw);
    glutMouseFunc(viewer_mouse_button);
    glutMotionFunc(viewer_mouse_move);
    glutPassiveMotionFunc(viewer_mouse_pmove);
    glutReshapeFunc(viewer_resize);
    glutIdleFunc(viewer_idle);
    glutKeyboardFunc(viewer_keyboard);
    glutSpecialFunc(viewer_keyspecial);
    
    glDisable(GL_LIGHT0);
    glDisable(GL_LIGHT1);
    glDisable(GL_LIGHT2);
    glDisable(GL_LIGHT3);
    glDisable(GL_LIGHT4);
    glDisable(GL_LIGHT5);
    glDisable(GL_LIGHT6);
    glDisable(GL_LIGHT7);
    
    resize( w, h );
}

Viewer::~Viewer() {
    viewer = nullptr;
}

ViewPanel *Viewer::get_under(int mx,int my) {
    if(fullscreen) return fullscreen->get_panel(mx,my);
    if(props_show) return main_props->get_panel(mx,my);
    if(help_show) return help;
    return main->get_panel(mx,my);
}

void Viewer::do_mouse_move( int mx, int my ) {
    if( main == nullptr ) return;
    
    if( active ) {
        active->do_mouse_move(mx,my);
    } else {
        ViewPanel *over = get_under(mx,my);
        
        if(over!=lastover) {
            if(lastover) lastover->do_mouse_out();
            lastover = over;
            over->do_mouse_in();
        }
        over->do_mouse_move( mx, my );
    }
}

void Viewer::do_mouse_down( int mx, int my, int mb ) {
    if( main == nullptr ) return;
    if( active ) return;
    
    active = get_under(mx,my);
    if( active != nullptr ) active->do_mouse_down(mx,my,mb);
}

void Viewer::do_mouse_up( int mx, int my, int mb ) {
    if( main == nullptr || active == nullptr ) return;
    active->do_mouse_up(mx,my,mb);
    active = nullptr;
}

#define CLAMP( x, m, M ) ( (x)<(m)?(m):( (x)>(M)?(M):(x) ) )
void Viewer::do_keyboard( unsigned char key, int mx, int my ) {
    if( key == 27 ) {
        if( help_show ) help_show = false;
        else exit(0);
    }
    if( help_show ) return;
    
    if( fn_keyboard ) fn_keyboard(key,data);
    
    if( main == nullptr ) return;
    if( active == nullptr ) {
        active = get_under(mx,my);
        if( active ) active->do_keyboard( key, mx, my );
        active = nullptr;
    } else {
        active->do_keyboard( key, mx, my );
    }
}

void Viewer::do_keyspecial( int keysp, int mx, int my ) {
    if( keysp == GLUT_KEY_F1 ) { help_show = !help_show; }
    
    if( main == nullptr ) return;
    
    switch( keysp ) {
        case GLUT_KEY_F2: {
            save_screenshot = true;
            screenshot_window = true;
            screenshot_alpha = glutGetModifiers() & GLUT_ACTIVE_SHIFT;
            glutSetCursor( GLUT_CURSOR_WAIT );
        } break;
        case GLUT_KEY_F3: {
            save_screenshot = true;
            screenshot_window = false;
            screenshot_alpha = glutGetModifiers() & GLUT_ACTIVE_SHIFT;
            glutSetCursor( GLUT_CURSOR_WAIT );
        } break;
        case GLUT_KEY_F11: {
            props_show = !props_show;
            resize(w,h);
            return;
        } break;
        case GLUT_KEY_F12: {
            if( fullscreen ) fullscreen = nullptr;
            else fullscreen = get_under(mx,my);
            resize(w,h);
            return;
        } break;
    }
    
    if( fn_keyspecial ) fn_keyspecial(keysp,data);
    
    if( active == nullptr ) {
        active = get_under(mx,my);
        if( active ) active->do_keyspecial( keysp, mx, my );
        active = nullptr;
    } else {
        active->do_keyspecial( keysp, mx, my );
    }
}

void Viewer::resize( int w, int h ) {
    active = nullptr;
    
    this->w = w; this->h = h;
    
    if(fullscreen) fullscreen->move(0,0,w,h);
    else if(props_show) main_props->move(0,0,w,h);
    else if(main) main->move( 0, 0, w, h );
    
    if( help ) help->move(0,0,w,h);
}

// http://wordgems.wordpress.com/2010/12/14/quick-dirty-pfm-reader/
void Viewer::savePFM3c( const char *filename, int w, int h, vector<unsigned char> &image3b, bool flip ) {
    string s_sz = to_string( "%d %d\n", w, h );
    int w3 = w * 3;
    float c[3];
    
    int y_start = ( flip ? 0 : h-1 );
    int y_end   = ( flip ? h : -1 );
    int y_step  = ( flip ? 1 : -1 );
    
    FILE *fp = fopen( filename, "wb" );
    
    // write header
    fwrite( "PF\n", sizeof(char), 3, fp );
    fwrite( s_sz.c_str(), sizeof(char), s_sz.length(), fp );
    fwrite( "-1.000000\n", sizeof(char), 10, fp );
    
    // write image data (converted: [0,255] -> [0.0f,1.0f] )
    for( int y = y_start; y != y_end; y += y_step ) {
        for( int x = 0; x < w; x++ ) {
            c[0] = (float)image3b[x*3+0+y*w3] / 255.0f;
            c[1] = (float)image3b[x*3+1+y*w3] / 255.0f;
            c[2] = (float)image3b[x*3+2+y*w3] / 255.0f;
            fwrite( c, sizeof(float), 3, fp );
        }
    }
    
    fclose(fp);
}

#define SWAP(x,y,t)  { t = x; x = y; y = t; }
void Viewer::draw() {
    glob.i_frame++;
    
    glViewport(0,0,w,h);
    draw_clear();
    
    if( help_show && help ) {
        help->draw();
    } else {
        if(fullscreen) fullscreen->draw();
        else if(props_show) main_props->draw();
        else if(main) main->draw();
    }
    
    glFlush();
    
    if( glob.play_save_screenshots && glob.auto_play ) {
        char filename[80];
        sprintf( filename, "ssplay_%06d.pfm", glob.play_save_index );
        glob.play_save_index++;
        
        dlog.start("Saving screenshot");
        
        dlog.start_("Reading pixels from OpenGL buffer");
        int w = viewer->w, h = viewer->h;
        vector<unsigned char> rgb(w*h*3);
        glReadBuffer(GL_BACK);
        glReadPixels(0,0,w,h, GL_RGB, GL_UNSIGNED_BYTE, &rgb[0]);
        dlog.end();
        
        dlog.start_("Saving");
        savePFM3c( filename, w, h, rgb, true );
        dlog.end();
        
        dlog.end();
        
        glutSetCursor( GLUT_CURSOR_INHERIT );
    } else if( save_screenshot ) {
        char filename[80];
        time_t rawtime;      time( &rawtime );
        struct tm *timeinfo; timeinfo = localtime( &rawtime );
        sprintf( filename, "ss_%04d%02d%02d_%02d%02d%02d.png",
                timeinfo->tm_year+1900, timeinfo->tm_mon+1, timeinfo->tm_mday,
                timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec
                );
        
        save_screenshot = false;
        
        dlog.start("Saving screenshot");
        
        dlog.start_("Reading pixels from OpenGL buffer");
        int x,y,w,h;
        if(screenshot_window) {
            x = 0;
            y = 0;
            w = viewer->w;
            h = viewer->h;
        } else {
            x = lastover->x;
            y = lastover->y;
            w = lastover->w;
            h = lastover->h;
        }
        
        int w4 = w*4;
        vector<unsigned char> rgba(w*h*4);
        vector<float> depth(w*h);
        vector<unsigned char> drgb(w*h*4);
        
        glReadBuffer(GL_BACK);
        glReadPixels(x,y,w,h, GL_RGBA, GL_UNSIGNED_BYTE, &rgba[0]);
        glPixelTransferf(GL_DEPTH_SCALE,1000.0f);
        glPixelTransferf(GL_DEPTH_BIAS, -999.0f);
        glReadPixels(x,y,w,h, GL_DEPTH_COMPONENT, GL_FLOAT, &depth[0]);
        dlog.end();
        
        dlog.start_("Flipping");
        #pragma omp parallel for default(none) shared(w,h,rgba,w4,depth,drgb)
        for( int x = 0; x < w; x++ ) {
            for( int y = 0; y < (int)ceil((float)h/2.0f); y++ ) {
                int y_ = h-1-y;
                unsigned char t;
                int i0 = x*4 + y * w4;
                int i1 = x*4 + y_ * w4;
                SWAP(rgba[i0+0],rgba[i1+0],t);
                SWAP(rgba[i0+1],rgba[i1+1],t);
                SWAP(rgba[i0+2],rgba[i1+2],t);
                if( screenshot_alpha ) {
                    rgba[i1+3] = depth[x+y*w] > 0.99f ? 0 : 255;
                    rgba[i0+3] = depth[x+(h-1-y)*w] > 0.99f ? 0 : 255;
                } else {
                    rgba[i0+3] = rgba[i1+3] = 255;
                }
                drgb[i0+0] = drgb[i0+1] = drgb[i0+2] = (unsigned char)(255.0f*depth[x+y_*w]);
                drgb[i1+0] = drgb[i1+1] = drgb[i1+2] = (unsigned char)(255.0f*depth[x+y*w]);
                drgb[i0+3] = drgb[i1+3] = 255;
            }
        }
        dlog.end();
        
        dlog.start_("Saving");
        unsigned error = lodepng::encode(filename, rgba, w, h);
        if(error) { warning_va("cannot write png image: %s", filename); }
        //lodepng::encode("depth.png",drgb,w,h);
        dlog.end();
        
        dlog.end();
        
        glutSetCursor( GLUT_CURSOR_INHERIT );
    }
    
    if( fn_draw_done ) fn_draw_done(data);
    
    glutSwapBuffers();
}










