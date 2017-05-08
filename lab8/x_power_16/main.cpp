#include <mpi.h>
#include <iostream>

const int EXPONENTIAL = 16;
const int x = 2;

int multiply(int start, int x, int times)
{
	for (int i = 0; i < times; ++i)
		start *= x;
	return start;
}

int main(int argc, char** argv)
{
	int rank, size;
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);

	int multiplcationTimesPerProcess = EXPONENTIAL / size;
	int lastProcessMultiplicationTimes = EXPONENTIAL - (size - 1) * multiplcationTimesPerProcess;
	int result = 1;
	MPI_Status status;

	if (rank == 0) {
		// first process
		result = multiply(1, x, multiplcationTimesPerProcess);

		// send the result to the next process
		MPI_Send(&result, 1, MPI_INT, 1, 0, MPI_COMM_WORLD);
	} else if (rank < size - 1) {
		// middle processes
		// receive the result from previous process
		MPI_Recv(&result, 1, MPI_INT, rank - 1, 0, MPI_COMM_WORLD, &status);

		result = multiply(result, x, multiplcationTimesPerProcess);

		// send the result to the next process
		MPI_Send(&result, 1, MPI_INT, rank + 1, 0, MPI_COMM_WORLD);
	} else if (rank == size - 1) {
		// last process
		// receive the result from the previous process
		MPI_Recv(&result, 1, MPI_INT, rank - 1, 0, MPI_COMM_WORLD, &status);

		result = multiply(result, x, lastProcessMultiplicationTimes);

		// print the result
		std::cout << "Final result is " << result << std::endl;
	}

	MPI_Finalize();
	return 0;
}
