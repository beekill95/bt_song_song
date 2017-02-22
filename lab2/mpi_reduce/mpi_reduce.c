#include <mpi.h>
#include <stdio.h>

int main(int argc, char** argv)
{
	int rank, size;
	int send, recv;

	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	send = rank;
	MPI_Reduce(&send, &recv, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
	if (rank == 0)
		printf("The result received is %d", recv);

	MPI_Finalize();
	return 0;
}
