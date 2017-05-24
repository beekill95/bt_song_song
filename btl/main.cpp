#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <mpi.h>
#include "utils/io.h"
#include "common.h"

#define TAG 0
#define RESULT_MATRIX_FILE "result_two_sided.mat.bin"

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

	// matrix to save recieved maxtrix from previous process
	int* matrixDataC = new int[blockMatrixSize];
	// intialize result matrix
	int* matrixDataResult = new int[blockMatrixSize];

	// multiply matrix data first
	multiplyMatrices(matrixDataA, matrixDataB, end - begin, matrixSize, matrixDataResult + (end - begin) * rank);
	
	//
	int i = minusOneWrapAround(rank, processCount);
	int nextProcess = (rank + 1) % processCount;
	int prevProcess = minusOneWrapAround(rank, processCount);
	while (i != rank) {
		// even process will send first, and then odd process will send
		if (rank % 2 == 0) {
			MPI_Send(matrixDataB, blockMatrixSize, MPI_INT, nextProcess, TAG, MPI_COMM_WORLD);
			//printf("Process %d has sent to #%d and receiving from #%d\n", rank, nextProcess, prevProcess);
			MPI_Recv(matrixDataC, blockMatrixSize, MPI_INT, prevProcess, TAG, MPI_COMM_WORLD, &status);
		} else {
			MPI_Recv(matrixDataC, blockMatrixSize, MPI_INT, prevProcess, TAG, MPI_COMM_WORLD, &status);
			//printf("Process %d has sent to #%d and receiving from #%d\n", rank, nextProcess, prevProcess);
			MPI_Send(matrixDataB, blockMatrixSize, MPI_INT, nextProcess, TAG, MPI_COMM_WORLD);
		}
		swapAddress(matrixDataB, matrixDataC);
		multiplyMatrices(matrixDataA, matrixDataB, end - begin, matrixSize, matrixDataResult + (end - begin) * i);

		i = minusOneWrapAround(i, processCount);
	}

	//printMatrix(matrixDataResult, end - begin, matrixSize, rank, "result");

	// save result matrices to file
	MPI_File_open(MPI_COMM_WORLD, RESULT_MATRIX_FILE, MPI_MODE_CREATE | MPI_MODE_WRONLY, MPI_INFO_NULL, &matrixFileResult);
	// we only need one process to write matrix size
	if (rank == 0) {
		MPI_File_write(matrixFileResult, &matrixSize, 1, MPI_INT, &status);
		MPI_File_write(matrixFileResult, &matrixSize, 2, MPI_INT, &status);
	}
	saveMatrixRowsToFileBinaryParallel(&matrixFileResult, begin, end, matrixSize, matrixDataResult);
	MPI_File_close(&matrixFileResult);

	delete[] matrixDataA;
	delete[] matrixDataB;
	delete[] matrixDataC;
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

	double startTime = MPI_Wtime();
	process(rank, size, matrixSizeA);
	double endTime = MPI_Wtime();

	printf("Process %d:\n"
		   "\tStarted at:\t%f\n"
		   "\tEnded at:\t%f\n"
		   "\tTotal:\t%f\n", rank, startTime, endTime, endTime - startTime);

	MPI_Finalize();
	return 0;
}
