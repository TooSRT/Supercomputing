CC = mpicc
CFLAGS = -Wall -I.  
LDFLAGS =           

MANDEL_DIR = mandel
MANDEL_MPI_DIR = mandel_MPI
MANDEL_OMP_DIR = mandel_OMP

COMMON_SRC = function_mandel.c

MANDEL_SRC = $(MANDEL_DIR)/mandel.c
MANDEL_MPI_SRC = $(MANDEL_MPI_DIR)/mandel_MPI.c
MANDEL_MPI2_SRC = $(MANDEL_MPI_DIR)/mandel_MPI2.c
MANDEL_OMP_STATIC_SRC = $(MANDEL_OMP_DIR)/mandel_OMP_static.c
MANDEL_OMP_DYNAMIC_SRC = $(MANDEL_OMP_DIR)/mandel_OMP_dynamic.c

MANDEL_OBJ = $(COMMON_SRC:.c=.o) $(MANDEL_SRC:.c=.o)
MANDEL_MPI_OBJ = $(COMMON_SRC:.c=.o) $(MANDEL_MPI_SRC:.c=.o)
MANDEL_MPI2_OBJ = $(COMMON_SRC:.c=.o) $(MANDEL_MPI2_SRC:.c=.o)

MANDEL_EXEC = mandel.out
MANDEL_MPI_EXEC = mandel_MPI.out
MANDEL_MPI2_EXEC = mandel_MPI2.out
MANDEL_OMP_STATIC_EXEC = mandel_OMP_static.out
MANDEL_OMP_DYNAMIC_EXEC = mandel_OMP_dynamic.out

all: $(MANDEL_EXEC) $(MANDEL_MPI_EXEC) $(MANDEL_MPI2_EXEC) $(MANDEL_OMP_STATIC_EXEC) $(MANDEL_OMP_DYNAMIC_EXEC)

$(MANDEL_EXEC): $(MANDEL_OBJ)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

$(MANDEL_MPI_EXEC): $(MANDEL_MPI_OBJ)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

$(MANDEL_MPI2_EXEC): $(MANDEL_MPI2_OBJ)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

$(MANDEL_OMP_STATIC_EXEC): $(MANDEL_OMP_STATIC_SRC) $(COMMON_SRC)
	gcc -fopenmp $(CFLAGS) -o $@ $^

$(MANDEL_OMP_DYNAMIC_EXEC): $(MANDEL_OMP_DYNAMIC_SRC) $(COMMON_SRC)
	gcc -fopenmp $(CFLAGS) -o $@ $^

run_mandel: $(MANDEL_EXEC)
	./$(MANDEL_EXEC) -f mandel.ppm

run_mandel_mpi: $(MANDEL_MPI_EXEC)
	mpirun -np 6 ./$(MANDEL_MPI_EXEC) -f mandel_mpi.ppm

run_mandel_mpi2: $(MANDEL_MPI2_EXEC)
	mpirun -np 6 ./$(MANDEL_MPI2_EXEC) -f mandel_mpi2.ppm

run_mandel_omp_static: $(MANDEL_OMP_STATIC_EXEC)
	./$(MANDEL_OMP_STATIC_EXEC) -f mandel_omp_static.ppm

run_mandel_omp_dynamic: $(MANDEL_OMP_DYNAMIC_EXEC)
	./$(MANDEL_OMP_DYNAMIC_EXEC) -f mandel_omp_dynamic.ppm

run: run_mandel run_mandel_mpi run_mandel_mpi2 run_mandel_omp_static run_mandel_omp_dynamic

clean:
	rm -f $(MANDEL_EXEC) $(MANDEL_MPI_EXEC) $(MANDEL_MPI2_EXEC) \
	      $(MANDEL_OMP_STATIC_EXEC) $(MANDEL_OMP_DYNAMIC_EXEC) \
	      $(COMMON_SRC:.c=.o) $(MANDEL_SRC:.c=.o) $(MANDEL_MPI_SRC:.c=.o) \
	      $(MANDEL_MPI2_SRC:.c=.o) *.ppm

.PHONY: all clean run run_mandel run_mandel_mpi run_mandel_mpi2 run_mandel_omp_static run_mandel_omp_dynamic
