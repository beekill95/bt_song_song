#ifndef MATRIX_H
#define MATRIX_H

typedef struct Matrix {
	int row, col;
	float* data;
} Matrix;

Matrix* newMatrix(int row, int col);
void destroyMatrix(Matrix** mat);

#endif
