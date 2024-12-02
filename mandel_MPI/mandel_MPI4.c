#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <mpi.h>
#include <time.h>
#include "../function_mandel.h"
#include <string.h>

int main (int argc, char * * argv){
    int comm_size, rank;
    
    MPI_Init(&argc, &argv);
    MPI_Status status;

    MPI_Comm_size(MPI_COMM_WORLD,&comm_size);
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);
    
    int nb_iter, width, height; /* Degree of precision, dimensions of the image */
    double x_min, x_max, y_min, y_max; /* Bounds of representation */
    char * path; /* File destination */
    Image im, packed_im;

    //mesure time taken
    struct timespec tstart , tend ;

    analyzis(argc, argv, & nb_iter, & x_min, & x_max, & y_min, & y_max, & width, & height, & path);

    //Check that total size is divisible by the number of CPU 
    if (height % comm_size != 0){
        if (rank==0){
        fprintf(stderr, "Error: the total height is not divisible by the number of CPU (comm_size).\n");
        }
        MPI_Finalize();
        return EXIT_FAILURE;
    }

    double local_ymin;
    double local_ymax;
    double size_y = (y_max-y_min)/height; //portion of lines computed by each line

    initialization(&im, width, 1); //we compute lines so size 1
    initialization(&packed_im, width, height-height/comm_size); //pack of image that will be send to CPU 0

    //CPU 0 compute lines and put them together
    if (rank==0){
        // Initialize final image for CPU 0
        Image final_im;
        initialization(&final_im, width, height);

        clock_gettime (CLOCK_MONOTONIC , & tstart);  //measure time

        for (int i=0; i < (height/comm_size); i++){ //loop that iterate on every line of our image
            local_ymin = y_min + (i * comm_size) * size_y; //rank == 0
            local_ymax = local_ymin;

            Compute(&im,nb_iter, x_min, x_max, local_ymin, local_ymax);
            memcpy(final_im.pixels + (rank+ i * comm_size) * width, im.pixels, width); //store the data directly into final_image 
        }
        MPI_Recv(packed_im.pixels, (height-height/comm_size), MPI_PACKED, 1, 1, MPI_COMM_WORLD, &status); //receive the packed data computed by every other CPU than 0
        int position = 0; 

        MPI_Unpack(packed_im.pixels, width*(height-height/comm_size), &position, final_im.pixels, (height - height / comm_size), MPI_CHAR, MPI_COMM_WORLD); //unpack the data

        //measure time taken
        clock_gettime(CLOCK_MONOTONIC, &tend);
        double elapsed_time = (tend.tv_sec - tstart.tv_sec) + (tend.tv_nsec - tstart.tv_nsec) / 1e9f;
        printf("Elapsed time (seconds) with MPI: %2.9lf\n", elapsed_time);
        
        save(&final_im,path); //save the final image
        free(final_im.pixels); //free final_im memory
    }
    else{
        int position = 0;

        for (int i=0; i<(height/comm_size); i++){
            local_ymin = y_min + (rank + i * comm_size) * size_y; //k+i*n
            local_ymax = local_ymin;

            Compute(&im, nb_iter, x_min, x_max, local_ymin, local_ymax); //compute lines for all CPU different from 0
            MPI_Pack(im.pixels, width, MPI_CHAR, packed_im.pixels, width * (height - height / comm_size), &position, MPI_COMM_WORLD); //Pack all the computed lines
        }

        //Only CPU 0 send tha packed lines
        if (rank==1){
            MPI_Send(packed_im.pixels, (height-height/comm_size), MPI_PACKED, 0, 1, MPI_COMM_WORLD); //send the packed data that have (height - height/comm_size) lines
        }
        free(packed_im.pixels); //free the memory for packed lines
    }
    free(im.pixels); //free the memory every time we send a line

    MPI_Finalize();

    return 0 ;
}