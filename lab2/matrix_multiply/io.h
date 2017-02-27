#ifndef IO_H
#define IO_H
#include "matrix.h"

/*
 * Matrix file format
 *
 * First line will contains the number of row and column of a matrix
 * Remaining lines will contains the element in each row of the matrix
 * Each element is seperated by <space>
 * Each line is separated by <\n>
 *
 * Eg:
 * 3 4
 * 1 2 3 0
 * 5 7 8 9
 * 2 0 1 3
 * */

/*
 * Load matrix from file name and return a matrix
 * 
 * fileName: path to a file contains the matrix
 * transposeMatrix: a flag indicate that the matrix should be transposed before returning
 * */
Matrix* loadMatrix(const char* fileName, int transposeMatrix);

/*
 * Save matrix to a file
 *
 * fileName: path to the destination file
 * mat: a matrix that will be saved to file
 * */
void saveMatrix(const char* fileName, const Matrix* mat);

#endif
