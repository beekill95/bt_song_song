#ifndef COMMON_H
#define COMMON_H

#define A_MATRIX_FILE "a.mat.bin"
#define B_MATRIX_FILE "b.mat.bin"

/**
 * Multiply two vectors
 *
 * @param vtA a vector to multiply
 * @param vtB a vector to multiply
 * @param size the size of the vector to multiply
 * @return the production of two vectors
 * */
int multiplyVectors(const int* vtA, const int* vtB, int size);

/**
 * Multiply two matrices together: mtA * mtB. Note that: mtB is already a transposed matrix.
 *
 * @param mtA matrix A to multiply
 * @param mtB matrix B to multiply. This matrix is already tranposed
 * @param row
 * */
void multiplyMatrices(const int* mtA, const int* mtB, int row, int col, int* result);

/**
 *
 * */
void swapAddress(int*& mtA, int*& mtB);

/**
 *
 * */
void printMatrix(const int* matrix, int rows, int cols, int process = 0, const char* matrixName = "");

/**
 *
 * */
int minusOneWrapAround(int value, int endValue);

#endif
