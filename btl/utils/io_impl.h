#include "io.h"
#include <stdlib.h>
#include <assert.h>

/**
 * Internal function for loading matrix size
 *
 * @param file the FILE struct that is returned from fopen and not equals null
 * @return the row of the matrix if no errors occur
 * */
int loadMatrixSizeFromFileBinary(FILE* file);

template<typename T>
T* loadMatrixRowFromFileBinary(FILE* file, int rows, int cols, int rowIndex)
{
	T* row;

	if (rowIndex < 0 || rowIndex >= rows) {
		fprintf(stderr, "Invalid row index: %d with matrix size (%d, %d)\n", rowIndex, rows, cols);
		return 0;
	}

	row = new T[cols];
	if (!row) {
		fprintf(stderr, "Failed to initialize memory block to store row's element");
		return 0;
	}

	// seek to the row index position
	fseek(file, rowIndex * cols * sizeof(T), SEEK_CUR);

	// read the whole block of elements to row
	int read = fread(row, sizeof(T), cols, file);
	if (read != cols) {
		fprintf(stderr, "Failed while reading: The number of read elements is different from expected\n");
		delete[] row;
		return 0;
	}

	return row;
}

template<typename T>
T* loadMatrixRowFromPathBinary(const char* filePath, int rowIndex)
{
	FILE* file;
	int rows;
	T* row;

	file = fopen(filePath, "rb");
	if (!file) {
		fprintf(stderr, "Cannot open file %s to load matrix row", filePath);
		return 0;
	}

	// load matrix size, currently only support square matrix
	rows = loadMatrixSizeFromFileBinary(file);
	if (!rows) {
		return 0;
	}

	fseek(file, 2 * sizeof(T), SEEK_SET);
	row = loadMatrixRowFromFileBinary<T>(file, rows, rows, rowIndex);
	fclose(file);
	return row;
}

template<typename T>
T* loadMatrixRowsFromFileBinary(FILE* file, int rows, int cols, int beginIndex, int endIndex)
{
	if (beginIndex < 0 || beginIndex >= rows || endIndex < 0 || endIndex > rows) {
		fprintf(stderr, "Invalid row range %d - %d for matrix size (%d, %d)\n", rows, cols, beginIndex, endIndex);
		return 0;
	}

	int elementCount = (endIndex - beginIndex) * cols;
	T* manyRows = new T[elementCount];
	if (!manyRows) {
		fprintf(stderr, "Cannot allocate memory to store matrix loaded from file\n");
		return 0;
	}

	fseek(file, beginIndex * cols * sizeof(T), SEEK_CUR);
	int read = fread(manyRows, sizeof(T), elementCount, file);
	if (read != elementCount) {
		fprintf(stderr, "Not reading enough elements from file\n");
		delete[] manyRows;
		return 0;
	}

	return manyRows;
}

template<typename T>
T* loadMatrixRowsFromPathBinary(const char* filePath, int beginIndex, int endIndex)
{
	FILE* file = fopen(filePath, "rb");
	if (!file) {
		fprintf(stderr, "Cannot read file from path %s\n", filePath);
		return 0;
	}

	int rows = loadMatrixSizeFromFileBinary(file);
	if (!rows) {
		return 0;
	}

	fseek(file, 2 * sizeof(T), SEEK_SET);
	T* manyRows = loadMatrixRowsFromFileBinary<T>(file, rows, rows, beginIndex, endIndex);
	fclose(file);
	return manyRows;
}

template<typename T>
void saveMatrixRowToFileBinary(FILE* file, int rowIndex, int cols, const T* row)
{
	assert(rowIndex >= 0);
	assert(cols > 0);
	fseek(file, rowIndex * cols * sizeof(T), SEEK_CUR);
	fwrite(row, sizeof(T), cols, file);
}

template<typename T>
void saveMatrixRowsToFileBinary(FILE* file, int beginIndex, int endIndex, int cols, const T* rows)
{
	for (int i = beginIndex; i < endIndex; ++i) {
		saveMatrixRowToFileBinary(file, i, cols, rows + i * cols);
		fseek(file, 2 * sizeof(T), SEEK_SET);
	}
}

template<typename T>
void saveMatrixRowsToPathBinary(const char* filePath, int beginIndex, int endIndex, int cols, const T* rows)
{
	FILE* file = fopen(filePath, "wb");
	if (!file) {
		fprintf(stderr, "Cannot open file %s to write\n", filePath);
		return;
	}

	fseek(file, 2 * sizeof(T), SEEK_SET);
	saveMatrixRowsToFileBinary<T>(file, beginIndex, endIndex, cols, rows);
	fclose(file);
}
