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
	size_t read;

	if (rowIndex < 0 || rowIndex >= rows) {
		fprintf(stderr, "Invalid row index: %d with matrix size (%d, %d)\n", rowIndex, rows, cols);
		return 0;
	}

	row = (T*) calloc(cols, sizeof(T));
	if (!row) {
		fprintf(stderr, "Failed to initialize memory block to store row's element");
		return 0;
	}

	// seek to the row index position
	fseek(file, rowIndex * cols * sizeof(T), SEEK_CUR);

	// read the whole block of elements to row
	read = fread(row, sizeof(T), cols, file);
	if (read != cols) {
		fprintf(stderr, "Failed while reading: The number of read elements is different from expected");
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
void saveMatrixRowToFileBinary(FILE* file, int rowIndex, int cols, const T* row)
{
	assert(rowIndex > 0);
	assert(cols > 0);
	fseek(file, rowIndex * cols * sizeof(T), SEEK_CUR);
	fwrite(row, sizeof(T), cols, file);
}
