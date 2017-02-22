#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char** argv)
{
	char fileName[20];
	int rank, size;
	FILE* fp;

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);

	if (rank < 2) {
		sprintf(fileName, "file_%d.out", rank);
		fp = fopen(fileName, "w");
		fprintf(fp, "P%d: before barrier\n", rank);
		fclose(fp);
	}
	MPI_Barrier(MPI_COMM_WORLD);

	if (rank < 2) {
		sprintf(fileName, "file_%d.out", rank == 0 ? 1 : 0);
		fp = fopen(fileName, "a");	
		fprintf(fp, "P%d: after barrier\n", rank);
		fclose(fp);
	}

	MPI_Finalize();
	return 0;
}

