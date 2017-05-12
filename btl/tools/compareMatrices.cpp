#include "../utils/io.h"
#include <stdlib.h>

void showHelp()
{
	printf("Compare two matrices and check whether the two are equal\n"
		   "Currently only support integer square matrices\n"
		   "\nUsage:\n"
		   "compareMatrices <first-matrix> <second-matrix>\n");
}

bool compareVectors(const int* vtA, const int* vtB, int rowIndex, int size)
{
	bool equal = true;
	for (int i = 0; i < size; ++i) {
		if (vtA[i] != vtB[i]) {
			printf("At (%d, %d) element %d of A is different from element %d of B\n", rowIndex, i, vtA[i], vtB[i]);
			equal = false;
			break;
		}
	}

	return equal;
}

void compareMatrices(const int mtASize, const int* mtA, const int mtBSize, const int* mtB)
{
	if (mtASize != mtBSize) {
		printf("Unequal size:\n"
			   "Matrix A size(%d, %d)\tMatrix B size(%d, %d)\n", mtASize, mtASize, mtBSize, mtBSize);
		return;
	}

	bool isEqual = true;
	for (int i = 0; i < mtASize; ++i) {
		if (!compareVectors(mtA + i * mtASize, mtB + i * mtASize, i, mtASize)) {
			isEqual = false;
		}
	}

	if (isEqual) {
		printf("Two matrices are EQUAl\n");
	} else {
		printf("Two matrices are NOT equal\n");
	}
}

void loadMatrix(const char* filePath, int& matrixSize, int*& matrixData)
{
	matrixSize = loadMatrixSizeFromPathBinary(filePath);
	if (!matrixSize) {
		// some errors happened
		matrixData = 0;
		return;
	}

	matrixData = loadMatrixRowsFromPathBinary<int>(filePath, 0, matrixSize);
}

int main(int argc, char** argv)
{
	if (argc != 3) {
		showHelp();
		return 1;
	}

	// load matrices
	int matrixSizeA, matrixSizeB;
	int* matrixDataA, *matrixDataB;
	loadMatrix(argv[1], matrixSizeA, matrixDataA);
	loadMatrix(argv[2], matrixSizeB, matrixDataB);

	if (!matrixSizeA || !matrixSizeB || !matrixDataA || !matrixDataB) {
		fprintf(stderr, "Something wrong happened\n");
		return 1;
	}

	compareMatrices(matrixSizeA, matrixDataA, matrixSizeB, matrixDataB);

	delete[] matrixDataA;
	delete[] matrixDataB;
	return 0;
}
