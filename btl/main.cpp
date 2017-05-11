#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <mpi.h>
#include "utils/io.h"

#define TAG 0
#define A_MATRIX_FILE "a.mat.bin"
#define B_MATRIX_FILE "b.mat.bin"
#define RESULT_MATRIX_FILE "result.mat.bin"

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
			int idx = i*col + j;
			result[idx] = res;
		}
	}
}

void swapAddress(int*& mtA, int*& mtB){
	int* mtC = mtA;
	mtA = mtB;
	mtB = mtC;
}

void printMatrix(const int* matrix, int rows, int cols)
{
	for (int i = 0; i < rows; ++i) {
		for (int j = 0; j < cols; ++j) {
			printf("%d ", matrix[i * cols + j]);
		}
		printf("\n");
	}
}

// assume that matrixSize % processCount == 0
void process(int rank, int processCount, int matrixSize)
{
	assert(matrixSize % processCount == 0);
	int begin = rank * matrixSize / processCount;
	int end = (rank + 1) * matrixSize / processCount;
	printf("(%d, %d)\n", begin, end);

	int blockMatrixSize = (end - begin) * matrixSize;
	MPI_Status status;
	MPI_File matrixFileA, matrixFileB, matrixFileResult;

	// load matrix data from begin to end
	MPI_File_open(MPI_COMM_WORLD, A_MATRIX_FILE, MPI_MODE_RDONLY, MPI_INFO_NULL, &matrixFileA);
	MPI_File_open(MPI_COMM_WORLD, B_MATRIX_FILE, MPI_MODE_RDONLY, MPI_INFO_NULL, &matrixFileB);
	int* matrixDataA = loadMatrixRowsFromFileBinaryParallel(&matrixFileA, matrixSize, matrixSize, begin, end);
	int* matrixDataB = loadMatrixRowsFromFileBinaryParallel(&matrixFileB, matrixSize, matrixSize, begin, end);
	MPI_File_close(&matrixFileA);
	MPI_File_close(&matrixFileB);

	printMatrix(matrixDataA, matrixSize, matrixSize);
	printf("\n");
	printMatrix(matrixDataB, matrixSize, matrixSize);
	printf("\n");

	// matrix to save recieved maxtrix from previous process
	int* matrixDataC = new int[blockMatrixSize];
	// intialize result matrix
	int* matrixDataResult = new int[blockMatrixSize];

	// multiply matrix data first
	multiplyMatrices(matrixDataA, matrixDataB, end - begin, matrixSize, matrixDataResult);
	MPI_Send(matrixDataB, blockMatrixSize, MPI_INT, (rank + 1) % processCount, TAG, MPI_COMM_WORLD);
	
	//
	for (int i = 0; i < processCount - 1; ++i) {
	  	if(rank != 0)
			MPI_Recv(matrixDataC, blockMatrixSize, MPI_INT, rank - 1, TAG, MPI_COMM_WORLD, &status);
		else
			MPI_Recv(matrixDataC, blockMatrixSize, MPI_INT, processCount - 1, TAG, MPI_COMM_WORLD, &status);
		swapAddress(matrixDataB, matrixDataC);
		multiplyMatrices(matrixDataA, matrixDataB, end - begin, matrixSize, matrixDataResult + (end - begin) * sizeof(int));
	}

	printMatrix(matrixDataResult, matrixSize, matrixSize);

	// save result matrices to file
	MPI_File_open(MPI_COMM_WORLD, RESULT_MATRIX_FILE, MPI_MODE_CREATE | MPI_MODE_WRONLY, MPI_INFO_NULL, &matrixFileResult);
	// we only need one process to write matrix size
	if (rank == 0) {
		MPI_File_write(matrixFileResult, &matrixSize, 1, MPI_INT, &status);
		MPI_File_write(matrixFileResult, &matrixSize, 2, MPI_INT, &status);
	}
	saveMatrixRowsToFileBinaryParallel(&matrixFileResult, begin, end, matrixSize, matrixDataResult);
	MPI_File_close(&matrixFileResult);
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

	printf("Rank: %d, Size: %d\n", rank, size);
	process(rank, size, matrixSizeA);

	MPI_Finalize();
	return 0;
}
