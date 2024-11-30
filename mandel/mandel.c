#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include "../function_mandel.h"

int main (int argc, char * * argv) {
    int nb_iter, width, height; /* Degree of precision, dimensions of the image */
    double x_min, x_max, y_min, y_max; /* Bounds of representation */
    char * path; /* File destination */
    Image im;

    struct timespec tstart , tend ;
    clock_gettime (CLOCK_MONOTONIC , & tstart ) ;

    analyzis(argc, argv, & nb_iter, & x_min, & x_max, & y_min, & y_max, & width, & height, & path);
    initialization (& im, width, height);
    Compute (& im, nb_iter, x_min, x_max, y_min, y_max);
    save (& im, path);

    clock_gettime ( CLOCK_MONOTONIC , & tend ) ;
    double elap_time =( tend . tv_sec - tstart . tv_sec ) +( tend . tv_nsec - tstart .tv_nsec ) /1e9f ;
    printf (" Elapsed time ( seconds ) with no para: %2.9lf\n", elap_time ) ;

    return 0 ;
}
