#ifndef _MAT_H_
#define _MAT_H_

#include "ext/common/vec.h"
#include "ext/common/frame.h"

template<typename R>
inline bool operator==(const vec2<R>& a,const vec2<R>& b) {
	return a.x == b.x && a.y == b.y;
}
template<typename R>
inline vec2<R> operator-(const vec2<R>& a) { return makevec2<R>(-a.x,-a.y); }
template<typename R>
inline vec2<R> operator+=(vec2<R> &a,const vec2<R> &b) {
	a.x += b.x;
	a.y += b.y;
	return a;
}
template<typename R>
inline vec2<R> operator-=(vec2<R> &a,const vec2<R> &b) {
	a.x -= b.x;
	a.y -= b.y;
	return a;
}
template<typename R>
inline vec2<R> operator*=(vec2<R> &a,const R &b) { a.x *= b; a.y *= b; return a; }
template<typename R>
inline vec2<R> operator/=(vec2<R> &a,const R &b) { a.x /= b; a.y /= b; return a; }
template <typename R>
inline R dot(const vec2<R> &a, const vec2<R> &b) { return a.x*b.x + a.y*b.y; }




template<typename R>
inline bool operator==(const vec3<R>& a,const vec3<R>& b) {
	return a.x == b.x && a.y == b.y && a.z == b.z;
}
template<typename R>
inline vec3<R> operator/(const float &a, const vec3<R> &b) {
	return makevec3<R>(a/b.x,a/b.y,a/b.z);
}




template<typename R>
inline bool operator==(const vec4<R>& a,const vec4<R>& b) {
	return a.x == b.x && a.y == b.y && a.z == b.z && a.w == b.w;
}
template<typename R>
inline vec4<R> operator-(const vec4<R> &a) { return makevec4<R>(-a.x,-a.y,-a.z,-a.w); }
template<typename R>
inline vec4<R> operator+(const vec4<R> &a,const vec4<R> &b) { return makevec4<R>(a.x+b.x,a.y+b.y,a.z+b.z,a.w+b.w); }
template<typename R>
inline vec4<R> operator-(const vec4<R> &a,const vec4<R> &b) { return makevec4<R>(a.x-b.x,a.y-b.y,a.z-b.z,a.w-b.w); }
template<typename R>
inline vec4<R> operator*(const vec4<R> &a, const R &b) {
	return makevec4<R>(a.x*b,a.y*b,a.z*b,a.w*b);
}
template<typename R>
inline vec4<R> operator*(const R &a, const vec4<R> &b) {
	return makevec4<R>(a*b.x,a*b.y,a*b.z,a*b.w);
}
template<typename R>
inline vec4<R> operator/(const float &a, const vec4<R> &b) {
	return makevec4<R>(a/b.x,a/b.y,a/b.z,a/b.w);
}
template<typename R>
inline vec4<R> operator/(const vec4<R> &a, const float &b) {
	return makevec4<R>(a.x/b,a.y/b,a.z/b,a.w/b);
}
template<typename R>
inline vec4<R> operator+=(vec4<R> &a,const vec4<R> &b) {
	a.x += b.x;
	a.y += b.y;
	a.z += b.z;
	a.w += b.w;
	return a;
}
template<typename R>
inline vec4<R> operator-=(vec4<R> &a,const vec4<R> &b) {
	a.x -= b.x;
	a.y -= b.y;
	a.z -= b.z;
	a.w -= b.w;
	return a;
}
template<typename R>
inline vec4<R> operator*=(vec4<R> &a,const R &b) { a.x *= b; a.y *= b; a.z *= b; a.w *= b; return a; }
template<typename R>
inline vec4<R> operator/=(vec4<R> &a,const R &b) { a.x /= b; a.y /= b; a.z /= b; a.w /= b; return a; }
template <typename R>
inline R dot(const vec4<R> &a, const vec4<R> &b) { return a.x*b.x + a.y*b.y + a.z*b.z + a.w+b.w; }




struct mat2f {
    typedef float base_type;
    typedef vec2f component_type;

    vec2f x, y;

    mat2f() : x(makevec2f(1, 0)), y(makevec2f(0, 1)) { }
    mat2f(const vec2f& x, const vec2f& y) : x(x), y(y) { }
    mat2f(const float& x_x, const float& x_y , const float& y_x, const float& y_y ) : x(makevec2f(x_x, x_y)), y(makevec2f(y_x, y_y)) { }

    vec2f& operator[](int i) { return (&x)[i]; }
    const vec2f& operator[](int i) const { return (&x)[i]; }

    static int size() { return 2; }

    vec2f* begin() { return &x; }
    vec2f* end() { return (&x)+2; }
    const vec2f* begin() const { return &x; }
    const vec2f* end() const { return (&x)+2; }

    vec2f* data() { return &x; }

    float* raw_data() { return &(x.x); }
    static int raw_size() { return 2*sizeof(x); }
};


const mat2f identity_mat2f = mat2f();

inline bool operator==(const mat2f& a, const mat2f& b) { return a.x==b.x && a.y==b.y; }
inline mat2f operator-(const mat2f& a) { return mat2f(-a.x, -a.y); }
inline mat2f operator+(const mat2f& a, const mat2f& b) { return mat2f(a.x+b.x, a.y+b.y); }
inline mat2f& operator+=(mat2f& a, const mat2f& b) { a.x+=b.x; a.y+=b.y; return a; }
inline mat2f operator-(const mat2f& a, const mat2f& b) { return mat2f(a.x-b.x, a.y-b.y); }
inline mat2f& operator-=(mat2f& a, const mat2f& b) { a.x-=b.x; a.y-=b.y; return a; }
inline mat2f operator*(const mat2f& a, const float& b) { return mat2f(a.x*b, a.y*b); }
inline mat2f operator*(const float& a, const mat2f& b) { return mat2f(b.x*a, b.y*a); }
inline mat2f& operator*=(mat2f& a, const float& b) { a.x*=b; a.y*=b; return a; }
inline mat2f operator/(const mat2f& a, const float& b) { return mat2f(a.x/b, a.y/b); }
inline mat2f& operator/=(mat2f& a, const float& b) { a.x/=b; a.y/=b; return a; }

inline mat2f operator*(const mat2f& a, const mat2f& b) { return mat2f(a.x.x*b.x.x+a.x.y*b.y.x , a.x.x*b.x.y+a.x.y*b.y.y  , a.y.x*b.x.x+a.y.y*b.y.x , a.y.x*b.x.y+a.y.y*b.y.y  ); }
inline mat2f& operator*=(mat2f& a, const mat2f& b) { a = a*b; return a; }
inline vec2f operator*(const mat2f& a, const vec2f& b) { return makevec2f(dot(a.x,b), dot(a.y,b)); }

inline mat2f transpose(const mat2f& a) { return mat2f(a.x.x, a.y.x , a.x.y, a.y.y ); }
inline float determinant(const mat2f& a) { return a.x.x*a.y.y-a.x.y*a.y.x; }
inline mat2f inverse(const mat2f& a) { float d = determinant(a); float di = 1/d; return mat2f(a.y.y,-a.x.y,-a.x.y,a.x.x)*di; }


struct mat2d {
    typedef double base_type;
    typedef vec2d component_type;

    vec2d x, y;

    mat2d() : x(makevec2d(1, 0)), y(makevec2d(0, 1)) { }
    mat2d(const vec2d& x, const vec2d& y) : x(x), y(y) { }
    mat2d(const double& x_x, const double& x_y , const double& y_x, const double& y_y ) :
		x(makevec2d(x_x, x_y)), y(makevec2d(y_x, y_y)) { }

    vec2d& operator[](int i) { return (&x)[i]; }
    const vec2d& operator[](int i) const { return (&x)[i]; }

    static int size() { return 2; }

    vec2d* begin() { return &x; }
    vec2d* end() { return (&x)+2; }
    const vec2d* begin() const { return &x; }
    const vec2d* end() const { return (&x)+2; }

    vec2d* data() { return &x; }

    double* raw_data() { return &(x.x); }
    static int raw_size() { return 2*sizeof(x); }
};


const mat2d identity_mat2d = mat2d();

inline bool operator==(const mat2d& a, const mat2d& b) { return a.x==b.x && a.y==b.y; }
inline mat2d operator-(const mat2d& a) { return mat2d(-a.x, -a.y); }
inline mat2d operator+(const mat2d& a, const mat2d& b) { return mat2d(a.x+b.x, a.y+b.y); }
inline mat2d& operator+=(mat2d& a, const mat2d& b) { a.x+=b.x; a.y+=b.y; return a; }
inline mat2d operator-(const mat2d& a, const mat2d& b) { return mat2d(a.x-b.x, a.y-b.y); }
inline mat2d& operator-=(mat2d& a, const mat2d& b) { a.x-=b.x; a.y-=b.y; return a; }
inline mat2d operator*(const mat2d& a, const double& b) { return mat2d(a.x*b, a.y*b); }
inline mat2d operator*(const double& a, const mat2d& b) { return mat2d(b.x*a, b.y*a); }
inline mat2d& operator*=(mat2d& a, const double& b) { a.x*=b; a.y*=b; return a; }
inline mat2d operator/(const mat2d& a, const double& b) { return mat2d(a.x/b, a.y/b); }
inline mat2d& operator/=(mat2d& a, const double& b) { a.x/=b; a.y/=b; return a; }

inline mat2d operator*(const mat2d& a, const mat2d& b) { return mat2d(a.x.x*b.x.x+a.x.y*b.y.x , a.x.x*b.x.y+a.x.y*b.y.y  , a.y.x*b.x.x+a.y.y*b.y.x , a.y.x*b.x.y+a.y.y*b.y.y  ); }
inline mat2d& operator*=(mat2d& a, const mat2d& b) { a = a*b; return a; }
inline vec2d operator*(const mat2d& a, const vec2d& b) { return makevec2d(dot(a.x,b), dot(a.y,b)); }

inline mat2d transpose(const mat2d& a) { return mat2d(a.x.x, a.y.x , a.x.y, a.y.y ); }
inline double determinant(const mat2d& a) { return a.x.x*a.y.y-a.x.y*a.y.x; }
inline mat2d inverse(const mat2d& a) { double d = determinant(a); double di = 1/d; return mat2d(a.y.y,-a.x.y,-a.x.y,a.x.x)*di; }


struct mat3f {
    typedef float base_type;
    typedef vec3f component_type;

    vec3f x, y, z;

    mat3f() : x(makevec3f(1, 0, 0)), y(makevec3f(0, 1, 0)), z(makevec3f(0, 0, 1)) { }
    mat3f(const vec3f& x, const vec3f& y, const vec3f& z) : x(x), y(y), z(z) { }
    mat3f(const float& x_x, const float& x_y, const float& x_z , const float& y_x, const float& y_y, const float& y_z , const float& z_x, const float& z_y, const float& z_z ) :
		x(makevec3f(x_x, x_y, x_z)), y(makevec3f(y_x, y_y, y_z)), z(makevec3f(z_x, z_y, z_z)) { }

    vec3f& operator[](int i) { return (&x)[i]; }
    const vec3f& operator[](int i) const { return (&x)[i]; }

    static int size() { return 3; }

    vec3f* begin() { return &x; }
    vec3f* end() { return (&x)+3; }
    const vec3f* begin() const { return &x; }
    const vec3f* end() const { return (&x)+3; }

    vec3f* data() { return &x; }

    float* raw_data() { return &(x.x); }
    static int raw_size() { return 3*sizeof(x); }
};


const mat3f identity_mat3f = mat3f();

inline bool operator==(const mat3f& a, const mat3f& b) { return a.x==b.x && a.y==b.y && a.z==b.z; }
inline mat3f operator-(const mat3f& a) { return mat3f(-a.x, -a.y, -a.z); }
inline mat3f operator+(const mat3f& a, const mat3f& b) { return mat3f(a.x+b.x, a.y+b.y, a.z+b.z); }
inline mat3f& operator+=(mat3f& a, const mat3f& b) { a.x+=b.x; a.y+=b.y; a.z+=b.z; return a; }
inline mat3f operator-(const mat3f& a, const mat3f& b) { return mat3f(a.x-b.x, a.y-b.y, a.z-b.z); }
inline mat3f& operator-=(mat3f& a, const mat3f& b) { a.x-=b.x; a.y-=b.y; a.z-=b.z; return a; }
inline mat3f operator*(const mat3f& a, const float& b) { return mat3f(a.x*b, a.y*b, a.z*b); }
inline mat3f operator*(const float& a, const mat3f& b) { return mat3f(a*b.x, a*b.y, a*b.z); }
inline mat3f& operator*=(mat3f& a, const float& b) { a.x*=b; a.y*=b; a.z*=b; return a; }
inline mat3f operator/(const mat3f& a, const float& b) { return mat3f(a.x/b, a.y/b, a.z/b); }
inline mat3f& operator/=(mat3f& a, const float& b) { a.x/=b; a.y/=b; a.z/=b; return a; }

inline mat3f operator*(const mat3f& a, const mat3f& b) { return mat3f(a.x.x*b.x.x+a.x.y*b.y.x+a.x.z*b.z.x , a.x.x*b.x.y+a.x.y*b.y.y+a.x.z*b.z.y , a.x.x*b.x.z+a.x.y*b.y.z+a.x.z*b.z.z  , a.y.x*b.x.x+a.y.y*b.y.x+a.y.z*b.z.x , a.y.x*b.x.y+a.y.y*b.y.y+a.y.z*b.z.y , a.y.x*b.x.z+a.y.y*b.y.z+a.y.z*b.z.z  , a.z.x*b.x.x+a.z.y*b.y.x+a.z.z*b.z.x , a.z.x*b.x.y+a.z.y*b.y.y+a.z.z*b.z.y , a.z.x*b.x.z+a.z.y*b.y.z+a.z.z*b.z.z  ); }
inline mat3f& operator*=(mat3f& a, const mat3f& b) { a = a*b; return a; }
inline vec3f operator*(const mat3f& a, const vec3f& b) { return makevec3f(dot(a.x,b), dot(a.y,b), dot(a.z,b)); }

inline mat3f transpose(const mat3f& a) { return mat3f(a.x.x, a.y.x, a.z.x , a.x.y, a.y.y, a.z.y , a.x.z, a.y.z, a.z.z ); }
inline float determinant(const mat3f& a) { return +a.x.x*(a.y.y*a.z.z-a.y.z*a.z.y)-a.x.y*(a.y.x*a.z.z-a.y.z*a.z.x)+a.x.z*(a.y.x*a.z.y-a.y.y*a.z.x); }
inline mat3f inverse(const mat3f& a) { float d = determinant(a); float di = 1/d; return mat3f(+(a.y.y*a.z.z-a.y.z*a.z.y), -(a.x.y*a.z.z-a.x.z*a.z.y), +(a.x.y*a.y.z-a.x.z*a.y.y), -(a.y.x*a.z.z-a.y.z*a.z.x), +(a.x.x*a.z.z-a.x.z*a.z.x), -(a.x.x*a.y.z-a.x.z*a.y.x), +(a.y.x*a.z.y-a.y.y*a.z.x), -(a.x.x*a.z.y-a.x.y*a.z.x), +(a.x.x*a.y.y-a.x.y*a.y.x))*di; }


struct mat3d {
    typedef double base_type;
    typedef vec3d component_type;

    vec3d x, y, z;

    mat3d() : x(makevec3d(1, 0, 0)), y(makevec3d(0, 1, 0)), z(makevec3d(0, 0, 1)) { }
    mat3d(const vec3d& x, const vec3d& y, const vec3d& z) : x(x), y(y), z(z) { }
    mat3d(const double& x_x, const double& x_y, const double& x_z , const double& y_x, const double& y_y, const double& y_z , const double& z_x, const double& z_y, const double& z_z ) :
		x(makevec3d(x_x, x_y, x_z)), y(makevec3d(y_x, y_y, y_z)), z(makevec3d(z_x, z_y, z_z)) { }

    vec3d& operator[](int i) { return (&x)[i]; }
    const vec3d& operator[](int i) const { return (&x)[i]; }

    static int size() { return 3; }

    vec3d* begin() { return &x; }
    vec3d* end() { return (&x)+3; }
    const vec3d* begin() const { return &x; }
    const vec3d* end() const { return (&x)+3; }

    vec3d* data() { return &x; }

    double* raw_data() { return &(x.x); }
    static int raw_size() { return 3*sizeof(x); }
};


const mat3d identity_mat3d = mat3d();

inline bool operator==(const mat3d& a, const mat3d& b) { return a.x==b.x && a.y==b.y && a.z==b.z; }
inline mat3d operator-(const mat3d& a) { return mat3d(-a.x, -a.y, -a.z); }
inline mat3d operator+(const mat3d& a, const mat3d& b) { return mat3d(a.x+b.x, a.y+b.y, a.z+b.z); }
inline mat3d& operator+=(mat3d& a, const mat3d& b) { a.x+=b.x; a.y+=b.y; a.z+=b.z; return a; }
inline mat3d operator-(const mat3d& a, const mat3d& b) { return mat3d(a.x-b.x, a.y-b.y, a.z-b.z); }
inline mat3d& operator-=(mat3d& a, const mat3d& b) { a.x-=b.x; a.y-=b.y; a.z-=b.z; return a; }
inline mat3d operator*(const mat3d& a, const double& b) { return mat3d(a.x*b, a.y*b, a.z*b); }
inline mat3d operator*(const double& a, const mat3d& b) { return mat3d(a*b.x, a*b.y, a*b.z); }
inline mat3d& operator*=(mat3d& a, const double& b) { a.x*=b; a.y*=b; a.z*=b; return a; }
inline mat3d operator/(const mat3d& a, const double& b) { return mat3d(a.x/b, a.y/b, a.z/b); }
inline mat3d& operator/=(mat3d& a, const double& b) { a.x/=b; a.y/=b; a.z/=b; return a; }

inline mat3d operator*(const mat3d& a, const mat3d& b) { return mat3d(a.x.x*b.x.x+a.x.y*b.y.x+a.x.z*b.z.x , a.x.x*b.x.y+a.x.y*b.y.y+a.x.z*b.z.y , a.x.x*b.x.z+a.x.y*b.y.z+a.x.z*b.z.z  , a.y.x*b.x.x+a.y.y*b.y.x+a.y.z*b.z.x , a.y.x*b.x.y+a.y.y*b.y.y+a.y.z*b.z.y , a.y.x*b.x.z+a.y.y*b.y.z+a.y.z*b.z.z  , a.z.x*b.x.x+a.z.y*b.y.x+a.z.z*b.z.x , a.z.x*b.x.y+a.z.y*b.y.y+a.z.z*b.z.y , a.z.x*b.x.z+a.z.y*b.y.z+a.z.z*b.z.z  ); }
inline mat3d& operator*=(mat3d& a, const mat3d& b) { a = a*b; return a; }
inline vec3d operator*(const mat3d& a, const vec3d& b) { return makevec3d(dot(a.x,b), dot(a.y,b), dot(a.z,b)); }

inline mat3d transpose(const mat3d& a) { return mat3d(a.x.x, a.y.x, a.z.x , a.x.y, a.y.y, a.z.y , a.x.z, a.y.z, a.z.z ); }
inline double determinant(const mat3d& a) { return +a.x.x*(a.y.y*a.z.z-a.y.z*a.z.y)-a.x.y*(a.y.x*a.z.z-a.y.z*a.z.x)+a.x.z*(a.y.x*a.z.y-a.y.y*a.z.x); }
inline mat3d inverse(const mat3d& a) { double d = determinant(a); double di = 1/d; return mat3d(+(a.y.y*a.z.z-a.y.z*a.z.y), -(a.x.y*a.z.z-a.x.z*a.z.y), +(a.x.y*a.y.z-a.x.z*a.y.y), -(a.y.x*a.z.z-a.y.z*a.z.x), +(a.x.x*a.z.z-a.x.z*a.z.x), -(a.x.x*a.y.z-a.x.z*a.y.x), +(a.y.x*a.z.y-a.y.y*a.z.x), -(a.x.x*a.z.y-a.x.y*a.z.x), +(a.x.x*a.y.y-a.x.y*a.y.x))*di; }


struct mat4f {
    typedef float base_type;
    typedef vec4f component_type;

    vec4f x, y, z, w;

    mat4f() : x(makevec4f(1, 0, 0, 0)), y(makevec4f(0, 1, 0, 0)), z(makevec4f(0, 0, 1, 0)), w(makevec4f(0, 0, 0, 1)) { }
    mat4f(const vec4f& x, const vec4f& y, const vec4f& z, const vec4f& w) : x(x), y(y), z(z), w(w) { }
    mat4f(const float& x_x, const float& x_y, const float& x_z, const float& x_w , const float& y_x, const float& y_y, const float& y_z, const float& y_w , const float& z_x, const float& z_y, const float& z_z, const float& z_w , const float& w_x, const float& w_y, const float& w_z, const float& w_w ) :
		x(makevec4f(x_x, x_y, x_z, x_w)), y(makevec4f(y_x, y_y, y_z, y_w)), z(makevec4f(z_x, z_y, z_z, z_w)), w(makevec4f(w_x, w_y, w_z, w_w)) { }

    vec4f& operator[](int i) { return (&x)[i]; }
    const vec4f& operator[](int i) const { return (&x)[i]; }

    static int size() { return 4; }

    vec4f* begin() { return &x; }
    vec4f* end() { return (&x)+4; }
    const vec4f* begin() const { return &x; }
    const vec4f* end() const { return (&x)+4; }

    vec4f* data() { return &x; }

    float* raw_data() { return &(x.x); }
    static int raw_size() { return 4*sizeof(x); }
};


const mat4f identity_mat4f = mat4f();

inline bool operator==(const mat4f& a, const mat4f& b) { return a.x==b.x && a.y==b.y && a.z==b.z && a.w==b.w; }
inline mat4f operator-(const mat4f& a) { return mat4f(-a.x, -a.y, -a.z, -a.w); }
inline mat4f operator+(const mat4f& a, const mat4f& b) { return mat4f(a.x+b.x, a.y+b.y, a.z+b.z, a.w+b.w); }
inline mat4f& operator+=(mat4f& a, const mat4f& b) { a.x+=b.x; a.y+=b.y; a.z+=b.z; a.w+=b.w; return a; }
inline mat4f operator-(const mat4f& a, const mat4f& b) { return mat4f(a.x-b.x, a.y-b.y, a.z-b.z, a.w-b.w); }
inline mat4f& operator-=(mat4f& a, const mat4f& b) { a.x-=b.x; a.y-=b.y; a.z-=b.z; a.w-=b.w; return a; }
inline mat4f operator*(const mat4f& a, const float& b) { return mat4f(a.x*b, a.y*b, a.z*b, a.w*b); }
inline mat4f operator*(const float& a, const mat4f& b) { return mat4f(a*b.x, a*b.y, a*b.z, a*b.w); }
inline mat4f& operator*=(mat4f& a, const float& b) { a.x*=b; a.y*=b; a.z*=b; a.w*=b; return a; }
inline mat4f operator/(const mat4f& a, const float& b) { return mat4f(a.x/b, a.y/b, a.z/b, a.w/b); }
inline mat4f& operator/=(mat4f& a, const float& b) { a.x/=b; a.y/=b; a.z/=b; a.w/=b; return a; }

inline mat4f operator*(const mat4f& a, const mat4f& b) { return mat4f(a.x.x*b.x.x+a.x.y*b.y.x+a.x.z*b.z.x+a.x.w*b.w.x , a.x.x*b.x.y+a.x.y*b.y.y+a.x.z*b.z.y+a.x.w*b.w.y , a.x.x*b.x.z+a.x.y*b.y.z+a.x.z*b.z.z+a.x.w*b.w.z , a.x.x*b.x.w+a.x.y*b.y.w+a.x.z*b.z.w+a.x.w*b.w.w  , a.y.x*b.x.x+a.y.y*b.y.x+a.y.z*b.z.x+a.y.w*b.w.x , a.y.x*b.x.y+a.y.y*b.y.y+a.y.z*b.z.y+a.y.w*b.w.y , a.y.x*b.x.z+a.y.y*b.y.z+a.y.z*b.z.z+a.y.w*b.w.z , a.y.x*b.x.w+a.y.y*b.y.w+a.y.z*b.z.w+a.y.w*b.w.w  , a.z.x*b.x.x+a.z.y*b.y.x+a.z.z*b.z.x+a.z.w*b.w.x , a.z.x*b.x.y+a.z.y*b.y.y+a.z.z*b.z.y+a.z.w*b.w.y , a.z.x*b.x.z+a.z.y*b.y.z+a.z.z*b.z.z+a.z.w*b.w.z , a.z.x*b.x.w+a.z.y*b.y.w+a.z.z*b.z.w+a.z.w*b.w.w  , a.w.x*b.x.x+a.w.y*b.y.x+a.w.z*b.z.x+a.w.w*b.w.x , a.w.x*b.x.y+a.w.y*b.y.y+a.w.z*b.z.y+a.w.w*b.w.y , a.w.x*b.x.z+a.w.y*b.y.z+a.w.z*b.z.z+a.w.w*b.w.z , a.w.x*b.x.w+a.w.y*b.y.w+a.w.z*b.z.w+a.w.w*b.w.w  ); }
inline mat4f& operator*=(mat4f& a, const mat4f& b) { a = a*b; return a; }
inline vec4f operator*(const mat4f& a, const vec4f& b) { return makevec4f(dot(a.x,b), dot(a.y,b), dot(a.z,b), dot(a.w,b)); }

inline mat4f transpose(const mat4f& a) { return mat4f(a.x.x, a.y.x, a.z.x, a.w.x , a.x.y, a.y.y, a.z.y, a.w.y , a.x.z, a.y.z, a.z.z, a.w.z , a.x.w, a.y.w, a.z.w, a.w.w ); }
inline float determinant(const mat4f& a) { return +a.x.x*(+a.y.y*(a.z.z*a.w.w-a.z.w*a.w.z)-a.y.z*(a.z.y*a.w.w-a.z.w*a.w.y)+a.y.w*(a.z.y*a.w.z-a.z.z*a.w.y))-a.x.y*(+a.y.x*(a.z.z*a.w.w-a.z.w*a.w.z)-a.y.z*(a.z.x*a.w.w-a.z.w*a.w.x)+a.y.w*(a.z.x*a.w.z-a.z.z*a.w.x))+a.x.z*(+a.y.x*(a.z.y*a.w.w-a.z.w*a.w.y)-a.y.y*(a.z.x*a.w.w-a.z.w*a.w.x)+a.y.w*(a.z.x*a.w.y-a.z.y*a.w.x))-a.x.w*(+a.y.x*(a.z.y*a.w.z-a.z.z*a.w.y)-a.y.y*(a.z.x*a.w.z-a.z.z*a.w.x)+a.y.z*(a.z.x*a.w.y-a.z.y*a.w.x)); }
inline mat4f inverse(const mat4f& a) { float d = determinant(a); float di = 1/d; return mat4f(+(+a.y.y*(a.z.z*a.w.w-a.z.w*a.w.z)-a.y.z*(a.z.y*a.w.w-a.z.w*a.w.y)+a.y.w*(a.z.y*a.w.z-a.z.z*a.w.y)), -(+a.x.y*(a.z.z*a.w.w-a.z.w*a.w.z)-a.x.z*(a.z.y*a.w.w-a.z.w*a.w.y)+a.x.w*(a.z.y*a.w.z-a.z.z*a.w.y)), +(+a.x.y*(a.y.z*a.w.w-a.y.w*a.w.z)-a.x.z*(a.y.y*a.w.w-a.y.w*a.w.y)+a.x.w*(a.y.y*a.w.z-a.y.z*a.w.y)), -(+a.x.y*(a.y.z*a.z.w-a.y.w*a.z.z)-a.x.z*(a.y.y*a.z.w-a.y.w*a.z.y)+a.x.w*(a.y.y*a.z.z-a.y.z*a.z.y)), -(+a.y.x*(a.z.z*a.w.w-a.z.w*a.w.z)-a.y.z*(a.z.x*a.w.w-a.z.w*a.w.x)+a.y.w*(a.z.x*a.w.z-a.z.z*a.w.x)), +(+a.x.x*(a.z.z*a.w.w-a.z.w*a.w.z)-a.x.z*(a.z.x*a.w.w-a.z.w*a.w.x)+a.x.w*(a.z.x*a.w.z-a.z.z*a.w.x)), -(+a.x.x*(a.y.z*a.w.w-a.y.w*a.w.z)-a.x.z*(a.y.x*a.w.w-a.y.w*a.w.x)+a.x.w*(a.y.x*a.w.z-a.y.z*a.w.x)), +(+a.x.x*(a.y.z*a.z.w-a.y.w*a.z.z)-a.x.z*(a.y.x*a.z.w-a.y.w*a.z.x)+a.x.w*(a.y.x*a.z.z-a.y.z*a.z.x)), +(+a.y.x*(a.z.y*a.w.w-a.z.w*a.w.y)-a.y.y*(a.z.x*a.w.w-a.z.w*a.w.x)+a.y.w*(a.z.x*a.w.y-a.z.y*a.w.x)), -(+a.x.x*(a.z.y*a.w.w-a.z.w*a.w.y)-a.x.y*(a.z.x*a.w.w-a.z.w*a.w.x)+a.x.w*(a.z.x*a.w.y-a.z.y*a.w.x)), +(+a.x.x*(a.y.y*a.w.w-a.y.w*a.w.y)-a.x.y*(a.y.x*a.w.w-a.y.w*a.w.x)+a.x.w*(a.y.x*a.w.y-a.y.y*a.w.x)), -(+a.x.x*(a.y.y*a.z.w-a.y.w*a.z.y)-a.x.y*(a.y.x*a.z.w-a.y.w*a.z.x)+a.x.w*(a.y.x*a.z.y-a.y.y*a.z.x)), -(+a.y.x*(a.z.y*a.w.z-a.z.z*a.w.y)-a.y.y*(a.z.x*a.w.z-a.z.z*a.w.x)+a.y.z*(a.z.x*a.w.y-a.z.y*a.w.x)), +(+a.x.x*(a.z.y*a.w.z-a.z.z*a.w.y)-a.x.y*(a.z.x*a.w.z-a.z.z*a.w.x)+a.x.z*(a.z.x*a.w.y-a.z.y*a.w.x)), -(+a.x.x*(a.y.y*a.w.z-a.y.z*a.w.y)-a.x.y*(a.y.x*a.w.z-a.y.z*a.w.x)+a.x.z*(a.y.x*a.w.y-a.y.y*a.w.x)), +(+a.x.x*(a.y.y*a.z.z-a.y.z*a.z.y)-a.x.y*(a.y.x*a.z.z-a.y.z*a.z.x)+a.x.z*(a.y.x*a.z.y-a.y.y*a.z.x)))*di; }

inline bool isaffine(const mat4f& m) { return m.w.x==0 and m.w.y==0 and m.w.z==0 and m.w.w==1; }

inline mat4f translate(const vec3f& t) { return mat4f(1,0,0,t.x,  0,1,0,t.y,  0,0,1,t.z,  0,0,0,1); }
inline mat4f rotate(const float& angle, const vec3f& axis) { float c = cos(angle); float s = sin(angle); vec3f vv = normalize(axis); return mat4f(c + (1-c)*vv.x*vv.x, (1-c)*vv.x*vv.y - s*vv.z, (1-c)*vv.x*vv.z + s*vv.y, 0,  (1-c)*vv.x*vv.y + s*vv.z, c + (1-c)*vv.y*vv.y, (1-c)*vv.y*vv.z - s*vv.x, 0,  (1-c)*vv.x*vv.z - s*vv.y, (1-c)*vv.y*vv.z + s*vv.x, c + (1-c)*vv.z*vv.z, 0,  0,0,0,1); }
inline mat4f scale(const vec3f& s) { return mat4f(s.x,0,0,0,  0,s.y,0,0,  0,0,s.z,0,  0,0,0,1); }
inline mat4f frustum(float l, float r, float b, float t, float n, float f) { return mat4f(2*n/(r-l), 0, (r+l)/(r-l), 0,  0, 2*n/(t-b), (t+b)/(t-b), 0,  0, 0, -(f+n)/(f-n), -2*f*n/(f-n), 0, 0, -1, 0); }
inline mat4f ortho(float l, float r, float b, float t, float n, float f) { return mat4f(2/(r-l), 0, 0, -(r+l)/(r-l),  0, 2/(t-b), 0, -(t+b)/(t-b),  0, 0, -2/(f-n), -(f+n)/(f-n), 0, 0, 0, 1); }

inline mat4f lookat(const vec3f& eye, const vec3f& center, const vec3f& up) { vec3f w = normalize(eye - center); vec3f u = normalize(cross(up,w)); vec3f v = cross(w,u); return mat4f(u.x, u.y, u.z, -dot(u,eye),  v.x, v.y, v.z, -dot(v,eye),  w.x, w.y, w.z, -dot(w,eye),  0, 0, 0, 1); }
inline mat4f ortho2d(float l, float r, float b, float t) { return ortho(l,r,b,t,-1,1); }
inline mat4f perspective(float fovy, float aspect, float near, float far) { float f = 1/tan(fovy/2); return mat4f(f/aspect,0,0,0,   0,f,0,0,    0,0,(far+near)/(near-far),2*far*near/(near-far),    0,0,-1,0); }

struct mat4d {
    typedef double base_type;
    typedef vec4d component_type;

    vec4d x, y, z, w;

    mat4d() : x(makevec4d(1, 0, 0, 0)), y(makevec4d(0, 1, 0, 0)), z(makevec4d(0, 0, 1, 0)), w(makevec4d(0, 0, 0, 1)) { }
    mat4d(const vec4d& x, const vec4d& y, const vec4d& z, const vec4d& w) : x(x), y(y), z(z), w(w) { }
    mat4d(const double& x_x, const double& x_y, const double& x_z, const double& x_w , const double& y_x, const double& y_y, const double& y_z, const double& y_w , const double& z_x, const double& z_y, const double& z_z, const double& z_w , const double& w_x, const double& w_y, const double& w_z, const double& w_w ) :
		x(makevec4d(x_x, x_y, x_z, x_w)), y(makevec4d(y_x, y_y, y_z, y_w)), z(makevec4d(z_x, z_y, z_z, z_w)), w(makevec4d(w_x, w_y, w_z, w_w)) { }

    vec4d& operator[](int i) { return (&x)[i]; }
    const vec4d& operator[](int i) const { return (&x)[i]; }

    static int size() { return 4; }

    vec4d* begin() { return &x; }
    vec4d* end() { return (&x)+4; }
    const vec4d* begin() const { return &x; }
    const vec4d* end() const { return (&x)+4; }

    vec4d* data() { return &x; }

    double* raw_data() { return &(x.x); }
    static int raw_size() { return 4*sizeof(x); }
};


const mat4d identity_mat4d = mat4d();

inline bool operator==(const mat4d& a, const mat4d& b) { return a.x==b.x && a.y==b.y && a.z==b.z && a.w==b.w; }
inline mat4d operator-(const mat4d& a) { return mat4d(-a.x, -a.y, -a.z, -a.w); }
inline mat4d operator+(const mat4d& a, const mat4d& b) { return mat4d(a.x+b.x, a.y+b.y, a.z+b.z, a.w+b.w); }
inline mat4d& operator+=(mat4d& a, const mat4d& b) { a.x+=b.x; a.y+=b.y; a.z+=b.z; a.w+=b.w; return a; }
inline mat4d operator-(const mat4d& a, const mat4d& b) { return mat4d(a.x-b.x, a.y-b.y, a.z-b.z, a.w-b.w); }
inline mat4d& operator-=(mat4d& a, const mat4d& b) { a.x-=b.x; a.y-=b.y; a.z-=b.z; a.w-=b.w; return a; }
inline mat4d operator*(const mat4d& a, const double& b) { return mat4d(a.x*b, a.y*b, a.z*b, a.w*b); }
inline mat4d operator*(const double& a, const mat4d& b) { return mat4d(a*b.x, a*b.y, a*b.z, a*b.w); }
inline mat4d& operator*=(mat4d& a, const double& b) { a.x*=b; a.y*=b; a.z*=b; a.w*=b; return a; }
inline mat4d operator/(const mat4d& a, const double& b) { return mat4d(a.x/b, a.y/b, a.z/b, a.w/b); }
inline mat4d& operator/=(mat4d& a, const double& b) { a.x/=b; a.y/=b; a.z/=b; a.w/=b; return a; }

inline mat4d operator*(const mat4d& a, const mat4d& b) { return mat4d(a.x.x*b.x.x+a.x.y*b.y.x+a.x.z*b.z.x+a.x.w*b.w.x , a.x.x*b.x.y+a.x.y*b.y.y+a.x.z*b.z.y+a.x.w*b.w.y , a.x.x*b.x.z+a.x.y*b.y.z+a.x.z*b.z.z+a.x.w*b.w.z , a.x.x*b.x.w+a.x.y*b.y.w+a.x.z*b.z.w+a.x.w*b.w.w  , a.y.x*b.x.x+a.y.y*b.y.x+a.y.z*b.z.x+a.y.w*b.w.x , a.y.x*b.x.y+a.y.y*b.y.y+a.y.z*b.z.y+a.y.w*b.w.y , a.y.x*b.x.z+a.y.y*b.y.z+a.y.z*b.z.z+a.y.w*b.w.z , a.y.x*b.x.w+a.y.y*b.y.w+a.y.z*b.z.w+a.y.w*b.w.w  , a.z.x*b.x.x+a.z.y*b.y.x+a.z.z*b.z.x+a.z.w*b.w.x , a.z.x*b.x.y+a.z.y*b.y.y+a.z.z*b.z.y+a.z.w*b.w.y , a.z.x*b.x.z+a.z.y*b.y.z+a.z.z*b.z.z+a.z.w*b.w.z , a.z.x*b.x.w+a.z.y*b.y.w+a.z.z*b.z.w+a.z.w*b.w.w  , a.w.x*b.x.x+a.w.y*b.y.x+a.w.z*b.z.x+a.w.w*b.w.x , a.w.x*b.x.y+a.w.y*b.y.y+a.w.z*b.z.y+a.w.w*b.w.y , a.w.x*b.x.z+a.w.y*b.y.z+a.w.z*b.z.z+a.w.w*b.w.z , a.w.x*b.x.w+a.w.y*b.y.w+a.w.z*b.z.w+a.w.w*b.w.w  ); }
inline mat4d& operator*=(mat4d& a, const mat4d& b) { a = a*b; return a; }
inline vec4d operator*(const mat4d& a, const vec4d& b) { return makevec4d(dot(a.x,b), dot(a.y,b), dot(a.z,b), dot(a.w,b)); }

inline mat4d transpose(const mat4d& a) { return mat4d(a.x.x, a.y.x, a.z.x, a.w.x , a.x.y, a.y.y, a.z.y, a.w.y , a.x.z, a.y.z, a.z.z, a.w.z , a.x.w, a.y.w, a.z.w, a.w.w ); }
inline double determinant(const mat4d& a) { return +a.x.x*(+a.y.y*(a.z.z*a.w.w-a.z.w*a.w.z)-a.y.z*(a.z.y*a.w.w-a.z.w*a.w.y)+a.y.w*(a.z.y*a.w.z-a.z.z*a.w.y))-a.x.y*(+a.y.x*(a.z.z*a.w.w-a.z.w*a.w.z)-a.y.z*(a.z.x*a.w.w-a.z.w*a.w.x)+a.y.w*(a.z.x*a.w.z-a.z.z*a.w.x))+a.x.z*(+a.y.x*(a.z.y*a.w.w-a.z.w*a.w.y)-a.y.y*(a.z.x*a.w.w-a.z.w*a.w.x)+a.y.w*(a.z.x*a.w.y-a.z.y*a.w.x))-a.x.w*(+a.y.x*(a.z.y*a.w.z-a.z.z*a.w.y)-a.y.y*(a.z.x*a.w.z-a.z.z*a.w.x)+a.y.z*(a.z.x*a.w.y-a.z.y*a.w.x)); }
inline mat4d inverse(const mat4d& a) { double d = determinant(a); double di = 1/d; return mat4d(+(+a.y.y*(a.z.z*a.w.w-a.z.w*a.w.z)-a.y.z*(a.z.y*a.w.w-a.z.w*a.w.y)+a.y.w*(a.z.y*a.w.z-a.z.z*a.w.y)), -(+a.x.y*(a.z.z*a.w.w-a.z.w*a.w.z)-a.x.z*(a.z.y*a.w.w-a.z.w*a.w.y)+a.x.w*(a.z.y*a.w.z-a.z.z*a.w.y)), +(+a.x.y*(a.y.z*a.w.w-a.y.w*a.w.z)-a.x.z*(a.y.y*a.w.w-a.y.w*a.w.y)+a.x.w*(a.y.y*a.w.z-a.y.z*a.w.y)), -(+a.x.y*(a.y.z*a.z.w-a.y.w*a.z.z)-a.x.z*(a.y.y*a.z.w-a.y.w*a.z.y)+a.x.w*(a.y.y*a.z.z-a.y.z*a.z.y)), -(+a.y.x*(a.z.z*a.w.w-a.z.w*a.w.z)-a.y.z*(a.z.x*a.w.w-a.z.w*a.w.x)+a.y.w*(a.z.x*a.w.z-a.z.z*a.w.x)), +(+a.x.x*(a.z.z*a.w.w-a.z.w*a.w.z)-a.x.z*(a.z.x*a.w.w-a.z.w*a.w.x)+a.x.w*(a.z.x*a.w.z-a.z.z*a.w.x)), -(+a.x.x*(a.y.z*a.w.w-a.y.w*a.w.z)-a.x.z*(a.y.x*a.w.w-a.y.w*a.w.x)+a.x.w*(a.y.x*a.w.z-a.y.z*a.w.x)), +(+a.x.x*(a.y.z*a.z.w-a.y.w*a.z.z)-a.x.z*(a.y.x*a.z.w-a.y.w*a.z.x)+a.x.w*(a.y.x*a.z.z-a.y.z*a.z.x)), +(+a.y.x*(a.z.y*a.w.w-a.z.w*a.w.y)-a.y.y*(a.z.x*a.w.w-a.z.w*a.w.x)+a.y.w*(a.z.x*a.w.y-a.z.y*a.w.x)), -(+a.x.x*(a.z.y*a.w.w-a.z.w*a.w.y)-a.x.y*(a.z.x*a.w.w-a.z.w*a.w.x)+a.x.w*(a.z.x*a.w.y-a.z.y*a.w.x)), +(+a.x.x*(a.y.y*a.w.w-a.y.w*a.w.y)-a.x.y*(a.y.x*a.w.w-a.y.w*a.w.x)+a.x.w*(a.y.x*a.w.y-a.y.y*a.w.x)), -(+a.x.x*(a.y.y*a.z.w-a.y.w*a.z.y)-a.x.y*(a.y.x*a.z.w-a.y.w*a.z.x)+a.x.w*(a.y.x*a.z.y-a.y.y*a.z.x)), -(+a.y.x*(a.z.y*a.w.z-a.z.z*a.w.y)-a.y.y*(a.z.x*a.w.z-a.z.z*a.w.x)+a.y.z*(a.z.x*a.w.y-a.z.y*a.w.x)), +(+a.x.x*(a.z.y*a.w.z-a.z.z*a.w.y)-a.x.y*(a.z.x*a.w.z-a.z.z*a.w.x)+a.x.z*(a.z.x*a.w.y-a.z.y*a.w.x)), -(+a.x.x*(a.y.y*a.w.z-a.y.z*a.w.y)-a.x.y*(a.y.x*a.w.z-a.y.z*a.w.x)+a.x.z*(a.y.x*a.w.y-a.y.y*a.w.x)), +(+a.x.x*(a.y.y*a.z.z-a.y.z*a.z.y)-a.x.y*(a.y.x*a.z.z-a.y.z*a.z.x)+a.x.z*(a.y.x*a.z.y-a.y.y*a.z.x)))*di; }

inline bool isaffine(const mat4d& m) { return m.w.x==0 and m.w.y==0 and m.w.z==0 and m.w.w==1; }

inline mat4d translate(const vec3d& t) { return mat4d(1,0,0,t.x,  0,1,0,t.y,  0,0,1,t.z,  0,0,0,1); }
inline mat4d rotate(const double& angle, const vec3d& axis) { double c = cos(angle); double s = sin(angle); vec3d vv = normalize(axis); return mat4d(c + (1-c)*vv.x*vv.x, (1-c)*vv.x*vv.y - s*vv.z, (1-c)*vv.x*vv.z + s*vv.y, 0,  (1-c)*vv.x*vv.y + s*vv.z, c + (1-c)*vv.y*vv.y, (1-c)*vv.y*vv.z - s*vv.x, 0,  (1-c)*vv.x*vv.z - s*vv.y, (1-c)*vv.y*vv.z + s*vv.x, c + (1-c)*vv.z*vv.z, 0,  0,0,0,1); }
inline mat4d scale(const vec3d& s) { return mat4d(s.x,0,0,0,  0,s.y,0,0,  0,0,s.z,0,  0,0,0,1); }
inline mat4d frustum(double l, double r, double b, double t, double n, double f) { return mat4d(2*n/(r-l), 0, (r+l)/(r-l), 0,  0, 2*n/(t-b), (t+b)/(t-b), 0,  0, 0, -(f+n)/(f-n), -2*f*n/(f-n), 0, 0, -1, 0); }
inline mat4d ortho(double l, double r, double b, double t, double n, double f) { return mat4d(2/(r-l), 0, 0, -(r+l)/(r-l),  0, 2/(t-b), 0, -(t+b)/(t-b),  0, 0, -2/(f-n), -(f+n)/(f-n), 0, 0, 0, 1); }

inline mat4d lookat(const vec3d& eye, const vec3d& center, const vec3d& up) { vec3d w = normalize(eye - center); vec3d u = normalize(cross(up,w)); vec3d v = cross(w,u); return mat4d(u.x, u.y, u.z, -dot(u,eye),  v.x, v.y, v.z, -dot(v,eye),  w.x, w.y, w.z, -dot(w,eye),  0, 0, 0, 1); }
inline mat4d ortho2d(double l, double r, double b, double t) { return ortho(l,r,b,t,-1,1); }
inline mat4d perspective(double fovy, double aspect, double near, double far) { double f = 1/tan(fovy/2); return mat4d(f/aspect,0,0,0,   0,f,0,0,    0,0,(far+near)/(near-far),2*far*near/(near-far),    0,0,-1,0); }



inline mat4f frame_to_matrix(const frame3f& f) { return mat4f(f.x.x, f.y.x, f.z.x,f.o.x, f.x.y, f.y.y, f.z.y,f.o.y, f.x.z, f.y.z, f.z.z,f.o.z, 0,0,0,1); }
inline mat4f frame_to_matrix_inverse(const frame3f& f) { return mat4f(f.x.x, f.x.y, f.x.z, -dot(f.o,f.x), f.y.x, f.y.y, f.y.z, -dot(f.o,f.y), f.z.x, f.z.y, f.z.z, -dot(f.o,f.z), 0,0,0,1); }



#endif
