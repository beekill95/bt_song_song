#include <stdlib.h>
#include <stdio.h>
#include <mpi.h>

/**
 * Load matrix size from file path
 *
 * @param filePath the path to the file
 * @return the size of the matrix if success, 0 otherwise
 * */
int loadMatrixSize(char* filePath)
{
	FILE* file = fopen(filePath, "r");
	int row, col;

	if (file == 0) {
		fprintf(stderr, "Cannot open file %s to load matrix size\n", filePath);
		return 0;
	}

	if (fscanf(file, "%d %d", &row, &col) != 2) {
		fprintf(stderr, "Unrecognized format %s\n", filePath);
		return 0;
	}

	if (row != col) {
		fprintf(stderr, "Unimplemented when row not equals col\n");
		return 0;
	}

	fclose(file);
	return row;
}

void loadMatrixRow(char* filePath, int rowIndex, int** row)
{

}

int main(int argc, char** argv)
{
	MPI_Init(&argc, &argv);

	MPI_Finalize();
	return 0;
}
