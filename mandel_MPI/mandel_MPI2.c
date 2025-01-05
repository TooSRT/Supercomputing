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
    initialization(&im, width, 1); //we compute lines so size 1
    double size_y = (y_max - y_min) / height; //Size of each interval to compute for each processors

    //CPU 0 compute lines and put them together
    if (rank==0){
        //Initialize final image for CPU 0
        Image final_im;
        initialization(&final_im, width, height);
        
        clock_gettime (CLOCK_MONOTONIC , & tstart ) ;//measure computation time

        //line computed by CPU0
        //don't need loop for CPU 0,just compute the block in one time so we can copy directly the memory into final_im
        for (int i=0; i < (height/comm_size); i++){ //loop that iterate on every line of our image
            local_ymin = y_min + ( i * comm_size) * size_y; //k+i*n method for alterned line computation
            
            //Compute the line into the image line buffer
            Compute(&im, nb_iter, x_min, x_max, local_ymin, local_ymin); //local_ymax = local_ymin because line are size 1 in height

            memcpy(final_im.pixels + (i * comm_size * width), im.pixels, width); //copy the pixel in the final image
        }
                
        //receive the number of line from other CPU (total_lines_received= height - height/comm_size)
        for (int proc=1; proc<comm_size; proc++){ //each processor send their line, we use it as a tag also
            for (int i=0; i<(height/comm_size); i++){ 
                //use a tag associated to each line, data received are sent into the final_im
                MPI_Recv(final_im.pixels + (i * comm_size + proc) * width, width, MPI_CHAR, proc, i, MPI_COMM_WORLD, &status);
            }
        }
        clock_gettime(CLOCK_MONOTONIC, &tend);
        double elapsed_time = (tend.tv_sec - tstart.tv_sec) + (tend.tv_nsec - tstart.tv_nsec) / 1e9;
        printf("Elapsed time (seconds) with MPI: %2.9lf\n", elapsed_time);
        
        //Save the final image and free the memory associated
        save(&final_im, path);
        free(final_im.pixels);
    }
    
    //other CPU computes lines and send them to CPU 0 
    else{
        for (int i=0; i < (height/comm_size); i++){ //loop that iterate on every line of our image
            local_ymin = y_min + (rank + i * comm_size) * size_y;
            
            //we compute only one line so local_ymax=local_ymin
            Compute(&im, nb_iter, x_min, x_max, local_ymin, local_ymin);

            //Send computed line direclty, use the line as a tag
            MPI_Send(im.pixels, width, MPI_CHAR, 0, i, MPI_COMM_WORLD);
        }
    }
    
    free(im.pixels); //free the memory every of im

    MPI_Finalize();

    return 0 ;
}