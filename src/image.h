#ifndef _IMAGE_H_
#define _IMAGE_H_

#include "ext/common/vec.h"

#include "std.h"

template<typename T>
struct image {
    image() : _w(0), _h(0) { }
    image(int w, int h) : _w(w), _h(h), _d(w*h,T()) { }
    
    int width() const { return _w; }
    int height() const { return _h; }
    
    T& at(int i, int j) { return _d[j*_w+i]; }
    const T& at(int i, int j) const { return _d[j*_w+i]; }
    
    T* data() { return _d.data(); }
    const T* data() const { return _d.data(); }
    
    int _w, _h;
    vector<T> _d;
};

typedef image<vec3f> image3f;
typedef vec3<unsigned char> vec3ub;
typedef image<vec3ub> image3ub;

template <typename T1, typename T2>
inline image<T1> image_cast(const image<T2>& img) {
    image<T1> ret(img.width(), img.height());
    for(int j = 0; j < img.height(); j ++) {
        for(int i = 0; i < img.width(); i ++) {
            
        }
    }
    return ret;
}

template <typename T>
inline image<T> flipy(const image<T>& img) {
    image<T> ret(img.width(),img.height());
    for(int j = 0; j < img.height(); j ++) {
        for(int i = 0; i < img.width(); i ++) {
            ret.at(i,img.height()-1-j) = img.at(i,j);
        }
    }
    return ret;
}

#endif
