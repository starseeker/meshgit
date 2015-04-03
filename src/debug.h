#ifndef DEBUG_H_
#define DEBUG_H_

#include "std_utils.h"

#include <cassert>
#include <stdio.h>
#include <cstdarg>
#include <string.h>
#include <stack>
#include <set>
#include <unordered_set>


using std::string;

inline void message(const char* msg) {
    fprintf(stdout, "%s\n", msg);
    fflush(stderr);
}

inline void message(const string& msg) { message(msg.c_str()); }
    
inline void message_va(const char* msg, ...) {
    va_list args; 
    va_start (args, msg);
    vfprintf(stdout, msg, args);
    va_end (args);
    fprintf(stdout, "\n");
    fflush(stdout);
}

inline void warning(const char* msg) {
    fprintf(stderr, "warning: %s\n", msg);
    fflush(stderr);
}

inline void warning(const string& msg) { warning(msg.c_str()); }
    
inline void warning_va(const char* msg, ...) {
    fprintf(stderr, "warning: ");
    va_list args;
    va_start (args, msg);
    vfprintf(stderr, msg, args);
    va_end (args);
    fprintf(stderr, "\n");
    fflush(stderr);
}

    
inline bool warning_if_not(bool check, const char* msg) {
    if(check) return check;
    fprintf(stderr, "warning: %s\n", msg);
    fflush(stderr);
    return check;
}

    
inline bool warning_if_not_va(bool check, const char* msg, ...) {
    if(check) return check;
    fprintf(stderr, "warning: ");
    va_list args;
    va_start (args, msg);
    vfprintf(stderr, msg, args);
    va_end (args);
    fprintf(stderr, "\n");
    fflush(stderr);
    return check;
}

    
inline void error(const char* msg) {
    fprintf(stderr, "error: %s\n", msg);
    fflush(stderr);
    assert(false);
}

inline void error(const string& msg) { error(msg.c_str()); }
    
inline void error_va(const char* msg, ...) {
    fprintf(stderr, "error: ");
    va_list args;
    va_start (args, msg);
    vfprintf(stderr, msg, args);
    va_end (args);
    fprintf(stderr, "\n");
    fflush(stderr);
    assert(false);
}

inline bool error_if_not(bool check, const char* msg) {
    if(check) return check;
    fprintf(stderr, "error: %s\n", msg);
    fflush(stderr);
    assert(check);
    return check;
}
    
inline bool error_if_not_va(bool check, const char* msg, ...) {
    if(check) return check;
    fprintf(stderr, "error: ");
    va_list args;
    va_start (args, msg);
    vfprintf(stderr, msg, args);
    va_end (args);
    fprintf(stderr, "\n");
    fflush(stderr);
    assert(check);
    return check;
}
    
inline void debug_break() {
    message("break: press any key to continue.");
    getchar();
}

inline void not_implemented_error() {
    error("not implemented");
}

inline void not_implemented_warning() {
    warning("not implemented");
}


extern timer dtimer;

struct DebugLevel {
    float time_start;
    char *text_end = nullptr;
    bool newline = true;
    
    DebugLevel( bool newline=true ) : time_start(dtimer.elapsed()), newline(newline) { }
};

struct DebugLog {
    const char *s_spinner = "|/-\\";
    int i_spinner = 0;
    
    const char *s_level = "  ";
    const int n_s_level = strlen(s_level);
    
    stack<DebugLevel*> levels;
    int depth = 0;
    
    const int sz_width = 120;
    const int sz_progress = 50;
    float progress_start_time = 0.0f;
    
    void _print_spaces() { for(int i = 0; i < depth; i++ ) printf( "%s", s_level ); }
    void _print_spaces( char *s ) {
        s[0] = 0; for(int i = 0; i < depth; i++ ) sprintf( &s[i*n_s_level], "%s", s_level );
    }
    void _print( const char *fmt, va_list args ) {
        _print_spaces();
        vprintf( fmt, args );
        printf("\n");
    }
    void print( const char *fmt, ... ) {
        if( !levels.empty() && !levels.top()->newline ) {
            printf( "\n" );
            levels.top()->newline = true;
        }
        
        va_list args; va_start(args, fmt);
        _print( fmt, args );
        va_end(args);
    }
    
    void print( const char *beg, vector<float> &l_vals, const char *fmt="%0.2f", const char *sep="," ) {
        if( !levels.empty() && !levels.top()->newline ) {
            printf( "\n" );
            levels.top()->newline = true;
        }
        _print_spaces();
        printf("%s[",beg);
        bool f = true;
        for( auto v : l_vals ) {
            if(!f) printf("%s",sep); else f=false;
            printf(fmt,v);
        }
        printf("]\n");
    }
    void print( const char *beg, vector<int> &l_vals, const char *fmt="%d", const char *sep="," ) {
        if( !levels.empty() && !levels.top()->newline ) {
            printf( "\n" );
            levels.top()->newline = true;
        }
        _print_spaces();
        printf("%s[",beg);
        bool f = true;
        for( auto v : l_vals ) {
            if(!f) printf("%s",sep); else f=false;
            printf(fmt,v);
        }
        printf("]\n");
    }
    void print( const char *beg, set<int> &s_ints, const char *fmt="%d", const char *sep="," ) {
        if( !levels.empty() && !levels.top()->newline ) {
            printf( "\n" );
            levels.top()->newline = true;
        }
        _print_spaces();
        printf("%s[",beg);
        bool f = true;
        for( int i : s_ints ) {
            if(!f) printf("%s",sep); else f=false;
            printf(fmt,i);
        }
        printf("]\n");
    }
    void print( const char *beg, unordered_set<int> &s_ints, const char *fmt="%d", const char *sep="," ) {
        if( !levels.empty() && !levels.top()->newline ) {
            printf( "\n" );
            levels.top()->newline = true;
        }
        _print_spaces();
        printf("%s{",beg);
        bool f = true;
        for( int i : s_ints ) {
            if(!f) printf("%s",sep); else f=false;
            printf(fmt,i);
        }
        printf("}\n");
    }
    
    
    
    void _print_progress( float p, bool clear=true ) {
        p = max(0.0f,min(1.0f,p));
        int n_s = (int)((float)sz_progress*p);
        int i_p = (int)( 100.0f * p );
        
        if( clear ) {
            printf( "\b\b\b\b\b\b\b\b" );
            for( int i = 0; i < sz_progress; i++ ) printf( "\b" );
            printf( "\b" );
        }
        
        printf( "[" );
        for( int i = 0; i < n_s; i++ ) printf("*");
        for( int i = n_s; i < sz_progress; i++ ) printf(" ");
        printf( "] %03d%% %c", i_p, s_spinner[i_spinner] );
        i_spinner = (i_spinner+1)%4;
    }
    void progress_start( const char *s_pre = nullptr ) {
        if( !levels.empty() && !levels.top()->newline ) {
            printf( "\n" );
            levels.top()->newline = true;
        }
        
        progress_start_time = dtimer.elapsed();
        
        if(false) {
            char s[256];
            _print_spaces(s); sprintf( &s[strlen(s)], "Progress: " );
            for( int i = strlen(s); i < sz_width; i++ ) sprintf( &s[i], "." );
            printf( "%s", s ); fflush(stdout);
        } else {
            _print_spaces();
        }
        
        if( s_pre ) printf( "%s", s_pre );
        _print_progress( 0.0f, false );
    }
    void progress_update( float p ) { _print_progress( p ); fflush(stdout); }
    void progress_end() {
        float time_delta = dtimer.elapsed() - progress_start_time;
        _print_progress( 1.0 ); printf( "\b%0.2fs\n", time_delta );
    }
    
    
    
    DebugLevel *start( const char *fmt, ... ) {
        if( !levels.empty() && !levels.top()->newline ) {
            printf( "\n" );
            levels.top()->newline = true;
        }
        
        va_list args; va_start(args, fmt);
        _print(fmt, args);
        va_end(args);
        
        DebugLevel *dlev = new DebugLevel();
        levels.push( dlev );
        depth++;
        return dlev;
    }
    DebugLevel *start_( const char *fmt, ... ) {
        if( !levels.empty() && !levels.top()->newline ) {
            printf( "\n" );
            levels.top()->newline = true;
        }
        
        char s[256];
        va_list args; va_start(args, fmt);
        _print_spaces(s);
        vsprintf( &s[strlen(s)], fmt, args );
        va_end(args);
        
        for( int i = strlen(s); i < sz_width; i++ ) sprintf( &s[i], "." );
        printf( "%s", s ); fflush(stdout);
        
        DebugLevel *dlev = new DebugLevel(false);
        levels.push( dlev );
        depth++;
        return dlev;
    }
    
    void fn( const char *fmt, void fn(void) ) {
        start(fmt); fn(); end();
    }
    void fn( const char *fmt, function<void(void)> fn) {
        start(fmt); fn(); end();
    }
    void fn_quiet( const char *fmt, function<void(void)> fn ) {
        start(fmt); fn(); end_quiet();
    }
    void fn_( const char *fmt, void fn(void) ) {
        start_(fmt); fn(); end();
    }
    void fn_( const char *fmt, function<void(void)> fn) {
        start_(fmt); fn(); end();
    }
    
    void end() {
        assert( !levels.empty() );
        DebugLevel *dlevel = levels.top();
        float time_delta = dtimer.elapsed() - dlevel->time_start;
        
        if( dlevel->newline ) {
            if( dlevel->text_end ) print( "%s %0.2fs", dlevel->text_end, time_delta );
            else print( "%0.2fs", time_delta );
        } else {
            printf( "%0.2fs\n", time_delta );
        }
        
        levels.pop(); depth--;
        delete dlevel;
    }
    
    void end_quiet() {
        assert( !levels.empty() );
        DebugLevel *dlevel = levels.top();
        levels.pop(); depth--;
        if( !dlevel->newline ) printf("\n");
        delete dlevel;
    }
    
};

extern DebugLog dlog;

#endif




