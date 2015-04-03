#ifndef quicksort
#define quicksort

void MGshuffle( unsigned int *inds, unsigned int size );
unsigned int *shuffled_inds( unsigned int size );

void quicksort_inds( float *vals, unsigned int *inds, unsigned int size );
void quicksort_inds_std( float *vals, unsigned int *inds, unsigned int size );
void quicksort_inds_nostd( float *vals, unsigned int *inds, unsigned int size );
void quicksort_inds_parallel( float *vals, unsigned int *inds, unsigned int size );

#endif
