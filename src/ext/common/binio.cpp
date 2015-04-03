#include "binio.h"
#include "debug.h"
#include <stdio.h>

//enum jsonType { jsonType_Null, jsonType_Bool, jsonType_Number, jsonType_String, jsonType_Array, jsonType_Object };


template<typename T>
inline T parseNumber(FILE *f) {
    T value;
    fread(&value, sizeof(T), 1, f);
    return value;
}

inline string parseString(FILE *f) {
    int size = parseNumber<int32_t>(f);
    char str[256];
    fread(str, sizeof(char), size, f);
    str[size] = 0;
    return str;
}

jsonArray parseArray(FILE *f);
jsonObject parseObject(FILE *f);
jsonValue parseValue(FILE *f) {
    int32_t type = parseNumber<int32_t>(f);
    switch (type) {
        case jsonType_Null: {
            message("null type");
        } break;
        case jsonType_Bool: {
            jsonBool value = parseNumber<int>(f);
            return value;
        } break;
        case jsonType_Number: {
            jsonNumber value = parseNumber<double>(f);
            return value;
        } break;
        case jsonType_String: {
            jsonString value = parseString(f);
            return value;
        } break;
        case jsonType_Array: {
            jsonArray value = parseArray(f);
            return value;
        } break;
        case jsonType_Object: {
            jsonObject value = parseObject(f);
            return value;
        } break;
        default: break;
    }
    return jsonValue();
}

jsonArray parseArray(FILE *f) {
    jsonArray json;
    int32_t size = parseNumber<int32_t>(f);
    json.reserve(size);
    for (int i = 0; i < size; i++) {
        json.push_back(parseValue(f));
    }
    return json;
}

jsonObject parseObject(FILE *f) {
    jsonObject json;
    int32_t size = parseNumber<int32_t>(f);
    for (int i = 0; i < size; i++) {
        string name = parseString(f);
        json[name] = parseValue(f);
    }
    return json;
}

jsonValue loadBin(const string& filename) {
    FILE* f = fopen(filename.c_str(), "rb");
    if (f == 0) {
        error_va("failed to open file: %s", filename.c_str());
        return jsonValue();
    }
    jsonValue json = parseObject(f);
    fclose(f);
    return json;
}

template<typename T>
inline void writeNumber(FILE *f, T value) {
    fwrite(&value, sizeof(T), 1, f);
}

inline void writeString(FILE *f, const string &str) {
    int32_t size = (int32_t)str.size();
    writeNumber<int32_t>(f, size);
    fwrite(str.c_str(), sizeof(char), size, f);
}

void writeArray(FILE *f, const jsonArray& array);
void writeObject(FILE *f, const jsonObject& object);
void writeValue(FILE *f, const jsonValue &value) {
    int32_t type = value.type;
    writeNumber<int32_t>(f, type);
    switch (type) {
        case jsonType_Null: {
            message("null type");
        } break;
        case jsonType_Bool: {
            writeNumber<int>(f, value.as_bool());
        } break;
        case jsonType_Number: {
            writeNumber<double>(f, value.as_number());
        } break;
        case jsonType_String: {
            writeString(f, value.as_string());
        } break;
        case jsonType_Array: {
            writeArray(f, value.as_array());
        } break;
        case jsonType_Object: {
            writeObject(f, value.as_object());
        } break;
        default: break;
    }
}

void writeArray(FILE *f, const jsonArray& json) {
    int32_t size = json.size();
    writeNumber<int32_t>(f, size);
    for (int i = 0; i < size; i++) {
        writeValue(f, json[i]);
    }
}

void writeObject(FILE *f, const jsonObject& json) {
    int32_t size = json.size();
    writeNumber<int32_t>(f, size);
    for (jsonObject::const_iterator it = json.begin();
         it != json.end(); it++) {
        writeString(f, (it->first));
        writeValue(f, (it->second));
    }
}


void saveBin(const string& filename, const jsonValue& json) {
    FILE* f = fopen(filename.c_str(), "wb");
    if (f == 0) {
        error_va("failed to open file: %s", filename.c_str());
    } else {
        writeObject(f, json);
        fclose(f);
    }
}
