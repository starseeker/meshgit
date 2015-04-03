#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>

#include "quicksort.h"


#define NPARTS          4

float *comp_vals;

int compare( const void *a, const void *b );
void quicksort_inds_nostd( float *vals, unsigned int *inds, unsigned int left, unsigned int right );


void MGshuffle( unsigned int *inds, unsigned int size )
{
    unsigned int i1, i2, t;
    srand( time(0) );
    for( i1 = 0; i1 < size; i1++ ) {
        i2 = ((unsigned int)(i2 + rand())) % size;
        t = inds[i1];
        inds[i1] = inds[i2];
        inds[i2] = t;
    }
}
unsigned int *shuffled_inds( unsigned int size )
{
    unsigned int *inds = (unsigned int*)malloc( size*sizeof(unsigned int) );
    if( inds == 0 ) {
        printf( "could not allocate %u bytes", (unsigned int)(size*sizeof(unsigned int)) );
        exit( 1 );
    }
    for( unsigned int i = 0; i < size; i++ ) inds[i] = i;
    MGshuffle( inds, size );
    return inds;
}

void quicksort_inds( float *vals, unsigned int *inds, unsigned int size )
{
    quicksort_inds_std( vals, inds, size );
    //quicksort_inds_nostd( vals, inds, size );
    //quicksort_inds_parallel( vals, inds, size );
}



void quicksort_inds_std( float *vals, unsigned int *inds, unsigned int size )
{
    comp_vals = vals;
    qsort( inds, size, sizeof(int), compare );
}
int compare( const void *a, const void *b ) { return ( comp_vals[*(unsigned int*)a] - comp_vals[*(unsigned int*)b] ); }


void quicksort_inds_nostd( float *vals, unsigned int *inds, unsigned int size )
{
    quicksort_inds_nostd( vals, inds, 0, size-1 );
}
// modified from: http://www.algolist.net/Algorithms/Sorting/Quicksort
void quicksort_inds_nostd( float *vals, unsigned int *inds, unsigned int left, unsigned int right )
{
    int i = left, j = right;
    int tmp;
    int pivot = inds[(left + right) / 2];
    float pivot_val = vals[pivot];
    
    /* partition */
    while( i <= j )
    {
        while( vals[inds[i]] < pivot_val ) i++;
        while( vals[inds[j]] > pivot_val ) j--;
        if (i <= j)
        {
            tmp = inds[i];
            inds[i] = inds[j];
            inds[j] = tmp;
            i++;
            j--;
        }
    };
    
    /* recursion */
    if( left < j )  quicksort_inds_nostd( vals, inds, left, j );
    if( i < right ) quicksort_inds_nostd( vals, inds, i, right );
}

void quicksort_inds_parallel( float *vals, unsigned int *inds, unsigned int size )
{
    int pi[NPARTS], pe[NPARTS];
    int skip = (int)ceil( (float)size / NPARTS );
    
    unsigned int *sinds = (unsigned int*) malloc( size*sizeof(unsigned int) );
    memcpy( sinds, inds, size*sizeof(int) );
    
    //printf( "size: %d, skip: %d\n", size, skip );
    #pragma omp parallel for default(none) \
        shared(vals,sinds,size,skip,pi,pe)
    for( int i = 0; i < NPARTS; i++ )
    {
        int st = skip*i;
        int en = st+skip < size ? st+skip : size;
        unsigned int sz = en - st;
        //printf( "%d: [%07d,%07d] = %07d\n", i, st, en-1, sz );
        
        pi[i] = st;
        pe[i] = en;
        quicksort_inds_nostd( vals, &sinds[st], sz );       // NOTE: DO *NOT* USE qsort()!
    }
    
    for( int i = 0; i < size; i++ )
    {
        int ip_m = -1;
        int ipi_m = -1;
        float v_m = 0.0f;
        
        for( int ip = 0; ip < NPARTS; ip++ )
        {
            if( pi[ip] == pe[ip] ) continue;
            
            int ipi = sinds[pi[ip]];
            float ipv = vals[ipi];
            
            if( ip_m == -1 || ipv < v_m ) {
                ip_m  = ip;
                ipi_m = ipi;
                v_m   = ipv;
            }
        }
        
        inds[i] = ipi_m;
        pi[ip_m]++;
    }
    
    free( sinds );
    
}



// look at: http://broadcast.oreilly.com/2009/06/may-column-multithreaded-algor.html
// look at: http://fhtr.blogspot.com/2009/01/multi-threaded-qsort-in-c.html
//    code: https://gist.github.com/45095






