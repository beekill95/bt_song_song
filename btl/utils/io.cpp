#include "io.h"
#include <assert.h>
#define BUFFER_SIZE 256

int* loadMatrixRowsFromFileBinaryParallel(MPI_FILE* file, int rows, int cols, int beginIndex, int endIndex)
{
	if (beginIndex < 0 || beginIndex >= rows || endIndex < 0 || endIndex >= rows) {
		fprintf(stderr, "Invalid row range: (%d - %d) for matrix size (%d, %d)\n", beginIndex, endIndex, rows, cols);
		return 0;
	}

	int elementCount = (endIndex - beginIndex) * cols;
	int* manyRows = new int[elementCounts];
	if (!manyRows) {
		fprintf(stderr, "Cannot allocate array to store matrix element\n");
		return 0;
	}

	// load elements
	MPI_Status status;
	int errorCode = MPI_File_read_at(*file, beginIndex * cols * sizeof(int), manyRows, elementCount, MPI_INT, &status);

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

void saveMatrixRowsToFileBinaryParallel(MPI_FILE* file, int beginIndex, int endIndex, int cols, const int* manyRows)
{
	assert(beginIndex >= 0);
	assert(beginIndex < endIndex);
	assert(cols > 0);

	// write file
	MPI_Status status;
	int elementCount = (endIndex - beginIndex) * cols;
	int errorCode = MPI_File_write_at(*file, beginIndex * cols * sizeof(int), manyRows, elementCount, MPI_INT, &status);

	// check for any errors
	if (errorCode != MPI_SUCCESS) {
		char errorString[BUFFER_SIZE];
		int errorStringLength;
		MPI_Error_string(errorCode, errorString, &errorStringLength);

		fprintf(stderr, "Error Code %d: %s\n", errorCode, errorString);
	}
}
