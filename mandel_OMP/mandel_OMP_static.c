#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <omp.h>
#include "../function_mandel.h"

int main(int argc,char **argv){
    int nb_iter, width, height; /* Degree of precision, dimensions of the image */
    double x_min, x_max, y_min, y_max; /* Bounds of representation */
    char * path; /* File destination */
    Image final_im;

    //get the number of processors 
    int num_procs = omp_get_num_procs();

    struct timespec tstart , tend ;

    analyzis(argc, argv, & nb_iter, & x_min, & x_max, & y_min, & y_max, & width, & height, & path);
    
    //initialize local image
    initialization (& final_im, width, height);

    clock_gettime (CLOCK_MONOTONIC , & tstart ); //measure time

    //Distribute thread in a loop
    //Master work (static scheduling) 
    #pragma omp parallel for schedule(static) shared(final_im) //share the total image to all threads
        //Distribute the total of line (height) to compute
        for (int i=0; i<(height/(num_procs*2)); i++){
            Compute (& final_im, nb_iter, x_min, x_max, y_min, y_max);
        }

    clock_gettime ( CLOCK_MONOTONIC , & tend ) ;
    double elap_time =( tend . tv_sec - tstart . tv_sec ) +( tend . tv_nsec - tstart .tv_nsec ) /1e9f ;
    printf (" Elapsed time ( seconds ) with static OpenMP: %2.9lf\n", elap_time ) ;

    save (&final_im, path);

    free(final_im.pixels);
    
    return 0 ;
}