#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <omp.h>
#include "../function_mandel.h"

int main(int argc,char **argv){
    int nb_iter, width, height; /* Degree of precision, dimensions of the image */
    double x_min, x_max, y_min, y_max; /* Bounds of representation */
    char * path; /* File destination */
    Image final_im;

    struct timespec tstart , tend ;

    analyzis(argc, argv, & nb_iter, & x_min, & x_max, & y_min, & y_max, & width, & height, & path);
    
    int chunk_size=1; //choose chunck_size for the schedule

    //initialize local image
    initialization (& final_im, width, height);

    clock_gettime (CLOCK_MONOTONIC , & tstart ); //measure time

    //Master work (static scheduling) 
    //We start the parallelization and distribute im to each CPU
    #pragma omp parallel
    {
        Image im; //initialize line size 1 that each CPU will compute, it will allow us to use a loop on compute also
        initialization(&im, width, 1);

        #pragma omp for schedule(static, chunk_size) //use a for loop to use static
        for (int i = 0; i < height; i++){
            double local_ymin = y_max - i * ((y_max - y_min) / height); 

            Compute(&im, nb_iter, x_min, x_max, local_ymin, local_ymin); //we compute one line so local_ymin = local_ymax

            memcpy(final_im.pixels + (i * width), im.pixels, width); //Copy the data into the final image
        }
        free(im.pixels); //free memory
    }
    
    clock_gettime ( CLOCK_MONOTONIC , & tend ) ;
    double elap_time =( tend . tv_sec - tstart . tv_sec ) +( tend . tv_nsec - tstart .tv_nsec ) /1e9f ;
    printf (" Elapsed time ( seconds ) with static OpenMP: %2.9lf\n", elap_time ) ;

    save (&final_im, path);

    free(final_im.pixels);
    
    return 0 ;
}