#include <mpi.h>
#include <stdio.h>
#include "matrix.h"
#include "io.h"

#define MATRIX_A_PATH "matrix/a.mat"
#define MATRIX_B_PATH "matrix/b.mat"

int main(int argc, char** argv)
{
	int rank, size;
	Matrix *a = 0, 
		   *b = 0, 
		   *result = 0;

	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	if (rank == 0) {
		// load the matrices
		a = loadMatrix(MATRIX_A_PATH, 0);
		b = loadMatrix(MATRIX_B_PATH, 1);
	}
	// wait after the first process finished loading the matrices
	// MPI_Barrier(MPI_COMM_WORLD);


	// free up memory
	if (a)
		destroyMatrix(&a);
	if (b)
		destroyMatrix(&b);
	if (result)
		destroyMatrix(&result);

	MPI_Finalize();
	return 0;
}
