#include <mpi.h>
#include <stdio.h>
#include "matrix.h"
#include "io.h"

#define MATRIX_A_PATH "matrix/a.mat"
#define MATRIX_B_PATH "matrix/b.mat"

void printMatrix(const Matrix* mat, const char* name)
{
	printf("Matrix %s:\n", name);

	int r, c;
	for (r = 0; r < mat->row; ++r) {
		const float* row = mat->data + r * mat->col;
		for (c = 0; c < mat->col - 1; ++c)
			printf("%f\t", row[c]);
		printf("%f\n", row[mat->col - 1]);
	}
}

int main(int argc, char** argv)
{
	int rank, size;
	Matrix *a = 0, 
		   *b = 0, 
		   *result = 0;
	int resultSize;
	float *row, *col;

	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	if (rank == 0) {
		// load the matrices
		a = loadMatrix(MATRIX_A_PATH, 0);
		b = loadMatrix(MATRIX_B_PATH, 1);
		
		// check whether we load the matrices successfully
		if (!a || !b)
			MPI_Abort(MPI_COMM_WORLD, -1);
		else if (a->col != b->col) {
			destroyMatrix(&a);
			destroyMatrix(&b);
			// check whether the matrices we loaded
			// are able to multiply together
			MPI_Abort(MPI_COMM_WORLD, -1);
		} else if (size != a->col) {
			destroyMatrix(&a);
			destroyMatrix(&b);
			// check whether we have enough processes
			fprintf(stderr, "Please specify the number of processes to %d\n", a->col);
			MPI_Abort(MPI_COMM_WORLD, -1);
		}

		// print the loaded matrices
		printMatrix(a, "a");
		printMatrix(b, "b");

		// create the result matrix to store the result
		result = newMatrix(a->row, b->row);
		if (!result)
			MPI_Abort(MPI_COMM_WORLD, -1);

		resultSize = result->row * result->col;
	}

	// broadcast the size to other processes
	MPI_Bcast(&resultSize, 1, MPI_INT, 0, MPI_COMM_WORLD);

	// loop through the resultSize
	// and calculate the dot product of each vector in matrix
	int i;
	for (i = 0; i < resultSize; ++i) {
		if (rank == 0) {
			int r = i / result->col;
			row = a->data + r * a->col;
			col = b->data + (i - r * result->col) * b->col;
		}

		// scatter vectors' elements to other processes
		// to calculate each vectors' elements product
		float rowElement, colElement;
		MPI_Scatter(row, 1, MPI_FLOAT, &rowElement, 1, MPI_FLOAT, 0, MPI_COMM_WORLD);
		MPI_Scatter(col, 1, MPI_FLOAT, &colElement, 1, MPI_FLOAT, 0, MPI_COMM_WORLD);

		// reduce the array to calculate the final dot product
		float product = rowElement * colElement;
		float sum = 0.0f;
		MPI_Reduce(&product, &sum, 1, MPI_FLOAT, MPI_SUM, 0, MPI_COMM_WORLD);

		// assign to result matrix
		if (rank == 0)
			result->data[i] = sum;	
	}

	// everything is finished
	// display the final result
	if (rank == 0)
		printMatrix(result, "result");

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
