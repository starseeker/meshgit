#include "json.h"
#include "cjson/cJSON.h"
#include "debug.h"

inline jsonValue _cjson_to_std(cJSON* elem) {
    switch(elem->type) {
        case cJSON_NULL: return jsonValue();
        case cJSON_False: return jsonValue(false);
        case cJSON_True: return jsonValue(true);
        case cJSON_Number: return jsonValue(elem->valuedouble);
        case cJSON_String: return jsonValue(string(elem->valuestring));
        case cJSON_Array: {
            jsonArray v;
            for(int i = 0; i < cJSON_GetArraySize(elem); i ++) {
                v.push_back(_cjson_to_std(cJSON_GetArrayItem(elem,i)));
            }
            return jsonValue(v); }
        case cJSON_Object: {
            jsonObject o;
            for(int i = 0; i < cJSON_GetArraySize(elem); i ++) {
                o[string(cJSON_GetArrayItem(elem,i)->string)] = _cjson_to_std(cJSON_GetArrayItem(elem,i));
            }
            return jsonValue(o); }
        default: error("unkown cjson type"); return jsonValue();
    }
    return jsonValue();
}

inline cJSON* _std_to_cjson(const jsonValue& json) {
    if(json.type == jsonType_Null) return cJSON_CreateNull();
    else if(json.type == jsonType_Bool) return (json.value_bool)?cJSON_CreateTrue():cJSON_CreateFalse();
    else if(json.type == jsonType_Number) return cJSON_CreateNumber(json.value_number);
    else if(json.type == jsonType_String) return cJSON_CreateString(json.value_string.c_str());
    else if(json.type == jsonType_Array) {
        cJSON* array = cJSON_CreateArray();
        for(size_t i = 0; i < json.value_array.size(); i ++) {
            cJSON_AddItemToArray(array, _std_to_cjson(json.value_array[i]));
        }
        return array;
    } else if(json.type == jsonType_Object) {
        cJSON* object = cJSON_CreateObject();
        for(jsonObject::const_iterator it = json.value_object.begin(); it != json.value_object.end(); ++it) {
            cJSON_AddItemToObject(object, it->first.c_str(), _std_to_cjson(it->second));
        }
        return object;
    } else {
        error("unknown type in json conversion");
        return 0;
    }
}

jsonValue parseJson(const string& text) {
    // read using cjson library
    cJSON *json = cJSON_Parse(text.c_str());
	if(!json) { error_if_not_va(false, "error in parsing json %s", text.c_str()); return jsonValue(); }
    // convert to our types
    jsonValue ret = _cjson_to_std(json);
    // clean
    cJSON_Delete(json);
    // done
    return ret;
}

string printJson(const jsonValue& value) {
    // convert to cjson
    cJSON* json = _std_to_cjson(value);
    // print using cjson
    char* text = cJSON_Print(json);
    // convert to string
    string ret = text;
    // cleanup
    cJSON_Delete(json);
    delete text;
    // done
    return ret;
}

string _loadTextFile(const string& filename) {
    // open
    FILE* file = fopen(filename.c_str(), "rt");
    error_if_not_va(file != 0, "cannot open file %s", filename.c_str());
	if (file == 0)
		return string();

    // compute length
    fseek(file, 0, SEEK_END);
    long int length = ftell(file);
    error_if_not(length >= 0, "error reading text file");
    fseek(file, 0, SEEK_SET);
    
    // allocate buffer and read
    char *buffer = new char[length];
    warning_if_not(fread(buffer, 1, length, file) == (size_t)length, "file lenght does not match");
    
    // convert to string
    string ret = buffer;
    
    // cleanup
    delete [] buffer;
    
    // done
    fclose(file);
    return ret;
}

void _writeTextFile(const string& filename, const string& text) {
    // open
    FILE* file = fopen(filename.c_str(), "wt");
    error_if_not_va(file != 0, "cannot open file %s", filename.c_str());
    
    // write
    error_if_not(fputs(text.c_str(), file) >= 0, "error writing file");
    
    // done
    fclose(file);
}

jsonValue loadJson(const string& filename) {
    return parseJson(_loadTextFile(filename));
}

void saveJson(const string& filename, const jsonValue& json) {
    string text = printJson(json);
    _writeTextFile(filename, text);
}

