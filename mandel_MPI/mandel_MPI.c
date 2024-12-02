#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <mpi.h>
#include <time.h>
#include <math.h>
#include "../function_mandel.h"

// Parallelization using only MPI_Gather

int main (int argc, char * * argv){
    int comm_size, rank;
    
    MPI_Init(&argc, &argv);

    MPI_Comm_size(MPI_COMM_WORLD,&comm_size);
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);
    
    int nb_iter, width, height; /* Degree of precision, dimensions of the image */
    double x_min, x_max, y_min, y_max; /* Bounds of representation */
    char * path; /* File destination */
    Image im, final_im; 

    //mesure time taken

    analyzis(argc, argv, & nb_iter, & x_min, & x_max, & y_min, & y_max, & width, & height, & path);

    //Check that total size is divisible by the number of CPU 
    if (height % comm_size != 0){
        if (rank==0){
        fprintf(stderr, "Error: the total height is not divisible by the number of CPU (comm_size).\n");
        }
        MPI_Finalize();
        return EXIT_FAILURE;
    }
    struct timespec tstart , tend ;
    clock_gettime (CLOCK_MONOTONIC , & tstart ) ;
    int loc_height = height/comm_size; //height of each local image proportional to the number of CPU

    //use Bcast to send info to each proc instead ????
    //use a scatter to spread the info and don't use two im ?
    //use a gatherV ??
    //use MPI Barier to synchronize everybody then MPI finalysze to debug (we see if we got an error at this point in the code)

    //Give task to each processors
    //Initialize each im struct associated to it's CPU
    initialization (& im, width, loc_height);

    //Size of each interval to compute for each processors
    double size_y = (y_max - y_min)/comm_size; 

    //y coordinates for each CPU 
    //Problem with the local_ymin/ymax we need to invert them in compute in order to work
    double local_ymin = y_min + (rank) * (size_y);
    double local_ymax = local_ymin + size_y;

    Compute (&im, nb_iter, x_min, x_max, local_ymax, local_ymin); //Compute the part of the image associated

    //printf("Processus %d: process lines [%d, %d] with y_min = %f and y_max = %f\n", rank, rank*loc_height, (rank + 1)*loc_height - 1, local_ymin, local_ymax);

    if (rank==0){
        initialization(&final_im,width,height);
    }
    //collect all lines from other CPU with gather and send them in final_im
    MPI_Gather(im.pixels, width * loc_height, MPI_CHAR, final_im.pixels, width * loc_height, MPI_CHAR, 0, MPI_COMM_WORLD);

    if (rank==0){
        //Measure execution time
        clock_gettime(CLOCK_MONOTONIC, &tend);
        double elapsed_time = (tend.tv_sec - tstart.tv_sec) + (tend.tv_nsec - tstart.tv_nsec) / 1e9f;
        printf("Elapsed time (seconds) with MPI: %2.9lf\n", elapsed_time);
        
        save(&final_im, path); //save final image on CPU 0

        free(final_im.pixels); //Free the memory
    }
    free(im.pixels);

    MPI_Finalize();

    return 0 ;
}
