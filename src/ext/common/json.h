#ifndef IO_H
#define IO_H

#include "debug.h"
#include "vec.h"
#include "array.h"
#include <string>
using std::string;
#include <vector>
using std::vector;
#include <map>
using std::map;

enum jsonType { jsonType_Null, jsonType_Bool, jsonType_Number, jsonType_String, jsonType_Array, jsonType_Object };

struct jsonValue;

typedef bool jsonBool;
typedef double jsonNumber;
typedef string jsonString;
typedef vector<jsonValue> jsonArray;
typedef map<string,jsonValue> jsonObject;

struct jsonValue {
    jsonType type;

    // this should be a union, but that does not work in c++0x
    jsonBool value_bool;
    jsonNumber value_number;
    jsonString value_string;
    jsonArray value_array;
    jsonObject value_object;

    jsonValue() { type = jsonType_Null; }
    jsonValue(const jsonBool& value) { type = jsonType_Bool; value_bool = value; }
    jsonValue(const jsonNumber& value) { type = jsonType_Number; value_number = value; }
    jsonValue(const jsonString& value) { type = jsonType_String; value_string = value; }
    jsonValue(const jsonArray& value) { type = jsonType_Array; value_array = value; }
    jsonValue(const jsonObject& value) { type = jsonType_Object; value_object = value; }
    jsonValue(const char* value) { type = jsonType_String; value_string = value; }
    jsonValue(const vec3f& value) { 
        type = jsonType_Array; 
        value_array.resize(3);
        value_array[0] = value.x;
        value_array[1] = value.y;
        value_array[2] = value.z; 
    }
    jsonValue(const float& value) { type = jsonType_Number; value_number = value; }
    jsonValue(const int& value) { type = jsonType_Number; value_number = value; }
    
    const jsonBool& as_bool() const { error_if_not(type==jsonType_Bool, "bool expected"); return value_bool; }
    const jsonNumber& as_number() const { error_if_not(type==jsonType_Number, "number expected"); return value_number; }
    const jsonObject& as_object() const { error_if_not(type==jsonType_Object, "object expected"); return value_object; }
    const jsonString& as_string() const { error_if_not(type==jsonType_String, "string expected"); return value_string; }
    const jsonArray& as_array() const { error_if_not(type==jsonType_Array, "array expected"); return value_array; }
    
    jsonArray& as_array_ref() { error_if_not(type==jsonType_Array, "object expected"); return value_array; }
    jsonObject& as_object_ref() { error_if_not(type==jsonType_Object, "object expected"); return value_object; }
    
    const jsonArray& as_array_ref() const { error_if_not(type==jsonType_Array, "object expected"); return value_array; }
    const jsonObject& as_object_ref() const { error_if_not(type==jsonType_Object, "object expected"); return value_object; }
    
    vec3f as_vec3f() const { 
        error_if_not(type==jsonType_Array, "array expected");
        error_if_not(value_array.size() == 3, "array fo length 3 expected");
        return makevec3f((float)value_array[0].as_number(),
                         (float)value_array[1].as_number(),
                         (float)value_array[2].as_number());
    }
    float as_float() const { return (float)as_number(); }
    int as_int() const { return (int)as_number(); }
    
    operator bool() const { return as_bool(); }
    operator int() const { return as_int(); }
    operator float() const { return as_float(); }
    operator double() const { return as_number(); }
    operator string() const { return as_string(); }
    operator vec3f() const { return as_vec3f(); }
    operator const jsonObject&() const { return as_object(); }
    
    void checkValue(const string& v) const { error_if_not(as_string() == v, "incorrect const value"); }
    void clear() { type = jsonType_Null; value_string = ""; value_object = jsonValue(); value_array.clear(); }
};

template<typename E>
inline jsonValue jsonFromEnum(const E& value, const sarray<sbimap_elem<string, E> >& bimap) { 
    return sbimap_find1(bimap, value); 
}

template<typename E>
inline E jsonToEnum(const jsonValue& json, const sarray<sbimap_elem<string, E> >& bimap) { 
    return sbimap_find2(bimap, json.as_string()); 
}

inline const jsonValue& jsonGet(const jsonObject& json, const string& name) {
    jsonObject::const_iterator it = json.find(name);
    error_if_not_va(it != json.end(), "variable %s expected", name.c_str());
    return it->second;
}

inline bool jsonHas(const jsonObject& json, const string& name) {
    return json.find(name) != json.end();
}

inline void jsonRemove(jsonObject& json, const string& name) {
    jsonObject::iterator it = json.find(name);
    if(it != json.end())
        json.erase(it);
}


jsonValue parseJson(const string& text);
string printJson(const jsonValue& value);

jsonValue loadJson(const string& filename);
void saveJson(const string& filename, const jsonValue& value);


#endif
