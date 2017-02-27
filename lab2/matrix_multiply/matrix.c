#include "matrix.h"
#include <stdlib.h>
#include <string.h>

Matrix* newMatrix(int row, int col)
{
	Matrix* mat = (Matrix*) malloc(sizeof(struct Matrix));
	mat->row = row;
	mat->col = col;
	mat->data = (float*) calloc(row * col, sizeof(float));

	if (mat->data) {
		memset(mat->data, 0, row * col * sizeof(float));
		return mat;
	} else {
		free(mat);
		return 0;
	}
}

void destroyMatrix(Matrix** mat)
{
	free((*mat)->data);
	free(*mat);
	*mat = 0;
}
