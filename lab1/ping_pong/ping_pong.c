#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

int isStop = 0;

void signalHandler(int signo)
{
	if (signo == SIGINT) {
		isStop = 1;
		printf("Stopping the program\n");
	}
}

void processZero(int sleepTime)
{
	const char* SEND_MESSAGE = "PING";
	const int RECIPIENT = 1;

	while (!isStop) {
		// send message to recipient, i.e process 1
		MPI_Send(SEND_MESSAGE, 5, MPI_CHAR, RECIPIENT, 11, MPI_COMM_WORLD);
		printf("\nSent %s to process 1", SEND_MESSAGE);

		// prepare buffer to store the message
		char buffer[128];
		MPI_Status status;
		MPI_Recv(buffer, 5, MPI_CHAR, RECIPIENT, 11, MPI_COMM_WORLD, &status);
		printf("\nReceived %s from process 1\n", buffer);

		// sleep for sleep time
		sleep(sleepTime);
	}

	printf("\nProcess 0 stopping");
}

void processOne(int sleepTime)
{
	const char* SEND_MESSAGE = "PONG";
	const int RECIPIENT = 0;

	while (!isStop) {
		// prepare buffer to store the message
		char buffer[128];
		MPI_Status status;
		MPI_Recv(buffer, 5, MPI_CHAR, RECIPIENT, 11, MPI_COMM_WORLD, &status);

		// send message to recipient, i.e process 1
		MPI_Send(SEND_MESSAGE, 5, MPI_CHAR, RECIPIENT, 11, MPI_COMM_WORLD);

		// sleep for sleep time
		sleep(sleepTime);
	}

	printf("\nProcess 1 stopping");
}

int main(int argc, char** argv)
{
	const int SLEEP_TIME = 5;
	int rank, size;

	// register SIGINT handler, i.e Ctrl + C
	if (signal(SIGINT, signalHandler) == SIG_ERR)
		printf("Cannot use Ctrl+C to stop the program");
	// Init mpi
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);

	// begin communicate between
	// process 0 and 1
	if (rank == 0) {
		processZero(SLEEP_TIME);
	} else if (rank == 1) {
		processOne(SLEEP_TIME);
	}

	// finish mpi
	MPI_Finalize();
	return 0;
}
