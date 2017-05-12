#include "common.h"
#include <stdio.h>

int multiplyVectors(const int* vtA, const int* vtB, int size)
{
	int sum = 0;
	for (int i = 0; i < size; ++i) {
		sum += vtA[i] * vtB[i];
	}
	return sum;
}

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

void printMatrix(const int* matrix, int rows, int cols, int process, const char* matrixName)
{
	printf("Process %d: Matrix %s\n", process, matrixName);
	for (int i = 0; i < rows; ++i) {
		for (int j = 0; j < cols; ++j) {
			printf("%d ", matrix[i * cols + j]);
		}
		printf("\n");
	}
}

int minusOneWrapAround(int value, int endValue)
{
	return value - 1 < 0 ? value - 1 + endValue : value - 1;
}


