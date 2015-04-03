#include "std_utils.h"

#include <stdlib.h>

// TODO: this is only for small strings!!!
template<typename T>
inline static string _to_string(const char* fmt, const T& value) {
    char buf[1024];
    sprintf(buf, fmt, value);
    return buf;
}

string to_string(const char* fmt, int value) { return _to_string(fmt, value); }
string to_string(const char* fmt, float value) { return _to_string(fmt, value); }
string to_string(const char* fmt, double value) { return _to_string(fmt, value); }

// insort with deletions
void sorted_del_add( vector<int> &l_c, const vector<int> &l_d, const vector<int> &l_a ) {
    vector<int> l_n;
    int s_c = l_c.size(), s_a = l_a.size(), s_d = l_d.size();
    
    l_n.reserve(s_c+s_a); // note: not all deletions may exist in current list!
    
    int i_c = 0, i_d = 0, i_a = 0;
    
    while( i_c < s_c || i_a < s_a ) {
        
        if( i_d < s_d && i_c < s_c ) {
            if( l_d[i_d] == l_c[i_c] ) {
                i_c++;
                continue;
            }
            if( l_d[i_d] < l_c[i_c] ) {
                i_d++;
                continue;
            }
        }
        
        if( i_a < s_a ) {
            if( i_c == s_c || l_a[i_a] < l_c[i_c] ) {
                l_n.push_back(l_a[i_a]);
                i_a++;
                continue;
            }
        }
        
        l_n.push_back( l_c[i_c] );
        i_c++;
    }
    
    l_c.swap( l_n );
}

void shuffle( unsigned int *inds, unsigned int size )
{
    unsigned int i1, i2 = rand(), t;
    srand( time(0) );
    for( i1 = 0; i1 < size; i1++ ) {
        i2 = ((unsigned int)(i2 + rand())) % size;
        t = inds[i1];
        inds[i1] = inds[i2];
        inds[i2] = t;
    }
}
void shuffled_indices( int size, vector<int> &l_inds )
{
    l_inds.resize(size); for( int i = 0; i < size; i++ ) l_inds[i] = i;
    srand(time(0));
    for( int i1 = 0, i2 = 0; i1 < size; i1++ ) {
        i2 = (((int)(i2+rand())) % size + size)%size;
        int t = l_inds[i1]; l_inds[i1] = l_inds[i2]; l_inds[i2] = t;
    }
}






