#ifndef KDTREE
#define KDTREE

#include "../vec.h"
#include <vector>

using namespace std;

class MGKDBranch {
    
public:
    
    MGKDBranch( vec3f &v, int i, int d ) : v(v), i(i), d(d) { }
    
    void insert( vec3f &v, int i );
    void knn( vec3f &v, int k, vector<int> &li_knn, vector<float> &ld2_knn, int &found, float &max2 );
    
    vec3f v;
    int i;
    int d;
    
    MGKDBranch *l = nullptr;
    MGKDBranch *r = nullptr;
    
};


class MGKDTree {

public:
    
    MGKDTree() { }
    
    void insert( vec3f &v, int i );
    void insert_shuffle( vector<vec3f> &l_v );
    
    int knn( vec3f &v, int k, vector<int> &li_knn, vector<float> &ld2_knn );
    inline int knn( vec3f &v, int k, vector<int> &li_knn ) {
        vector<float> ld2_knn; return knn( v, k, li_knn, ld2_knn );
    }
    
private:
    
    MGKDBranch *top = nullptr;
    
};


#endif

