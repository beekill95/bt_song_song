#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#define FILE_MAT_A "a.mat"
#define FILE_MAT_B "b.mat"

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
	FILE* file = fopen(matrixFile, "w");
	int i, j;

	if (file == 0) {
		printf("Error: Cannot open file %s to write", matrixFile);
		return;
	}

	fprintf(file, "%d %d\n", matrixSize, matrixSize);
	for (i = 0; i < matrixSize; ++i) {
		for (j = 0; j < matrixSize; ++j) {
			fprintf(file, " %d", rand() % 100);
		}
		fprintf(file, "\n");
	}

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
