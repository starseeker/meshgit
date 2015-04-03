#ifndef _STD_H_
#define _STD_H_

#include <string>
#include <vector>
#include <map>
#include <set>
#include <unordered_set>
//#include <tuple>
#include <memory>
//#include <array>
#include <chrono>
#include <functional>
#include <sys/stat.h>

using namespace std;

using std::string;
using std::vector;
using std::map;
using std::pair;
using std::make_pair;
using std::swap;
using std::array;
using std::ref;
using std::cref;
//using std::tuple;
//using std::make_tuple;
using std::function;
using namespace std::chrono;
using std::to_string;


inline bool fileexists( const char *fn ) {
    FILE *fp = fopen( fn, "rb" );
    if( fp == 0 ) return false;
    fclose(fp);
    return true;
    
    struct stat buffer;
    if( stat( fn, &buffer ) ) return true;
    return false;
}
inline bool fileexists( const string &s ) { return fileexists( s.c_str() ); }


template<typename T>
void reorder(vector<T> &l, vector<int> &inds) {
    vector<T> l_; l_.reserve(inds.size());
    for( int i : inds ) l_.push_back(l[i]);
    swap( l_, l );
}



struct range_iter {
    int i;
    range_iter(int i) : i(i) { }
    bool operator==(const range_iter& r) { return r.i == i; }
    // operator int()() { return i; }
};

struct range {
    int min = 0;
    int max = 0;
    
    range(int max) : max(max) { }
    range(int min, int max) : min(min), max(max) { }
    
    range_iter begin() const { return min; }
    range_iter end() const { return max; }
};

#endif
