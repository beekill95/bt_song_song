#ifndef IO_H
#define IO_H
#include <stdio.h>
#include <mpi.h>

/**
 * Load matrix size from file path
 *
 * @param filePath the path to the file
 * @return the size of the matrix if success, 0 otherwise
 * */
int loadMatrixSizeFromPathBinary(const char* filePath);

/**
 * The function will load a whole matrix row at rowIndex and return the loaded row.
 * It assumes that the FILE struct is not null and both row and col is a legal 
 * matrix row and column. The function also assumes that the file position is currently
 * at the beginning of the matrix (i.e the file position is pointing to the first element
 * in the matrix). In order to use the function correctly, you should seek to the position
 * of the first element in the matrix.
 *
 * @param file a FILE struct that describe the opened file
 * @param rows the number of rows of the matrix
 * @param cols the number of cols of the matrix
 * @param rowIndex the index of the row that needs loading
 * @return the row array if loaded succesffuly, null otherwise
 * */
template<typename T>
T* loadMatrixRowFromFileBinary(FILE* file, int rows, int cols, int rowIndex);

/**
 *
 * */
template<typename T>
T* loadMatrixRowsFromFileBinary(FILE* file, int rows, int cols, int beginIndex, int endIndex);

/**
 *
 * */
//int* loadMatrixRowFromFileBinaryParallel(MPI_FILE* file, int rows, int cols, int rowIndex);

/**
 *
 * */
int* loadMatrixRowsFromFileBinaryParallel(MPI_File* file, int rows, int cols, int beginIndex, int endIndex);

/**
 * The function will open a FILE from file path and
 * load a whole matrix row at rowIndex and return the loaded row.
 *
 * @param filePath the path to the file
 * @param rowIndex the index of the row that will be loaded
 * @return the row array if loaded successfully, null otherwise
 * */
template<typename T>
T* loadMatrixRowFromPathBinary(const char* filePath, int rowIndex);

/**
 *
 * */
template<typename T>
T* loadMatrixRowFromPathBinary(const char* filePath, int beginIndex, int endIndex);

/**
 * Save a whole matrix row to the specified row index. The function assumes that
 * the FILE struct is not null and the file position is pointing to the first element
 * in the matrix
 *
 * @param file a FILE struct that describe the opened file and is not null
 * @param rowIndex the row index that will be written to
 * @param cols the number of elements in the row
 * @param row the array of elements
 * */
template<typename T>
void saveMatrixRowToFileBinary(FILE* file, int rowIndex, int cols, const T* row);

/**
 *
 * */
template<typename T>
void saveMatrixRowsToFileBinary(FILE* file, int beginIndex, int endIndex, int cols, const T* rows);

/**
 *
 * */
template<typename T>
void saveMatrixRowsToPathBinary(const char* filePath, int beginIndex, int endIndex, int cols, const T* rows);

/**
 *
 * */
void saveMatrixRowsToFileBinaryParallel(MPI_File* file, int beginIndex, int endIndex, int cols, const int* manyRows);

#include "io_impl.h"
#endif
