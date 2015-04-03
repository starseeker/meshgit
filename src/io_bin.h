#ifndef _IO_BIN_H_
#define _IO_BIN_H_

#include "std.h"
#include "vec.h"
#include "ext/common/frame.h"
#include "debug.h"

#include <set>

using namespace std;



bool  LoadBinaryFile_bool ( FILE *fp ) { char c;  fread( &c, 1, 1, fp ); return (c!=0); }
int   LoadBinaryFile_int  ( FILE *fp ) { int i;   fread( &i, 4, 1, fp ); return i; }
float LoadBinaryFile_float( FILE *fp ) { float f; fread( &f, 4, 1, fp ); return f; }
string LoadBinaryFile_string( FILE *fp ) {
    int n = LoadBinaryFile_int(fp); char str[n+1];
    fread( str, 1, n, fp ); str[n] = 0;
    return string(str);
}
vec2i LoadBinaryFile_vec2i( FILE *fp ) { vec2i v; fread( &v, 4, 2, fp ); return v; }
vec2f LoadBinaryFile_vec2f( FILE *fp ) { vec2f v; fread( &v, 4, 2, fp ); return v; }
vec3i LoadBinaryFile_vec3i( FILE *fp ) { vec3i v; fread( &v, 4, 3, fp ); return v; }
vec3f LoadBinaryFile_vec3f( FILE *fp ) { vec3f v; fread( &v, 4, 3, fp ); return v; }
vec4i LoadBinaryFile_vec4i( FILE *fp ) { vec4i v; fread( &v, 4, 4, fp ); return v; }
vec4f LoadBinaryFile_vec4f( FILE *fp ) { vec4f v; fread( &v, 4, 4, fp ); return v; }
frame3f LoadBinaryFile_frame3f( FILE *fp ) { frame3f f; fread( &f, 4, 12, fp ); return f; }

void LoadBinaryFile( FILE *fp, bool  &b ) { char c; fread( &c, 1, 1, fp ); b=(c!=0); }
void LoadBinaryFile( FILE *fp, int   &i ) { fread( &i, 4, 1, fp ); }
void LoadBinaryFile( FILE *fp, float &f ) { fread( &f, 4, 1, fp ); }
void LoadBinaryFile( FILE *fp, string &s ) {
    int n = LoadBinaryFile_int(fp); char str[n+1];
    fread( str, 1, n, fp ); str[n] = 0;
    s = str;
}
template<typename T> void LoadBinaryFile( FILE *fp, vec2<T> &v ) { fread( &v, sizeof(T), 2, fp ); }
template<typename T> void LoadBinaryFile( FILE *fp, vec3<T> &v ) { fread( &v, sizeof(T), 3, fp ); }
template<typename T> void LoadBinaryFile( FILE *fp, vec4<T> &v ) { fread( &v, sizeof(T), 4, fp ); }
template<typename T> void LoadBinaryFile( FILE *fp, frame3<T> &f ) { fread( &f, sizeof(T), 12, fp ); }


template<typename T> void LoadBinaryFile( FILE *fp, vector<T&> &l ) {
    int n = LoadBinaryFile_int(fp);
    if( n == 0 ) return;
    l.resize(n);
    for( int i = 0; i < n; i++ ) LoadBinaryFile( fp, l[i] );
}

template<typename T> void LoadBinaryFile( FILE *fp, vector<T> &l ) {
    int n = LoadBinaryFile_int(fp);
    if( n == 0 ) return;
    l.resize(n);
    for( int i = 0; i < n; i++ ) { T v; LoadBinaryFile( fp, v ); l[i] = v; }
}
template<typename T> void LoadBinaryFile( FILE *fp, unordered_set<T> &s ) {
    int n = LoadBinaryFile_int(fp);
    if( n == 0 ) return;
    s.reserve(n);
    for( int i = 0; i < n; i++ ) { T v; LoadBinaryFile( fp, v ); s.insert(v); }
}

template<typename T> void LoadBinaryFile( const string &fn, T &l ) {
    FILE *fp = fopen( fn.c_str(), "rb" );
    error_if_not_va( fp, "no file found: %s", fn.c_str() );
    LoadBinaryFile( fp, l );
    fclose(fp);
}




void SaveBinaryFile( FILE *fp, bool b ) { char c=(b?1:0); fwrite( &c, 1, 1, fp ); }
void SaveBinaryFile( FILE *fp, int i ) { fwrite( &i, 4, 1, fp ); }
void SaveBinaryFile( FILE *fp, float f ) { fwrite( &f, 4, 1, fp ); }
void SaveBinaryFile( FILE *fp, const string &s ) {
    int n = s.length(); SaveBinaryFile( fp, n ); fwrite( s.c_str(), 1, n, fp );
}
template<typename T> void SaveBinaryFile( FILE *fp, const vec2<T> &v ) { fwrite( &v, sizeof(T), 2, fp ); }
template<typename T> void SaveBinaryFile( FILE *fp, const vec3<T> &v ) { fwrite( &v, sizeof(T), 3, fp ); }
template<typename T> void SaveBinaryFile( FILE *fp, const vec4<T> &v ) { fwrite( &v, sizeof(T), 4, fp ); }
template<typename T> void SaveBinaryFile( FILE *fp, const frame3<T> &v ) { fwrite( &v, sizeof(T), 12, fp ); }

template<typename T> void SaveBinaryFile( FILE *fp, const vector<T> &l ) {
    SaveBinaryFile(fp,(int)l.size()); for( const T &v : l ) SaveBinaryFile(fp,v);
}
template<typename T> void SaveBinaryFile( FILE *fp, const unordered_set<T> &s ) {
    SaveBinaryFile(fp,(int)s.size()); for( const T &v : s ) SaveBinaryFile(fp,v);
}
template<typename T> void SaveBinaryFile( FILE *fp, const vector<T> &l, function<void(FILE*,T)> fp_save ) {
    SaveBinaryFile(fp,(int)l.size()); for( const T &v : l ) fp_save(fp,v);
}
template<typename T> void SaveBinaryFile( FILE *fp, const unordered_set<T> &s, function<void(FILE*,T)> fp_save ) {
    SaveBinaryFile(fp,(int)s.size()); for( const T &v : s ) fp_save(fp,v);
}






#endif
