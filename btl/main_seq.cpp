#include <stdlib.h>
#include "utils/io.h"
#include "common.h"
#include <time.h>

#define RESULT_MATRIX_FILE "result_seq.mat.bin"

int main()
{
	int matrixSizeA = loadMatrixSizeFromPathBinary(A_MATRIX_FILE);
	int matrixSizeB = loadMatrixSizeFromPathBinary(B_MATRIX_FILE);

	if (matrixSizeA != matrixSizeB) {
		fprintf(stderr, "Matrix A size %d cannot multiply with matrix B size %d\n", matrixSizeA, matrixSizeB);
		return 1;
	}

	clock_t startTime = clock();
	int* matrixDataA = loadMatrixRowsFromPathBinary<int>(A_MATRIX_FILE, 0, matrixSizeA);
	int* matrixDataB = loadMatrixRowsFromPathBinary<int>(B_MATRIX_FILE, 0, matrixSizeB);
	if (!matrixDataA || !matrixDataB) {
		return 1;
	}

	int* matrixDataResult = new int[matrixSizeA * matrixSizeA];
	if (!matrixDataResult) {
		fprintf(stderr, "Cannot allocate memory to store result matrix\n");
		return 1;
	}

	multiplyMatrices(matrixDataA, matrixDataB, matrixSizeA, matrixSizeA, matrixDataResult);

	// save result matrix to file
	FILE* file = fopen(RESULT_MATRIX_FILE, "wb");
	if (file) {
		fwrite(&matrixSizeA, sizeof(int), 1, file);
		fwrite(&matrixSizeA, sizeof(int), 1, file);
		saveMatrixRowsToFileBinary(file, 0, matrixSizeA, matrixSizeA, matrixDataResult);
	} else {
		fprintf(stderr, "Cannot open file %s to save result matrix\n", RESULT_MATRIX_FILE);
	}

	// clean up
	delete[] matrixDataA;
	delete[] matrixDataB;
	delete[] matrixDataResult;
	clock_t endTime = clock();

	// print the time result
	printf("Process %d:\n"
		   "\tStarted at:\t%f\n"
		   "\tEnded at:\t%f\n"
		   "\tTotal:\t%f\n", 0, (double)startTime/CLOCKS_PER_SEC, (double)endTime/CLOCKS_PER_SEC, (double)(endTime - startTime)/CLOCKS_PER_SEC);

	return 0;
}
