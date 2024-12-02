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
    MPI_Request request;

    MPI_Comm_size(MPI_COMM_WORLD,&comm_size);
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);
    
    int nb_iter, width, height; /* Degree of precision, dimensions of the image */
    double x_min, x_max, y_min, y_max; /* Bounds of representation */
    char * path; /* File destination */
    Image im; 

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
    initialization(&im, width, 1); //we compute lines so size 1

    clock_gettime (CLOCK_MONOTONIC , & tstart ) ;

    //CPU 0 compute lines and put them together
    if (rank==0){
        // Initialize final image for CPU 0
        Image final_im;
        initialization(&final_im, width, height);

        for (int i=0; i<(height - height/comm_size); i++){ //because CPU0 already compute height/comm_size line
            //use a tag associated to each line 
            MPI_Irecv(im.pixels, width, MPI_CHAR, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &request);
        }   
        
        for (int i=0; i < (height/comm_size); i++){ //loop that iterate on every line of our image
            local_ymin = y_min + (rank + i * comm_size) * (y_max - y_min) / height; //k+i*n method for alterned line computation
            local_ymax = local_ymin; //we compute only one line so y_min=y_max
            
            //Compute the line associated to CPU 0
            Compute (&im, nb_iter, x_min, x_max, local_ymin, local_ymax); //Compute the part of the image associated
            memcpy(final_im.pixels + (rank + i * comm_size) * width, im.pixels, width);
        }
        //receive the number of line from other CPU (height - height/comm_size)

            //do something else
            
            
        MPI_Wait(&request, &status); //Wait for the message to complete

        int line_index = status.MPI_TAG; //Line index received via tag
        memcpy(final_im.pixels + line_index * width, im.pixels, width); 

        clock_gettime(CLOCK_MONOTONIC, &tend);
        double elapsed_time = (tend.tv_sec - tstart.tv_sec) + (tend.tv_nsec - tstart.tv_nsec) / 1e9;
        printf("Elapsed time (seconds) with MPI: %2.9lf\n", elapsed_time);
        
        //Save the final image
        save(&final_im, path);
        free(final_im.pixels);
    }
    
    //other CPU computes lines and send them to CPU 0 
    else{
        for (int i=0; i < (height/comm_size); i++){ //loop that iterate on every line of our image
            local_ymin = y_min + (rank + i * comm_size) * (y_max - y_min) / height;
            local_ymax = local_ymin; //we compute only one line so local_ymax=local_ymin

            Compute(&im, nb_iter, x_min, x_max, local_ymin, local_ymax);

            //use the line number as a tag
            MPI_Isend(im.pixels, width, MPI_CHAR, 0, rank + i*comm_size, MPI_COMM_WORLD, &request);      
            
            //do something else

            //
            MPI_Wait(&request, &status); // Wait for the message to complete

        }
    }
    
    free(im.pixels); //free the memory every time we send a line

    MPI_Finalize();

    return 0 ;
}