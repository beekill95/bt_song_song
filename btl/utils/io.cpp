#include "io.h"
#include <assert.h>
#define BUFFER_SIZE 256

/**
 * Internal function for loading matrix size
 *
 * @param file the FILE struct that is returned from fopen and not equals null
 * @return the row of the matrix if no errors occur
 * */
int loadMatrixSizeFromFileBinary(FILE* file)
{
	int row, col;
	fread(&row, 1, sizeof(int), file);
	fread(&col, 1, sizeof(int), file);

	if (row != col) {
		fprintf(stderr, "Unimplemented when row not equals col\n");
		return 0;
	}

	return row;
}

int loadMatrixSizeFromPathBinary(const char* filePath)
{
	FILE* file;
	int row;

	file = fopen(filePath, "rb");
	if (!file) {
		fprintf(stderr, "Cannot open file %s to load matrix size\n", filePath);
		return 0;
	}

	row = loadMatrixSizeFromFileBinary(file);

	fclose(file);
	return row;
}

int* loadMatrixRowsFromFileBinaryParallel(MPI_File* file, int rows, int cols, int beginIndex, int endIndex)
{
	if (beginIndex < 0 || beginIndex >= rows || endIndex < 0 || endIndex > rows) {
		fprintf(stderr, "Invalid row range: (%d - %d) for matrix size (%d, %d)\n", beginIndex, endIndex, rows, cols);
		return 0;
	}

	int elementCount = (endIndex - beginIndex) * cols;
	int* manyRows = new int[elementCount];
	if (!manyRows) {
		fprintf(stderr, "Cannot allocate array to store matrix element\n");
		return 0;
	}

	// load elements
	MPI_Status status;
	int offset = 2 * sizeof(int);
	int errorCode = MPI_File_read_at(*file, offset + beginIndex * cols * sizeof(int), manyRows, elementCount, MPI_INT, &status);

	// check for any errors
	if (errorCode != MPI_SUCCESS) {
		delete[] manyRows;

		char errorString[BUFFER_SIZE];
		int errorStringLength;
		MPI_Error_string(errorCode, errorString, &errorStringLength);

		fprintf(stderr, "Error Code %d: %s\n", errorCode, errorString);
		return 0;
	}

	return manyRows;
}

void saveMatrixRowsToFileBinaryParallel(MPI_File* file, int beginIndex, int endIndex, int cols, const int* manyRows)
{
	assert(beginIndex >= 0);
	assert(beginIndex < endIndex);
	assert(cols > 0);

	// write file
	MPI_Status status;
	int offset = 2 * sizeof(int);
	int elementCount = (endIndex - beginIndex) * cols;
	int errorCode = MPI_File_write_at(*file, offset + beginIndex * cols * sizeof(int), manyRows, elementCount, MPI_INT, &status);

	// check for any errors
	if (errorCode != MPI_SUCCESS) {
		char errorString[BUFFER_SIZE];
		int errorStringLength;
		MPI_Error_string(errorCode, errorString, &errorStringLength);

		fprintf(stderr, "Error Code %d: %s\n", errorCode, errorString);
	}
}
