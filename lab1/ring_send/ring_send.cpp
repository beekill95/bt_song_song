#include <mpi.h>
#include <cstdio>
#include <cstdlib>


// if messageToSend is nullptr
// then it is an indication that this is not the process that initiate the ring
void processWork(int* messageToSend, int rank, int size)
{
	int initialization = 7;
	int messageReceived;
	MPI_Status status;

	if (messageToSend) {
		// the process to initiate this ring
		// send to the next process
		MPI_Send(messageToSend, 1, MPI_INT, (rank + 1) % size, 11, MPI_COMM_WORLD);

		// listen for incomming message
		// only accept message from prev process in ring
		MPI_Recv(&messageReceived, 1, MPI_INT, (rank - 1) % size, 11, MPI_COMM_WORLD, &status);
	} else {
		// every other processes
		// only accept message from prev process in ring
		MPI_Recv(&messageReceived, 1, MPI_INT, (rank - 1) % size, 11, MPI_COMM_WORLD, &status);

		// send to the next process
		MPI_Send(&messageReceived, 1, MPI_INT, (rank + 1) % size, 11, MPI_COMM_WORLD);
	}

	// print out the result
	printf("Process %d received %d\n", rank, initialization + messageReceived);
}

int main(int argc, char** argv)
{
	int size, rank;

	// init mpi
	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	// send ring
	if (rank == 0) {
		int MESSAGE_TO_SEND = 10;
		processWork(&MESSAGE_TO_SEND, rank, size);
	} else
		processWork(NULL, rank, size);

	// finish mpi
	MPI_Finalize();
	return 0;
}
