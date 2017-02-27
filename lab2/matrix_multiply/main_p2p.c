#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include "matrix.h"
#include "io.h"

#define MATRIX_A_PATH "matrix/a.mat"
#define MATRIX_B_PATH "matrix/b.mat"

#define A_ROW_TAG 1
#define B_COL_TAG 2
#define PROCESS_RESULT_TAG 0

float multiplyArray(const float* lhs, const float* rhs, int size)
{
	int i = 0;
	float result = 0.0f;
	for (; i < size; ++i)
		result += lhs[i] * rhs[i];
	return result;
}

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

void processZeroWork(const Matrix* a, const Matrix* b, Matrix* result, int arrSize)
{
	int resultSize = result->row * result->col;
	int p;

	// send the aRow and bCol to other processes
	// first send aRow, and then bCol
	for (p = 1; p < resultSize; ++p) {
		int r = p / result->col;
		float* row = a->data + r;
		float* col = b->data + (p - r * result->col);

		MPI_Send(row, arrSize, MPI_FLOAT, p, A_ROW_TAG, MPI_COMM_WORLD);
		MPI_Send(col, arrSize, MPI_FLOAT, p, B_COL_TAG, MPI_COMM_WORLD);
	}

	// get the buffer and calculate the result
	// and assign it to the result's data
	result->data[0] = multiplyArray(a->data, b->data, arrSize);

	// receive the result from other processes
	for (p = 1; p < resultSize; ++p) {
		int pResult;
		MPI_Status status;
		MPI_Recv(&pResult, 1, MPI_FLOAT, p, PROCESS_RESULT_TAG, MPI_COMM_WORLD, &status);

		result->data[p] = pResult;
	}

	// show the matrix result
	printMatrix(result, "result");
}

void otherProcessesWork(int rank, int arrSize)
{
	float* aRow = (float*)calloc(arrSize, sizeof(float));
	float* bCol = (float*)calloc(arrSize, sizeof(float));

	// receive the aRow and bCol from process 0
	// first receive aRow
	// then receive bCol
	MPI_Status status;
	MPI_Recv(aRow, arrSize, MPI_FLOAT, 0, A_ROW_TAG, MPI_COMM_WORLD, &status);
	MPI_Recv(bCol, arrSize, MPI_FLOAT, 0, B_COL_TAG, MPI_COMM_WORLD, &status);

	// compute the result
	float result = multiplyArray(aRow, bCol, arrSize);

	// send the result back to the process zero
	MPI_Send(&result, 1, MPI_FLOAT, 0, PROCESS_RESULT_TAG, MPI_COMM_WORLD);

	free(aRow);
	free(bCol);
}

int main(int argc, char** argv)
{
	int rank, size;
	// result = a * b
	Matrix *a = 0, 
		   *b = 0, 
		   *result = 0;
	int resultSize, arrSize;

	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	if (rank == 0) {
		// load the matrices
		a = loadMatrix(MATRIX_A_PATH, 0);
		b = loadMatrix(MATRIX_B_PATH, 1); // the matrix is loaded and then transposed

		// check whether we load the matrices successfully
		if (!a || !b)
			MPI_Abort(MPI_COMM_WORLD, -1);
		else if (a->col != b->col) {
			destroyMatrix(&a);
			destroyMatrix(&b);

			// check whether the matrices we loaded
			// are able to multiply together
			MPI_Abort(MPI_COMM_WORLD, -1);
		} else if (size != a->row * b->row) {
			destroyMatrix(&a);
			destroyMatrix(&b);
			fprintf(stderr, "Please choose the number of processes equal to the size of result matrix, i.e %d\n", a->row * b->row);
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
		arrSize = a->col;
	}
	
	// broadcast the array size and result matrix size
	MPI_Bcast(&arrSize, 1, MPI_INT, 0, MPI_COMM_WORLD);
	MPI_Bcast(&resultSize, 1, MPI_INT, 0, MPI_COMM_WORLD);

	// only working with number of processes equal to the size of result matrix
	if (size == resultSize) {
		if (rank == 0)
			processZeroWork(a, b, result, arrSize);
		else
			otherProcessesWork(rank, arrSize);
	}

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
