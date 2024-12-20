#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <mpi.h>
#include <time.h>
#include "../function_mandel.h"
#include <string.h>

int main (int argc, char **argv){
    int comm_size, rank;
    
    MPI_Init(&argc, &argv);
    MPI_Status status;
    MPI_Request request;
    MPI_Request request_recv;


    MPI_Comm_size(MPI_COMM_WORLD, &comm_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    
    int nb_iter, width, height; /* Degree of precision, dimensions of the image */
    double x_min, x_max, y_min, y_max; /* Bounds of representation */
    char *path; /* File destination */
    Image im;
    Image final_im;

    //Measure time taken
    struct timespec tstart, tend;

    analyzis(argc, argv, &nb_iter, &x_min, &x_max, &y_min, &y_max, &width, &height, &path);

    if (height % comm_size != 0){
        if (rank == 0){
            fprintf(stderr, "Error: the total height is not divisible by the number of processes.\n");
        }
        MPI_Finalize();
        return EXIT_FAILURE;
    }

    double local_ymin;
    //Allocate memory for one line
    initialization(&im, width, 1);

    double size_y = (y_max - y_min)/height;

    if (rank == 0){
        initialization(&final_im, width, height);
        clock_gettime(CLOCK_MONOTONIC, &tstart);
    }

    //Compute lines by each CPU
    for (int i = 0; i < height/comm_size; i++){
        local_ymin = y_min + (rank + i * comm_size) *size_y;
        //every compute is done while sending lines at the same time
        Compute(&im, nb_iter, x_min, x_max, local_ymin, local_ymin);

        //CPU directly copy the line into the final image
        if (rank==0){ 
            memcpy(final_im.pixels + (rank + i * comm_size) * width, im.pixels, width);
        } 
        //other CPU send their lines to CPU 0
        else{
            MPI_Isend(im.pixels, width, MPI_CHAR, 0, rank + i * comm_size, MPI_COMM_WORLD, &request);
            MPI_Request_free(&request); //Free the request 
        }
    }

    //receive lines
    if (rank==0){
        for (int proc = 1; proc<comm_size; proc++){
            for (int i = 0; i <height/comm_size; i++){
                MPI_Irecv(final_im.pixels + (proc + i * comm_size) * width, width, MPI_CHAR, proc, proc + i * comm_size, MPI_COMM_WORLD, &request_recv);
                
                //wait for the reception of the line
                MPI_Wait(&request_recv, &status); // Wait for the reception to complete
            }
        }

        clock_gettime(CLOCK_MONOTONIC, &tend);
        double elapsed_time = (tend.tv_sec - tstart.tv_sec) + (tend.tv_nsec - tstart.tv_nsec) / 1e9f;
        printf("Elapsed time (seconds) with MPI: %2.9lf\n", elapsed_time);

        save(&final_im, path);
        free(final_im.pixels);
    }

    free(im.pixels);
    MPI_Finalize();
    return 0;
}
