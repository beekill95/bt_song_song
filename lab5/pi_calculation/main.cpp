#include <mpi.h>
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <cmath>

const int NUM_RANDOM_POINTS = 2000000000;
const double CIRCLE_RADIUS = 1.0;

inline double randomFromMinusOneToOne()
{
	return rand() * 2.0 / RAND_MAX - 1;
}

void master(int numSlaves)
{
	MPI_Status status;

	// distribute the number of random points to each slave
	int numPointPerSlave = NUM_RANDOM_POINTS / numSlaves;
	for (int i = 0; i < numSlaves - 1; ++i) {
		MPI_Send(&numPointPerSlave, 1, MPI_INT, i + 1, 0, MPI_COMM_WORLD);
	}
	int numPointLastSlave = NUM_RANDOM_POINTS - numPointPerSlave * (numSlaves - 1);
	MPI_Send(&numPointLastSlave, 1, MPI_INT, numSlaves, 0, MPI_COMM_WORLD);

	// receive the result from each slave
	int totalPointInsideCircle = 0;
	for (int i = 0; i < numSlaves; ++i) {
		int pointInsideCircle;
		MPI_Recv(&pointInsideCircle, 1, MPI_INT, i + 1, 0, MPI_COMM_WORLD, &status); 

		totalPointInsideCircle += pointInsideCircle;
	}

	// calculate the number PI and output the result
	double pi = 4.0 * totalPointInsideCircle / NUM_RANDOM_POINTS;
	std::cout.precision(10);
	std::cout 
		<< "Tinh toan duoc so PI la: " << pi 
		<< "\nVoi do sai so la: " << std::abs(M_PI - pi) << std::endl;
}

void slave()
{
	MPI_Status status;
	
	// receive number of points to random
	int numRandomPoints;
	MPI_Recv(&numRandomPoints, 1, MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

	// generate random points and check whether the generated points is inside the circle
	int numPointInsideCircle = 0;
	for (int i = 0; i < numRandomPoints; ++i) {
		double x = randomFromMinusOneToOne();
		double y = randomFromMinusOneToOne();

		if (x * x + y * y <= CIRCLE_RADIUS)
			numPointInsideCircle++;	
	}

	// return the number of inside-circle points to master
	MPI_Send(&numPointInsideCircle, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
}

int main(int argc, char** argv)
{
	int size, rank;
	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	if (rank == 0) {
		master(size - 1);
	} else {
		srand(time(0));
		slave();
	}

	MPI_Finalize();
	return 0;
}
