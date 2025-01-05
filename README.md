## Supercomputing Project M2
Project of supercomputing, the goal is to use differents parallelization methods (OpenMP, MPI, CUDA) and compare their efficacity.

## Usage 
The program contain different mandel directory corresponding to the  differents parallelization methods used.
It also have a results directory with all the data collected during my personnal test.

A makefile is available

Compile all the files with 
```bash
make
```

Or compile and execute all the programs with
```bash
make run
```

Or run and execute a specific program with
```bash
make run_<name_of_the_file>
```
i.e:
mandel
mandel_mpi
mandel_omp_static
....

Clean all the files (.ppm, compiled files)
```bash
make clean
```

You can directly modify the properties of the final image in function_mandel.c

## Author

-Théo Servotte