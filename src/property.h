#ifndef _PROPERTY_H_
#define _PROPERTY_H_

#include <string.h>
#include "std_utils.h"


typedef void (*fn_prop_callback)(void *data);
typedef void (*fn_prop_callback_int)(int v);
typedef void (*fn_prop_callback_float)(float pre,float cur);



struct Property {
    string name;
    bool changable = true;
    vec3f color = one3f;
    
    Property( string name ) : name(name) { }
    Property( string name, vec3f color ) : name(name), color(color) { }
    
    virtual string as_str( int w ) { return name; }
    
    virtual void do_mouse_down( int mouse_but ) { }
    virtual void do_mouse_up( int mouse_but ) { }
    virtual void do_mouse_move( vec2i mouse ) { }
    virtual void do_keyboard( unsigned char key ) { }
    virtual void do_keyspecial( int keysp ) { }
    virtual void do_mouse_in() { }
    virtual void do_mouse_out() { }
};

struct Prop_Divider : Property {
    char c = '-';
    Prop_Divider() : Property("") { changable=false; }
    Prop_Divider( char c ) : Property(""), c(c) { changable=false; }
    virtual string as_str( int w ) { return string( w, c ); }
};

enum TextAlignment { TEXT_LEFT, TEXT_CENTER, TEXT_RIGHT };
struct Prop_Text : Property {
    TextAlignment align;
    
    Prop_Text( string text, TextAlignment align ) :
    Property(text), align(align) { changable=false; }
    
    virtual string as_str( int w ) {
        w = (w>name.length()) ? w : name.length();
        char cs[w+1];
        for( int i = 0; i < w; i++ ) cs[i] = ' ';
        int i = 0;
        if( align == TEXT_CENTER ) i = (w - name.length() ) / 2;
        else if( align == TEXT_RIGHT ) i = w - name.length();
        sprintf( &cs[i], "%s", name.c_str() );
        return string(cs);
    }
};


struct Prop_Bind_Int : Property {
    int *v; int m, M; bool changable = true;
    int v_prev = 0;
    void *data;
    function<void(void*)> fn_change = nullptr;
    function<void(void*)> fn_change_done = nullptr;
    
    Prop_Bind_Int( string name, int *v, int m, int M ) :
    Property(name), v(v), m(m), M(M) {}
    Prop_Bind_Int( string name, int *v, int m, int M, bool changable ) :
    Property(name), v(v), m(m), M(M), changable(changable) {}
    
    virtual string as_str( int w ) {
        if( w == 0 ) return "";
        char cf[80], cs[80], cv[80];
        sprintf( cv, "%5d", *v );
        int l = strlen(cv);
        sprintf( cf, "%%-%ds%%s", w-l );
        sprintf( cs, cf, name.c_str(), cv );
        return string(cs);
    }
    
    void set( int v ) {
        if( !changable ) return;
        *(this->v) = min( M, max( m, v ) );
        if( fn_change ) fn_change(data);
    }
    void change_done() {
        if( v_prev != *v ) {
            if( fn_change_done ) fn_change_done(data);
            v_prev = *v;
        }
    }
    
    virtual void do_mouse_move( vec2i mouse ) { set( *v+mouse.x ); }
    virtual void do_mouse_up( int but ) { change_done(); }
    
    virtual void do_keyboard( unsigned char key ) {
        if( key == '+' ) set(*v+1);
        if( key == '-' ) set(*v-1);
        change_done();
    }
    
    virtual void do_keyspecial( int keysp ) {
        if( keysp == GLUT_KEY_HOME ) set( m );
        if( keysp == GLUT_KEY_END ) set( M );
        if( keysp == GLUT_KEY_PAGE_DOWN ) set( *v-50 );
        if( keysp == GLUT_KEY_PAGE_UP ) set( *v+50 );
        change_done();
    }
};

struct Prop_Bind_Float : Property {
    float *v; float m, M; float scale;
    float v_prev;
    void *data;
    const char *fmt = "%0.2f";
    
    fn_prop_callback fn_change = nullptr;
    fn_prop_callback fn_change_done = nullptr;
    fn_prop_callback_float fn_change_precur = nullptr;
    
    function<void(void*)> lfn_change = nullptr;
    
    Prop_Bind_Float( string name, float *v, float m, float M ) :
    Property(name), v(v), m(m), M(M), scale((M-m)/100.0f), v_prev(*v) {}
    
    virtual string as_str( int w ) {
        if( w == 0 ) return "";
        char cf[80], cs[80], cv[80];
        sprintf( cv, fmt, *v );
        int l = strlen(cv);
        sprintf( cf, "%%-%ds%%s", w-l );
        sprintf( cs, cf, name.c_str(), cv );
        return string(cs);
    }
    
    void set( float v ) {
        float n = min(M,max(m,v));
        if( fn_change_precur ) fn_change_precur(*(this->v),n);
        *(this->v) = n;
        if( fn_change ) fn_change(data);
        if( lfn_change ) lfn_change(data);
    }
    
    virtual void do_mouse_move( vec2i mouse ) { set( *v+(mouse.x*scale) ); }
    virtual void do_mouse_up( int but ) {
        if( v_prev != *v ) {
            if( fn_change_done ) fn_change_done(data);
            v_prev = *v;
        }
    }
    
    virtual void do_keyboard( unsigned char key ) {
        if( key == '+' ) set(*v+scale);
        if( key == '-' ) set(*v-scale);
    }
    
    virtual void do_keyspecial( int keysp ) {
        if( keysp == GLUT_KEY_HOME ) set( m );
        if( keysp == GLUT_KEY_END ) set( M );
        if( keysp == GLUT_KEY_PAGE_DOWN ) set( *v-scale*10 );
        if( keysp == GLUT_KEY_PAGE_UP ) set( *v+scale*10 );
    }
};

struct Prop_Bind_Bool : Property {
    bool *v;
    bool v_prev;
    void *data;
    
    function<void(void*)> lfn_change = nullptr;
    function<void(void*)> fn_change_done = nullptr;
    
    Prop_Bind_Bool( string name, bool *v ) :
    Property(name), v(v) { }
    
    Prop_Bind_Bool( string name, bool *v, vec3f color ) :
    Property(name,color), v(v) { }
    
    virtual string as_str( int w ) {
        const char *cv = *v ? "[X]" : "[ ]";
        char cf[80], cs[80];
        sprintf( cf, "%%-%ds%%3s", w-3 );
        sprintf( cs, cf, name.c_str(), cv );
        return string(cs);
    }
    virtual void do_mouse_down( int ) {
        v_prev = *v;
        *v = !*v;
        if( lfn_change ) lfn_change(data);
    }
    virtual void do_mouse_up( int but ) {
        if( v_prev != *v ) {
            if( fn_change_done ) fn_change_done(data);
            v_prev = *v;
        }
    }
};

struct Prop_Bind_String : Property {
    timer uptime;
    //static timer *uptime = new timer();
    string *s; int cursor=0;
    void *data;
    fn_prop_callback fn_change = nullptr;
    bool mouse_in = false;
    string valid = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789!@#$%^&*(){}[]/?=+\\|',.\"-_`~";
    
    Prop_Bind_String( string name, string *s ) : Property(name), s(s) { cursor = s->length(); }
    
    virtual void do_mouse_in() { mouse_in = true; }
    virtual void do_mouse_out() { mouse_in = false; }
    
    virtual string as_str( int w ) {
        const string &sv = *s;
        bool blip = (uptime.elapsed_saw(4.0f) > 0.5f) & mouse_in;
        cursor = max(0,min((int)sv.length(),cursor));
        int max_w = sv.length()+1;
        char cf[80], cs[80], cv[80];
        sprintf( cv, "%s ", sv.c_str() );
        if( blip ) cv[cursor] = '_'; //'_';
        sprintf( cf, "%%s%%-%ds", max_w );
        sprintf( cs, cf, name.c_str(), cv );
        return string(cs);
    }
    
    bool is_valid(unsigned char key) {
        const char *v = valid.c_str(); int n = valid.size();
        for( int _ = 0; _ < n; _++ ) if( v[_]==key ) return true;
        return false;
    }
    
    virtual void do_keyboard( unsigned char key ) {
        if( is_valid(key) ) {
            s->insert( s->begin()+cursor, key);
            cursor++;
        } else {
            switch(key) {
                case 8: {
                    if( cursor > 0 ) {
                        s->erase( s->begin() + cursor - 1 );
                        cursor--;
                    }
                } break;
                case 127: {
                    if( cursor < s->length() ) s->erase( s->begin() + cursor );
                } break;
                default: {
                    //printf("%d ",(int)key); fflush(stdout);
                } break;
            }
        }
    }
    
    virtual void do_keyspecial( int keysp ) {
        if( keysp == GLUT_KEY_HOME ) cursor=0;
        if( keysp == GLUT_KEY_END ) cursor=s->length();
        if( keysp == GLUT_KEY_LEFT ) cursor = max(0,cursor-1);
        if( keysp == GLUT_KEY_RIGHT ) cursor = min((int)s->length(),cursor+1);
    }
};



struct Prop_Bind_Enum : Property {
    vector<string> values;
    int *v;
    void *data;
    function<void(void*)> fn_change = nullptr;
    
    Prop_Bind_Enum( string name, const vector<string> &values, int *v ) :
    Property(name), values(values), v(v) { }
    virtual string as_str( int w ) {
        const string &sv = values[*v];
        int max_w = sv.length();
        const char *cv = sv.c_str();
        char cf[80], cs[80];
        sprintf( cf, "%%-%ds%%%ds", w-max_w, max_w );
        sprintf( cs, cf, name.c_str(), cv );
        return string(cs);
    }
    virtual void do_mouse_down( int mouse_but ) {
        if( mouse_but == MOUSE_LBUTTON ) *v = (*v+1) % values.size();
        else if( mouse_but == MOUSE_RBUTTON ) *v = (*v+values.size()-1) % values.size();
        if( fn_change ) fn_change(data);
    }
};

struct Prop_Button : Property {
    void *data;
    //fn_prop_callback fn_push = nullptr;
    function<void(void*)> lfn_push = nullptr;
    bool fullwidth;
    
    //Prop_Button( string name, void *data, fn_prop_callback fn_push, bool fullwidth=false ) :
    //Property(name), data(data), fn_push(fn_push), fullwidth(fullwidth) { }
    
    Prop_Button( string name, void *data, function<void(void*)> lfn_push, bool fullwidth=false ) :
    Property(name), data(data), lfn_push(lfn_push), fullwidth(fullwidth) { }
    
    Prop_Button( string name, function<void()> lfnpush, bool fullwidth=false ) :
    Property(name), data(nullptr), fullwidth(fullwidth) {
        this->lfn_push = [&](void*) { lfnpush(); };
    }
    
    virtual string as_str( int w ) {
        char cs[80];
        if( fullwidth ) {
            char cf[80];
            int l = name.length();
            int l1 = (w-2-l)/2+l;
            int l2 = w-2-l1+1;
            sprintf( cf, "%%c%%%ds%%%dc", l1, l2 );
            sprintf( cs, cf, '[', name.c_str(), ']' );
        } else {
            int i;
            const char *ca_name = name.c_str();
            for( i = 0; ca_name[i]==' '; i++ ) cs[i] = ' ';
            sprintf( &cs[i], "[ %s ]", &ca_name[i]);
        }
        return string(cs);
    }
    virtual void do_mouse_down( int mouse_but ) {
        if( mouse_but != MOUSE_LBUTTON ) return;
        //if( fn_push ) fn_push(data);
        if( lfn_push ) lfn_push(data);
    }
};


#endif

