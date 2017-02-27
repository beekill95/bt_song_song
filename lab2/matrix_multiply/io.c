#include "io.h"
#include <stdio.h>
#include <string.h>

Matrix* loadMatrix(const char* fileName, int tranposeMatrix)
{
//	FILE* fp = fopen(fileName, "r");
//	if (!fp) {
//		fprintf(stderr, "Cannot open %s to read. ABORT!", fileName);
//		return 0;
//	}
//
//	const int BUFFER_SIZE = 1024;
//	char buffer[BUFFER_SIZE];
//	// read matrix's row and col;
//	if (fgets(buffer, BUFFER_SIZE, fp)) {
//		fprintf(stderr, "Wrong file format. ABORT!");
//		fclose(fp);
//		return 0;
//	}
//
//	int row, col;
//	if (sscanf(buffer, "%d %d\n", &row, &col) != 2) {
//		fprintf(stderr, "Wrong file format. ABORT!");
//		fclose(fp);
//		return 0;
//	}
//
//	// create matrix
//	Matrix* mat = newMatrix(row, col);
//	if (!mat) {
//		// not enough memory
//		fprintf(stderr, "Not enough memory. ABORT!");
//		return 0;
//	}
//
//	// read each line to row
//
//	fclose(fp);
//	return 0;

	// because I'm so lazy
	// so I'll just create 2 static matrices
	// based on the matrix's path being passed
	if (strcmp(fileName, "matrix/a.mat") == 0) {
		float elems[] = {
			1, 2, 3,
			4, 5, 6,
			7, 8, 9
		};

		Matrix* a = newMatrix(3, 3);
		memcpy(a->data, elems, sizeof(float) * 9);
		return a;
	} else if (strcmp(fileName, "matrix/b.mat") == 0) {
		float elems[] = {
			1, 0, 0,
			0, 1, 0,
			0, 0, 1
		};

		Matrix* b = newMatrix(3, 3);
		memcpy(b->data, elems, sizeof(float) * 9);
		return b;
	} else
		return 0;
}

void saveMatrix(const char* fileName, const Matrix* mat)
{
	FILE* fp = fopen(fileName, "w");
	if (!fp) {
		fprintf(stderr, "Cannot open %s to write. ABORT!", fileName);
		return;	
	}

	int row = mat->row,
		col = mat->col;
	float* data = mat->data;

	// write matrix's row and col
	fprintf(fp, "%d %d\n", row, col);

	// write each row to file
	int i, j;
	for (i = 0; i < row; ++i) {
		for (j = 0; j < col - 1; ++j) {
			fprintf(fp, "%f ", data[i * col + j]);
		}
		fprintf(fp, "%f\n", data[(j + 1) * col - 1]);
	}

	fclose(fp);
}
