#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <cuda.h>

#include "function_mandel.h"

//Cuda function
__global__ void ComputeKernel(char *pixels, int nb_columns, int nb_rows, int nb_iter, double x_min, double x_max, double y_min, double y_max) {
    int idx = blockIdx.x * blockDim.x + threadIdx.x; //Thread index
    int total_pixels = nb_columns * nb_rows;

    if (idx < total_pixels) {
        int l = idx / nb_columns; 
        int c = idx % nb_columns; 

        double dx = (x_max - x_min) / nb_columns;
        double dy = (y_max - y_min) / nb_rows;

        double a = x_min + c * dx;
        double b = y_max - l * dy;
        double x = 0, y = 0;
        int i = 0;

        while (i < nb_iter) {
            double tmp = x;
            x = x * x - y * y + a;
            y = 2 * tmp * y + b;
            if (x * x + y * y > 4)
                break;
            i++;
        }
        pixels[idx] = (char)((double)i / nb_iter * 255);
    }
}

int main(int argc, char **argv) {
    int nb_iter, width, height; // Précision, dimensions de l'image
    double x_min, x_max, y_min, y_max; // Bornes de représentation
    char *path; // Destination du fichier
    Image im;

    struct timespec tstart, tend;
    clock_gettime(CLOCK_MONOTONIC, &tstart);

    analyzis(argc, argv, &nb_iter, &x_min, &x_max, &y_min, &y_max, &width, &height, &path);

    initialization(&im, width, height);

    char *d_pixels;
    size_t size = width * height * sizeof(char);
    cudaMalloc((void **)&d_pixels, size);     //Allocation of memory into the GPU
    cudaMemcpy(d_pixels, im.pixels, size, cudaMemcpyHostToDevice); //Copy of the memory into the GPU

    // Définition des dimensions de la grille et des blocs
    int total_pixels = width * height;
    int threads_per_block = 256;
    int num_blocks = (total_pixels + threads_per_block - 1) / threads_per_block;

    // Lancement du kernel CUDA
    ComputeKernel<<<num_blocks, threads_per_block>>>(d_pixels, width, height, nb_iter, x_min, x_max, y_min, y_max);

    //Copy the result into tje CPU
    cudaMemcpy(im.pixels, d_pixels, size, cudaMemcpyDeviceToHost);

    //Save final image
    save(&im, path);

    //Free memory on the GPU
    cudaFree(d_pixels);

    clock_gettime(CLOCK_MONOTONIC, &tend);
    double elap_time = (tend.tv_sec - tstart.tv_sec) + (tend.tv_nsec - tstart.tv_nsec) / 1e9f;
    printf("Elapsed time (seconds) with CUDA: %2.9lf\n", elap_time);

    return 0;
}
