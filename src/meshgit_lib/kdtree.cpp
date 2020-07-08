//#include <fstream.h>
//#include <iostream.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include "kdtree.h"
#include "quicksort.h"


void shuffled_indices_local( int size, vector<int> &l_inds )
{
    l_inds.resize(size); for( int i = 0; i < size; i++ ) l_inds[i] = i;
    srand(time(0));
    for( int i1 = 0, i2 = 0; i1 < size; i1++ ) {
        i2 = (((int)(i2+rand())) % size + size)%size;
        int t = l_inds[i1]; l_inds[i1] = l_inds[i2]; l_inds[i2] = t;
    }
}



void MGKDTree::insert( vec3f &v, int i )
{
    if( !top ) top = new MGKDBranch( v, i, 0 );
    else top->insert( v, i );
}
void MGKDTree::insert_shuffle( vector<vec3f> &l_v )
{
    vector<int> inds; shuffled_indices_local((int)l_v.size(),inds);
    for( int i : inds ) insert(l_v[i],i);
}
void MGKDBranch::insert( vec3f &v, int i )
{
    float dist_partition = this->v[d] - v[d];
    
    if( dist_partition >= 0 ) {
        if( l == 0 ) l = new MGKDBranch( v, i, (d+1)%3 );
        else l->insert( v, i );
    } else {
        if( r == 0 ) r = new MGKDBranch( v, i, (d+1)%3 );
        else r->insert( v, i );
    }
}




int MGKDTree::knn( vec3f &v, int k, vector<int> &li_knn, vector<float> &ld2_knn )
{
    li_knn.assign( k, -1 ); ld2_knn.assign( k, 0.0f );
    
    if( !top ) return 0;
    
    int found = 0; float max2 = INFINITY;
    top->knn( v, k, li_knn, ld2_knn, found, max2 );
    return found;
}

//#define DOT( x, y )   ( (x[0]-y[0])*(x[0]-y[0]) + (x[1]-y[1])*(x[1]-y[1]) + (x[2]-y[2])*(x[2]-y[2]) )
#define DIFFDOT(a,b)    ( (a.x-b.x)*(a.x-b.x) + (a.y-b.y)*(a.y-b.y) + (a.z-b.z)*(a.z-b.z) )
void MGKDBranch::knn( vec3f &v, int k, vector<int> &li_knn, vector<float> &ld2_knn, int &found, float &max2 )
{
    float dist_partition = this->v[d] - v[d];
    float dist_partition2 = dist_partition * dist_partition;
    float dist2 = DIFFDOT( this->v, v );
    
    if( found < k || dist2 <= max2 ) {
        int imax;
        
        if( found == k ) {
            // find and replace max distance
            imax = 0;
            float vmax2 = ld2_knn[0];
            for( int i = 1; i < found; i++ ) if( ld2_knn[i] >= vmax2 ) { imax = i; vmax2 = ld2_knn[i]; }
        } else {
            // insert at end
            imax = found;
            found++;
        }
        
        li_knn[imax] = this->i;
        ld2_knn[imax] = dist2;
        
        // find new max
        max2 = dist2;
        for( int i = 0; i < found; i++ ) max2 = max( max2, ld2_knn[i] );
        
    }
    
    if( dist_partition == 0 ) {
        if( l != 0 ) l->knn( v, k, li_knn, ld2_knn, found, max2 );
        if( r != 0 ) r->knn( v, k, li_knn, ld2_knn, found, max2 );
    } else if( dist_partition > 0 ) {
        if( l != 0 ) l->knn( v, k, li_knn, ld2_knn, found, max2 );
        if( ( found < k || dist_partition2 <= max2 ) && r != 0 ) r->knn( v, k, li_knn, ld2_knn, found, max2 );
    } else {
        if( r != 0 ) r->knn( v, k, li_knn, ld2_knn, found, max2 );
        if( ( found < k || dist_partition2 <= max2 ) && l != 0 ) l->knn( v, k, li_knn, ld2_knn, found, max2 );
    }
    
}



