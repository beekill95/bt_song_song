#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <mpi.h>
#include "utils/io.h"

/**
 * Multiply two vectors
 *
 * @param vtA a vector to multiply
 * @param vtB a vector to multiply
 * @param size the size of the vector to multiply
 * @return the production of two vectors
 * */
int multiplyVectors(const int* vtA, const int* vtB, int size)
{
	int sum = 0;
	for (int i = 0; i < size; ++i) {
		sum += vtA[i] * vtB[i];
	}
	return sum;
}

/**
 * Multiply two matrices together: mtA * mtB. Note that: mtB is already a transposed matrix.
 *
 * @param mtA matrix A to multiply
 * @param mtB matrix B to multiply. This matrix is already tranposed
 * @param row
 * */
void multiplyMatrices(const int* mtA, const int* mtB, int row, int col, int* result)
{
	for (int i = 0; i < row; ++i) {
		for (int j = 0; j < row; ++j) {
			int res = multiplyVectors(mtA + i * col, mtB + j * col, col);
			int idx = ;
			result[idx] = res;
		}
	}
}

// assume that matrixSize % processCount == 0
void process(int rank, int processCount, int matrixSize)
{
	assert(matrixSize % processCount == 0);
	int begin = rank * matrixSize / processCount;
	int end = (rank + 1) * matrixSize / processCount;

	// load matrix data from begin to end
	int* matrixDataA;
	int* matrixDataB;

	// intialize result matrix
	int* matrixDataResult = new int[(end - begin) * matrixSize];

	// multiply matrix data first
	multiplyMatrices(matrixDataA, matrixDataB, end - begin, matrixSize, matrixDataResult);

	for (int i = 0; i < processCount - 1; ++i) {
		
	}

}

int main(int argc, char** argv)
{
	// read matrix size
	// because we're assuming that both the matrices are square
	int matrixSizeA = loadMatrixSizeFromPathBinary("a.mat.bin");
	int matrixSizeB = loadMatrixSizeFromPathBinary("b.mat.bin");
	if (matrixSizeA != matrixSizeB) {
		fprintf(stderr, "Wrong matrix dimension: %d and %d", matrixSizeA, matrixSizeB);
		return 1;
	}

	int rank, size;
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);

	process(rank, size);

	MPI_Finalize();
	return 0;
}
