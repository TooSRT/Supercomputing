CC = mpicc                             
CFLAGS = -Wall

#Repertory of projects
MANDEL_DIR = mandel
MANDEL_MPI_DIR = mandel_MPI

#Fichiers sources
MANDEL_SRC = $(MANDEL_DIR)/mandel.c
MANDEL_MPI_SRC = $(MANDEL_MPI_DIR)/mandel_MPI.c

# Fichiers exécutables
MANDEL_EXEC = mandel.out
MANDEL_MPI_EXEC = mandel_MPI.out

#Compilation
all: $(MANDEL_EXEC) $(MANDEL_MPI_EXEC)

$(MANDEL_EXEC): $(MANDEL_SRC)
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

$(MANDEL_MPI_EXEC): $(MANDEL_MPI_SRC)
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

# Exécution de mandel
run_mandel: $(MANDEL_EXEC)
	./$(MANDEL_EXEC) -f output.ppm

#Exécution de mandel_MPI
run_mandel_mpi: $(MANDEL_MPI_EXEC)
	mpirun -np 6 ./$(MANDEL_MPI_EXEC) -f output_mpi.ppm

# Exécuter tout
run: run_mandel run_mandel_mpi

clean:	
	rm -f $(MANDEL_EXEC) $(MANDEL_MPI_EXEC) *.ppm

.PHONY: all clean run run_mandel run_mandel_mpi


