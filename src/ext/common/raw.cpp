#include "raw.h"

#include "debug.h"
#include <cstring>

// format
// first character: structure type
// A -> ascii, B -> binary
// second character: basic type (as python array module)
// b/B -> byte (signed unsigned) 1 byte
// i/I -> int (signed unsigned) 4 bytes
// f -> float
// data: nc size1 size2 type+
// size2 is 1 for arrays
// binary data is encoded in native format and read directly
// ascii data are numbers separated by spaces

// low level file handling

void loadRaw(const string& filename, rawtype& type,
             int& size1, int& size2, int& nc,
             rawbuffer& buffer) {
    // open file
    FILE* file = fopen(filename.c_str(), "rb");
    if (file == 0) {
        error_if_not_va(file != 0, "cannot open file %s", filename.c_str());
        nc = size1 = size2 = 0;
        buffer = 0;
        return;
    }
    
    
    // load magic
    char magic; 
    bool ascii = false;
    error_if_not(fread(&magic, sizeof(char), 1, file) == 1, "problem reading file");
    if(magic == 'A') ascii = true;
    else if(magic == 'B') ascii = false;
    else error("incorrect magic");
    
    // load type
    error_if_not(fread(&type, sizeof(char), 1, file) == 1, "problem reading file");
    error_if_not(type == 'b' || type == 'B' || type == 'i' || type == 'I' || type == 'f', "unknown raw type");
        
    // determining data size
    int ds = (type == 'b' || type == 'B')?sizeof(char):sizeof(int);
    
    // read sizes
    if(ascii) {
        // read shape
        error_if_not(fscanf(file, "%d %d %d", &size1, &size2, &nc) == 3, "problem reading file");
    } else {
        // compute type size
        error_if_not(fread(&size1, sizeof(int), 1, file) == 1, "problem reading file");
        error_if_not(fread(&size2, sizeof(int), 1, file) == 1, "problem reading file");
        error_if_not(fread(&nc, sizeof(int), 1, file) == 1, "problem reading file");
    }

    // allocate
    buffer = new unsigned char[ds*nc*size1*size2];
    
    // read data buffer
    if(ascii) {
        if(type == 'b') {
            for(int i = 0; i < nc*size1*size2; i ++) {
                int v;
                error_if_not(fscanf(file, "%d", &v) == 1, "problem reading file");
                ((char*)buffer)[i] = v; // TODO: check size
            }
        } else if(type == 'B') {
            for(int i = 0; i < nc*size1*size2; i ++) {
                unsigned int v;
                error_if_not(fscanf(file, "%ud", &v) == 1, "problem reading file");
                ((unsigned char*)buffer)[i] = v; // TODO: check size
            }
        } else if(type == 'i') {
            for(int i = 0; i < nc*size1*size2; i ++) {
                int v;
                error_if_not(fscanf(file, "%d", &v) == 1, "problem reading file");
                ((int*)buffer)[i] = v;
            }
        } else if(type == 'I') {
            for(int i = 0; i < nc*size1*size2; i ++) {
                unsigned int v;
                error_if_not(fscanf(file, "%ud", &v) == 1, "problem reading file");
                ((unsigned int*)buffer)[i] = v;
            }
        } else if(type == 'f') {
            for(int i = 0; i < nc*size1*size2; i ++) {
                float v;
                error_if_not(fscanf(file, "%f", &v) == 1, "problem reading file");
                ((float*)buffer)[i] = v;
            }
        } else error("Not supported ASCII format.");
    } else {
        error_if_not((int)fread(buffer, ds, nc*size1*size2, file) == nc*size1*size2, "problem reading file");
    }
    
	// done
	fclose(file);
    file = 0;
}

void saveRaw(const string& filename, rawtype type,
             int size1, int size2, int nc,
             bool ascii, rawbuffer buffer) {
    // open file
    FILE* file = fopen(filename.c_str(), "wb");
    error_if_not_va(file != 0, "problem opening file %s", filename.c_str());
    
    // write magic
    char magic = (ascii)?'A':'B';
    error_if_not_va(fwrite(&magic, sizeof(char), 1, file) == 1, "problem writing file %s", filename.c_str());
    
    // write type
    error_if_not_va(fwrite(&type, sizeof(char), 1, file) == 1, "problem writing file %s", filename.c_str());
    
    // write sizes
    if(ascii) {
        int written = fprintf(file, " %d %d %d\n", size1, size2, nc);
        error_if_not_va(written > 0, "problem writing file %s", filename.c_str());
    } else {
        error_if_not_va(fwrite(&size1, sizeof(int), 1, file) == 1, "problem writing file %s", filename.c_str());
        error_if_not_va(fwrite(&size2, sizeof(int), 1, file) == 1, "problem writing file %s", filename.c_str());
		error_if_not_va(fwrite(&nc, sizeof(int), 1, file) == 1, "problem writing file %s", filename.c_str());
    }
    
    // write data
    if(ascii) {
        if(type == 'b') {
            for(int i = 0; i < nc*size1*size2; i ++) {
                int v = ((char*)buffer)[i];
                error_if_not_va(fprintf(file, " %d", v) > 0, "problem writing file %s", filename.c_str());
            }
        } else if(type == 'B') {
            for(int i = 0; i < nc*size1*size2; i ++) {
                unsigned int v = ((unsigned char*)buffer)[i];
                error_if_not_va(fprintf(file, " %ud", v) > 0, "problem writing file %s", filename.c_str());
            }
        } else if(type == 'i') {
            for(int i = 0; i < nc*size1*size2; i ++) {
                int v = ((int*)buffer)[i];
                error_if_not_va(fprintf(file, " %d", v) > 0, "problem writing file %s", filename.c_str());
            }
        } else if(type == 'i') {
            for(int i = 0; i < nc*size1*size2; i ++) {
                unsigned int v = ((int*)buffer)[i];
                error_if_not_va(fprintf(file, " %ud", v) > 0, "problem writing file %s", filename.c_str());
            }
        } else if(type == 'f') {
            for(int i = 0; i < nc*size1*size2; i ++) {
                float v = (float)((int*)buffer)[i];
                error_if_not_va(fprintf(file, " %f", v) > 0, "problem writing file %s", filename.c_str());
            }
        } else error("Not supported ascii format");
    } else {
        int ds = (type == 'b' || type == 'B')?sizeof(char):sizeof(int);
        int written = (int)fwrite(buffer, ds, nc*size1*size2, file);
        error_if_not_va(written == nc*size1*size2, "problem writing file %s", filename.c_str());
    }
    
    // done
    fflush(file);
    fclose(file);
}

template<typename T>
void _impl_loadRaw(const string& filename, sarray<T> &array, rawtype type, int nc) {
    int size1, size2, _nc; rawbuffer buffer;
    loadRaw(filename, type, size1, size2, _nc, buffer);
    error_if_not(_nc == nc, "incorrect number of channels");
    error_if_not(size2 == 1, "raw arrays should have size2 == 1");
    array.resize(size1);
    memcpy(&(array[0]), buffer, sizeof(T)*size1);
	delete [] (T*)buffer;
}

void loadRaw(const string& filename, sarray<float>& array) { _impl_loadRaw(filename, array, 'f', 1); }
void loadRaw(const string& filename, sarray<vec2f>& array) { _impl_loadRaw(filename, array, 'f', 2); }
void loadRaw(const string& filename, sarray<vec3f>& array) { _impl_loadRaw(filename, array, 'f', 3); }
void loadRaw(const string& filename, sarray<int>& array) { _impl_loadRaw(filename, array, 'i', 1); }
void loadRaw(const string& filename, sarray<vec3i>& array)  { _impl_loadRaw(filename, array, 'f', 3); }
void loadRaw(const string& filename, sarray<vec2i>& array)  { _impl_loadRaw(filename, array, 'i', 2); }

template<typename T>
void _impl_saveRaw(const string& filename, const sarray<T> &array, rawtype type, int nc, int ds) {
    int size1 = array.size();
    int size2 = 1;
    rawbuffer buffer;
    buffer = new unsigned char[ds*nc*size1*size2];
    memcpy(buffer, &(array[0]), sizeof(T)*size1);
    saveRaw(filename, type, size1, 1, nc, false, buffer);
    delete [] (T*)buffer;
}
    
template<typename T>
void _impl_saveRaw(const string& filename, const darray<T> &array, rawtype type, int nc, int ds) {
    int size1 = array.size();
    int size2 = 1;
    rawbuffer buffer;
    buffer = new unsigned char[ds*nc*size1*size2];
    memcpy(buffer, &(array[0]), sizeof(T)*size1);
    saveRaw(filename, type, size1, 1, nc, false, buffer);
    delete [] (T*)buffer;
}

void saveRaw(const string& filename, const sarray<vec3f>& array) { _impl_saveRaw(filename, array, 'f', 3, sizeof(float)); }
void saveRaw(const string& filename, const sarray<vec3i>& array) { _impl_saveRaw(filename, array, 'i', 3, sizeof(int)); }
void saveRaw(const string& filename, const sarray<vec2f>& array) { _impl_saveRaw(filename, array, 'f', 2, sizeof(float)); }
void saveRaw(const string& filename, const darray<vec3f>& array) { _impl_saveRaw(filename, array, 'f', 3, sizeof(float)); }
void saveRaw(const string& filename, const darray<vec3i>& array) { _impl_saveRaw(filename, array, 'i', 3, sizeof(int)); }
void saveRaw(const string& filename, const darray<vec2f>& array) { _impl_saveRaw(filename, array, 'f', 2, sizeof(float)); }
