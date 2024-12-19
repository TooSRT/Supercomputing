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
if (rank == 0) {
    Image final_im;
    initialization(&final_im, width, height);
    // Calcul local pour les lignes assignées au processus 0
    for (int i = 0; i < (height / comm_size); i++) {
        local_ymin = y_min + (i * comm_size) * (y_max - y_min) / height;

        // Calcul de la ligne
        Compute(&im, nb_iter, x_min, x_max, local_ymin, local_ymin);

        // Placement correct dans l'image finale
        for (int j = 0; j < width; j++) {
            final_im.pixels[i * comm_size * width + j] = im.pixels[j];
        }
    }

    // Réception des lignes des autres processus
    MPI_Request recv_requests[height - height / comm_size];
    MPI_Status recv_status[height - height / comm_size];
    for (int i = 0; i < (height - height / comm_size); i++) {
        MPI_Irecv(final_im.pixels, width, MPI_CHAR, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &recv_requests[i]);
    }

    // Synchronisation des réceptions et placement des lignes
    MPI_Waitall(height - height / comm_size, recv_requests, recv_status);
    for (int i = 0; i < (height - height / comm_size); i++) {
        int line_tag = recv_status[i].MPI_TAG; // Ligne reçue
        for (int j = 0; j < width; j++) {
            final_im.pixels[line_tag * width + j] = im.pixels[j];
        }
    }

    clock_gettime(CLOCK_MONOTONIC, &tend);
    double elapsed_time = (tend.tv_sec - tstart.tv_sec) + (tend.tv_nsec - tstart.tv_nsec) / 1e9;
    printf("Elapsed time (seconds) with MPI: %2.9lf\n", elapsed_time);

    // Sauvegarder l'image finale et libérer la mémoire
    save(&final_im, path);
    free(final_im.pixels);
} else {
    // Calcul et envoi pour les autres processus
    MPI_Request send_requests[height / comm_size];
    for (int i = 0; i < (height / comm_size); i++) {
        local_ymin = y_min + (rank + i * comm_size) * (y_max - y_min) / height;

        // Calcul de la ligne
        Compute(&im, nb_iter, x_min, x_max, local_ymin, local_ymin);

        // Envoi de la ligne avec le tag correct
        MPI_Isend(im.pixels, width, MPI_CHAR, 0, rank + i * comm_size, MPI_COMM_WORLD, &send_requests[i]);
    }

    // Synchronisation des envois
    MPI_Waitall(height / comm_size, send_requests, MPI_STATUSES_IGNORE);
}


    
    free(im.pixels); //free the memory every time we send a line

    MPI_Finalize();

    return 0 ;
}