#include "../utils/io.h"
#include <stdlib.h>
#include <stdio.h>

void showHelp()
{
	printf("View matrix in generated matrix file from genmatrix\n"
		   "\nUsage:\n"
		   "viewmatrix <input-file-path>");
}

void displayRow(int* row, int rowIndex, int cols)
{
	int i;
	if (row) {
		for (i = 0; i < cols; ++i) {
			printf("%d ", row[i]);
		}
		printf("\n");
	} else {
		fprintf(stderr, "Error while loading row index %d\n", rowIndex);
	}
}

int main(int argc, char** argv)
{
	FILE* file;
	int rows, i;
	int* row;

	if (argc != 2) {
		showHelp();
		return 1;
	}

	file = fopen(argv[1], "rb");
	if (!file) {
		fprintf(stderr, "Cannot open file %s\n", argv[1]);
		return 1;
	}

	rows = loadMatrixSizeFromFileBinary(file);
	if (!rows) {
		return 1;
	}

	printf("Matrix size: (%d, %d)\n", rows, rows);
	for (i = 0; i < rows; ++i) {
		row = loadMatrixRowFromFileBinary<int>(file, rows, rows, i);
		displayRow(row, i, rows);
		free(row);
		fseek(file, 2 * sizeof(int), SEEK_SET);
	}

	fclose(file);
	return 0;
}
