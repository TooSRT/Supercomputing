#ifndef FUNCTION_MANDEL_H
#define FUNCTION_MANDEL_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

typedef struct {
    int nb_rows, nb_columns; 
    char * pixels;           
} Image;

void analyzis (int argc, char * * argv, int * nb_iter, double * x_min, double * x_max, double * y_min, double * y_max, int * width, int * height, char * * path);
void initialization (Image * im, int nb_columns, int nb_rows);
void save (const Image * im, const char * path);
void Compute (Image * im, int nb_iter, double x_min, double x_max, double y_min, double y_max);

#endif 

