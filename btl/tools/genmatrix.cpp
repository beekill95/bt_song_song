#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#define FILE_MAT_A "a.mat.bin"
#define FILE_MAT_B "b.mat.bin"

void showHelp()
{
	printf("Generate matrix for multiplication\n"
		   "Usage: genmatrix <matrix-size>\n"
		   "With <matrix-size> is an non-zero integer\n"
		   "\nThe generated files are a.mat and b.mat,\n"
		   "with b.mat contains a transposed matrix\n");
}

void genMatrixToFile(int matrixSize, char* matrixFile)
{
	FILE* file = fopen(matrixFile, "wb");
	int* row;
	int i, j;

	// write matrix size
	fwrite(&matrixSize, sizeof(int), 1, file);
	fwrite(&matrixSize, sizeof(int), 1, file);

	// write matrix elements
	row = (int*) calloc(matrixSize, sizeof(int));
	if (!row) {
		fprintf(stderr, "Cannot initialize array to store matrix element");
		return;
	}

	for (i = 0; i < matrixSize; ++i) {
		for (j = 0; j < matrixSize; ++j) {
			row[j] = rand() % 100;
		}
		fwrite(row, sizeof(int), matrixSize, file);
	}

	fflush(file);
	fclose(file);
}

int main(int argc, char** argv)
{
	int matrixSize;
	if (argc != 2) {
		showHelp();
		return 1;
	}

	if (!(matrixSize = atoi(argv[1]))) {
		showHelp();
		return 1;
	}

	genMatrixToFile(matrixSize, FILE_MAT_A);
	genMatrixToFile(matrixSize, FILE_MAT_B);

	return 0;
}
