#ifndef _JSON_H_
#define _JSON_H_

#include "std.h"
#include "debug.h"
#include "vec.h"
#include "string.h"

using namespace std;

struct ParsedJson {
    string _json;
    struct Value { int start, end; vector<int> children; };
    vector<Value> _values;
    
    ParsedJson(const string& _json) { _parse(_json); }
    ParsedJson(FILE* file) { _parse(file); }
    
    bool is_null(int v) { return _json[_values[v].start] == 'n'; }
    bool is_true(int v) { return _json[_values[v].start] == 't'; }
    bool is_false(int v) { return _json[_values[v].start] == 'f'; }
    bool is_bool(int v) { return is_true(v) or is_false(v); }
    bool is_number(int v) { return _json[_values[v].start] == '+' or _json[_values[v].start] == '-' or isdigit(_json[_values[v].start]); }
    bool is_string(int v) { return _json[_values[v].start] == '"'; }
    bool is_object(int v) { return _json[_values[v].start] == '{'; }
    bool is_array(int v) { return _json[_values[v].start] == '['; }
    
    void get_value(int v, bool& value) { error_if_not(is_bool(v), "bool expected"); value = is_true(v); }
    void get_value(int v, int& value) { error_if_not(is_number(v), "number expected"); error_if_not(sscanf(_json.c_str()+_values[v].start, "%d", &value) == 1, "int expected"); }
    void get_value(int v, float& value) { error_if_not(is_number(v), "number expected"); error_if_not(sscanf(_json.c_str()+_values[v].start, "%f", &value) == 1, "float expected"); }
    void get_value(int v, double& value) { error_if_not(is_number(v), "number expected"); error_if_not(sscanf(_json.c_str()+_values[v].start, "%lf", &value) == 1, "double expected"); }
    void get_value(int v, string& value) { error_if_not(is_string(v), "string expected"); value = _json.substr(_values[v].start+1,_values[v].end-_values[v].start-1); }
    void get_value(int v, vec2i& value) {
        get_value(get_array_member(v,0), value.x);
        get_value(get_array_member(v,1), value.y);
    }
    void get_value(int v, vec3f& value) {
        get_value(get_array_member(v,0), value.x);
        get_value(get_array_member(v,1), value.y);
        get_value(get_array_member(v,2), value.z);
    }
    void get_value(int v, vec3i& value) {
        get_value(get_array_member(v,0), value.x);
        get_value(get_array_member(v,1), value.y);
        get_value(get_array_member(v,2), value.z);
    }
    
    void get_value(int v, vector<bool>& array) {
        int c = get_array_size(v);
        array.resize(c);
		bool b;
        for( int i = 0; i < c; i++ ) {
            get_value( get_array_member( v, i ), b );
			array[i] = b;
		}
    }
    void get_value(int v, vector<int>& array) {
        int c = get_array_size(v);
        array.resize(c);
        for( int i = 0; i < c; i++ )
            get_value( get_array_member( v, i ), array[i] );
    }
    void get_value(int v, vector<float>& array) {
        int c = get_array_size(v);
        array.resize(c);
        for( int i = 0; i < c; i++ )
            get_value( get_array_member( v, i ), array[i] );
    }
    void get_value(int v, vector<vec2i>& array) {
        int c = get_array_size(v);
        array.resize(c);
        for( int i = 0; i < c; i++ )
            get_value( get_array_member( v, i ), array[i] );
    }
    void get_value(int v, vector<vec3f>& array) {
        int c = get_array_size(v);
        array.resize(c);
        for( int i = 0; i < c; i++ )
            get_value( get_array_member( v, i ), array[i] );
    }
    void get_value(int v, vector<vec3i>& array) {
        int c = get_array_size(v);
        array.resize(c);
        for( int i = 0; i < c; i++ )
            get_value( get_array_member( v, i ), array[i] );
    }
    
    int get_size(int v) { return _values[v].children.size(); }
    int get_child(int v, int i) { error_if_not(i >= 0 and i < get_size(v), "index out of range"); return _values[v].children[i]; }
    
    int get_array_size(int v) { error_if_not(is_array(v), "array expected"); return get_size(v); }
    int get_array_member(int v, int i) { error_if_not(is_array(v), "array expected"); return get_child(v, i); }
    
    bool has_object_member(int v, const char* name) { return _find_object_member(v, name) >= 0; }
    int get_object_member(int v, const char* name) {
        int ret = _find_object_member(v, name);
        error_if_not(ret >= 0, "object member not found");
        return ret;
    }
    
    int _find_object_member(int v, const char* name) {
        error_if_not_va(is_object(v), "object expected at %d when looking for %s", _values[v].start, name);
        for(int i = 0; i < get_size(v); i += 2) {
            auto c = get_child(v, i);
            auto s = _values[c].start+1;
            auto l = _values[c].end-_values[c].start-1;
            if(strlen(name) != l) continue;
            if(not strncmp(name,_json.c_str()+s,l)) return get_child(v,i+1);
        }
        return -1;
    }
    
    void _parse(FILE* file);
    void _parse(const string& _json);
    
    int _rec_parse(int v, int cur);
    void _parse_error(int pos, const char* msg) { error(msg); }
    void _parse_error(int pos, int len, const char* msg) {
        string s_err = _json.substr( pos, len );
        string s_ctx = _json.substr( pos-20, len+40 );
        string s_mkr = string(len, '^' );
        string s_spc = string(20,' ');
        
        error_va("%s at %d (+%d):\nctx: %s\n     %s%s", msg, pos, len, s_ctx.c_str(), s_spc.c_str(), s_mkr.c_str() );
    }
    int _add_value() { _values.push_back(Value()); return _values.size()-1; }
    void _add_child(int v, int i) { _values[v].children.push_back(i); }
    int _skipws(int cur);
    void _check(int cur, const char* msg, int n);
};

struct JsonNavigator {
    shared_ptr<ParsedJson> _json;
    int _i;
	
    JsonNavigator( const char *jsonfn ) { _init( jsonfn ); }
    JsonNavigator( string s_jsonfn ) { _init( s_jsonfn.c_str() ); }
	JsonNavigator( ParsedJson *json, int i_start=0 ) {
		_json = shared_ptr<ParsedJson>(json);
        _i = i_start;
	}
    JsonNavigator( shared_ptr<ParsedJson> json, int i_start=0 ) {
        _json = json;
        _i = i_start;
    }
    
    void _init( const char *jsonfn ) {
        FILE *fp = fopen( jsonfn, "rt" );
        if( fp == nullptr ) { error("no file"); }
        auto json = new ParsedJson(fp);
        fclose(fp);
        _json = shared_ptr<ParsedJson>(json);
        _i = 0;
    }
    
	JsonNavigator get_child( const int i_child ) {
        return JsonNavigator( _json, _json->get_child(_i,i_child));
	}
	JsonNavigator get_child( const char *s_child ) {
        return JsonNavigator(_json,_json->get_object_member(_i,s_child));
	}
	JsonNavigator get_child( const string &s_child) {
        return JsonNavigator(_json,_json->get_object_member(_i,s_child.c_str()));
	}
	
	JsonNavigator operator[](const string &s_child) { return get_child(s_child); }
	JsonNavigator operator[](const int i_child) { return get_child(i_child); }
	
	bool has_child( const char *s_child ) { return _json->has_object_member(_i, s_child); }
    bool has_child( int i_child ) { return (i_child >= 0 && i_child < get_children_count()); }
	int get_children_count() { return _json->get_array_size(_i); }
	
	void get_child_value( const char *s_child, bool& value ) {
		_json->get_value( _json->get_object_member( _i, s_child ), value );
	}
	void get_child_value( const char *s_child, int& value ) {
		_json->get_value( _json->get_object_member( _i, s_child ), value );
	}
	void get_child_value( const char *s_child, float& value ) {
		_json->get_value( _json->get_object_member( _i, s_child ), value );
	}
	void get_child_value( const char *s_child, string& value ) {
		_json->get_value( _json->get_object_member( _i, s_child ), value );
	}
    void get_child_value( const char *s_child, string &value, string &defval ) {
        if(has_child(s_child)) get_child_value(s_child,value);
        else value = defval;
    }
	void get_child_value( const char *s_child, vec2i &value ) {
		_json->get_value( _json->get_object_member( _i, s_child ), value );
	}
	void get_child_value( const char *s_child, vec3f &value ) {
		_json->get_value( _json->get_object_member( _i, s_child ), value );
	}
    void get_child_value( const char *s_child, vec3f &value, vec3f defval ) {
        if(has_child(s_child)) get_child_value(s_child,value);
        else value = defval;
    }
	void get_child_value( const char *s_child, vec3i &value ) {
		_json->get_value( _json->get_object_member( _i, s_child ), value );
	}
	void get_child_value( const char *s_child, vector<bool>& value ) {
		_json->get_value( _json->get_object_member( _i, s_child ), value );
	}
	void get_child_value( const char *s_child, vector<int>& value ) {
		_json->get_value( _json->get_object_member( _i, s_child ), value );
	}
	void get_child_value( const char *s_child, vector<float>& value ) {
		_json->get_value( _json->get_object_member( _i, s_child ), value );
	}
	void get_child_value( const char *s_child, vector<vec2i>& value ) {
		_json->get_value( _json->get_object_member( _i, s_child ), value );
	}
	void get_child_value( const char *s_child, vector<vec3f>& value ) {
		_json->get_value( _json->get_object_member( _i, s_child ), value );
	}
	void get_child_value( const char *s_child, vector<vec3i>& value ) {
		_json->get_value( _json->get_object_member( _i, s_child ), value );
	}
	
	void get_child_value( const int i_child, bool &value ) {
		_json->get_value(_json->get_array_member(_i,i_child),value);
	}
	void get_child_value( const int i_child, int &value ) {
		_json->get_value(_json->get_array_member(_i,i_child),value);
	}
	void get_child_value( const int i_child, float &value ) {
		_json->get_value(_json->get_array_member(_i,i_child),value);
	}
	void get_child_value( const int i_child, string &value ) {
		_json->get_value(_json->get_array_member(_i,i_child),value);
	}
	void get_child_value( const int i_child, vec2i &value ) {
		_json->get_value(_json->get_array_member(_i,i_child),value);
	}
	void get_child_value( const int i_child, vec3f &value ) {
		_json->get_value(_json->get_array_member(_i,i_child),value);
	}
	void get_child_value( const int i_child, vec3i &value ) {
		_json->get_value(_json->get_array_member(_i,i_child),value);
	}
	void get_child_value( const int i_child, vector<bool> &value ) {
		_json->get_value(_json->get_array_member(_i,i_child),value);
	}
	void get_child_value( const int i_child, vector<int> &value ) {
		_json->get_value(_json->get_array_member(_i,i_child),value);
	}
	void get_child_value( const int i_child, vector<float> &value ) {
		_json->get_value(_json->get_array_member(_i,i_child),value);
	}
	void get_child_value( const int i_child, vector<vec2i> &value ) {
		_json->get_value(_json->get_array_member(_i,i_child),value);
	}
	void get_child_value( const int i_child, vector<vec3f> &value ) {
		_json->get_value(_json->get_array_member(_i,i_child),value);
	}
	void get_child_value( const int i_child, vector<vec3i> &value ) {
		_json->get_value(_json->get_array_member(_i,i_child),value);
	}
	
	void get_value(bool& value) { _json->get_value(_i, value); }
	void get_value(int& value) { _json->get_value(_i, value); }
	void get_value(float& value) { _json->get_value(_i, value); }
	void get_value(string &value) { _json->get_value(_i, value); }
    void get_value(vec2i& value) { _json->get_value(_i, value); }
	void get_value(vec3f& value) { _json->get_value(_i, value); }
	void get_value(vec3i& value) { _json->get_value(_i, value); }
	void get_value(vector<bool>& value) { _json->get_value(_i, value); }
	void get_value(vector<int>& value) { _json->get_value(_i, value); }
	void get_value(vector<float>& value) { _json->get_value(_i, value); }
	void get_value(vector<vec2i>& value) { _json->get_value(_i, value); }
	void get_value(vector<vec3f>& value) { _json->get_value(_i, value); }
	void get_value(vector<vec3i>& value) { _json->get_value(_i, value); }
    
    bool get_bool() { bool v; _json->get_value(_i,v); return v; }
    int get_int() { int v; _json->get_value(_i,v); return v; }
    float get_float() { float v; _json->get_value(_i,v); return v; }
    string get_string() { string v; _json->get_value(_i,v); return v; }
    vec3f get_vec3f() { vec3f v; _json->get_value(_i,v); return v; }
    vec3i get_vec3i() { vec3i v; _json->get_value(_i,v); return v; }
    vec2i get_vec2i() { vec2i v; _json->get_value(_i,v); return v; }
    
    bool is_null() { return _json->is_null(_i); }
    
    //operator bool() { bool v; get_value(v); return v; }
    //operator int() { int v; get_value(v); return v; }
    //operator float() { float v; get_value(v); return v; }
    //operator vec3i() { vec3i v; get_value(v); return v; }
    //operator vec3f() { vec3f v; get_value(v); return v; }
};

JsonNavigator LoadJsonNavigator( string &s_jsonfn );
JsonNavigator LoadJsonNavigator( const char *s_jsonfn );


#endif
