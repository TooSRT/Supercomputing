#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <cuda.h>
#include <cuda_runtime.h>

/* Bounds of the Mandelbrot set */
#define X_MIN -1.78
#define X_MAX 0.78
#define Y_MIN -0.96
#define Y_MAX 0.96

typedef struct {
    int nb_rows, nb_columns; /* Dimensions */
    char * pixels; /* Linearized matrix of pixels */
} Image;

void error_options () {
    fprintf (stderr, "Use : ./mandel [options]\n\n");
    fprintf (stderr, "Options \t Meaning \t\t Default val.\n\n");
    fprintf (stderr, "-n \t\t Nb iter. \t\t 100\n");
    fprintf (stderr, "-b \t\t Bounds \t\t -1.78 0.78 -0.96 0.96\n");
    fprintf (stderr, "-d \t\t Dimensions \t\t 1024 768\n");
    fprintf (stderr, "-f \t\t File \t\t /tmp/mandel.ppm\n");
    exit (1);
}

void analyzis(int argc, char **argv, int *nb_iter, double *x_min, double *x_max, double *y_min, double *y_max, int *width, int *height, const char **path) {
    const char *opt = "b:d:n:f:";
    int c;

    /* Default values */
    *nb_iter = 200000;
    *x_min = X_MIN;
    *x_max = X_MAX;
    *y_min = Y_MIN;
    *y_max = Y_MAX;
    *width = 1024;
    *height = 768;
    *path = "mandel_cuda.ppm";  // Assign directly to a const char*

    /* Analysis of arguments */
    while ((c = getopt(argc, argv, opt)) != EOF) {
        switch (c) {
            case 'b':
                sscanf(optarg, "%lf", x_min);
                sscanf(argv[optind++], "%lf", x_max);
                sscanf(argv[optind++], "%lf", y_min);
                sscanf(argv[optind++], "%lf", y_max);
                break;
            case 'd': /* width */
                sscanf(optarg, "%d", width);
                sscanf(argv[optind++], "%d", height);
                break;
            case 'n': /* Number of iterations */
                *nb_iter = atoi(optarg);
                break;
            case 'f': /* Output file */
                *path = optarg;
                break;
            default:
                error_options();
        }
    }
}

void initialization (Image * im, int nb_columns, int nb_rows) {
    im -> nb_rows = nb_rows;
    im -> nb_columns = nb_columns;
    im -> pixels = (char *) malloc (sizeof (char) * nb_rows * nb_columns); /* Space memory allocation */
}

void save (const Image * im, const char * path) {
    /* Image saving using the ASCII format'.PPM' */
    unsigned i;
    FILE * f = fopen (path, "w");
    fprintf (f, "P6\n%d %d\n255\n", im -> nb_columns, im -> nb_rows);
    for (i = 0; i < im -> nb_columns * im -> nb_rows; i ++) {
        char c = im -> pixels [i];
        fprintf (f, "%c%c%c", c, c, c); /* Monochrome weight */
    }
    fclose (f);
}


//Cuda function
__global__ void Compute(char *pixels, int nb_columns, int nb_rows, int nb_iter, double x_min, double x_max, double y_min, double y_max){
    int pos = blockIdx.x * blockDim.x + threadIdx.x; //index of a thread in our grid 

    if (pos >= nb_rows * nb_columns) return; // check that we are in the grid

    //INdex of our lines and columns
    int l = pos/nb_columns; 
    int c = pos%nb_columns;  

    //In Cuda we don't need loop anymore, every thread is associated to the computation of a pixel
    /* Discretization */
    double dx = (x_max - x_min) / nb_columns;
    double dy = (y_max - y_min) / nb_rows;

    /* Computation at each point of the image */
    double a = x_min + c * dx;
    double b = y_max - l * dy;
    double x = 0, y = 0;
    int i = 0;

    while (i < nb_iter) {
        double tmp = x;
        x = x * x - y * y + a;
        y = 2 * tmp * y + b;
        if (x * x + y * y > 4){ /* Divergence ! */
            break;
        } else {
            i++;
        }
    }
    pixels[pos] = (char)((i * 255)/nb_iter);
}

int main(int argc, char **argv) {
    int nb_iter, width, height;
    double x_min, x_max, y_min, y_max;
    const char *path;  // Declare as const char*
    Image im;

    struct timespec tstart, tend, tstart_copy, tend_copy, tstart_kernel, tend_kernel;
    clock_gettime(CLOCK_MONOTONIC, &tstart);

    analyzis(argc, argv, &nb_iter, &x_min, &x_max, &y_min, &y_max, &width, &height, &path);

    initialization(&im, width, height);

    char *d_pixels;
    size_t size = width * height * sizeof(char); //total size of the image we have with*height pixels and each of them of size(char)

    //------Time for memory allocation and copying to the GPU------
    clock_gettime(CLOCK_MONOTONIC, &tstart_copy);

    cudaMalloc((void **)&d_pixels, size);     //Allocation of memory into the GPU to sotre pixels
    cudaMemcpy(d_pixels, im.pixels, size, cudaMemcpyHostToDevice); //Copy of the memory into the GPU

    clock_gettime(CLOCK_MONOTONIC, &tend_copy);

    //-------Measure time taken for memory copy-------
    double elap_copy_time = (tend_copy.tv_sec - tstart_copy.tv_sec) + (tend_copy.tv_nsec - tstart_copy.tv_nsec)/1e9f;
    printf("Time taken for memory copy to GPU: %2.9lf seconds\n", elap_copy_time);

    //Define block and grid
    int total_pixels = width * height; //total pixels of the image
    int threads_per_block = 256; // we choose 256 threads per block (arbitrary)
    int num_blocks = (total_pixels + threads_per_block - 1)/threads_per_block; //total block needed to cover all the pixels

    //Measure time taken for GPU computation
    //------------------------------------------------
    clock_gettime(CLOCK_MONOTONIC, &tstart_kernel);

    //Perform CUDA computation
    Compute<<<num_blocks, threads_per_block>>>(d_pixels, width, height, nb_iter, x_min, x_max, y_min, y_max);

    clock_gettime(CLOCK_MONOTONIC, &tend_kernel); 
    //-------------------------------------------------

    //------Measure time taken for GPU computation------
    double elap_kernel_time = (tend_kernel.tv_sec - tstart_kernel.tv_sec) + (tend_kernel.tv_nsec - tstart_kernel.tv_nsec)/1e9f;
    printf("Time taken for GPU computation: %2.9lf seconds\n", elap_kernel_time);

    //Copy the result to the CPU
    cudaMemcpy(im.pixels, d_pixels, size, cudaMemcpyDeviceToHost);

    //Save the final image
    save(&im, path);

    // Free memory on the GPU
    cudaFree(d_pixels);

    //--------Measure total time taken--------
    clock_gettime(CLOCK_MONOTONIC, &tend);
    double elap_time = (tend.tv_sec - tstart.tv_sec) + (tend.tv_nsec - tstart.tv_nsec)/1e9f;
    printf("Total elapsed time: %2.9lf seconds\n", elap_time);

    return 0;
}
