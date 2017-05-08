#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

#define NRA 1000
#define NCA 1000
#define NCB 1000
#define MASTER 0
#define FROM_MASTER 1
#define FROM_WORKER 2

double a[NRA][NCA],
	   b[NCA][NCB],
	   c[NRA][NCB];

int main(int argc, char** argv)
{
	int numTasks,
		taskId,
		numWorkers,
		source,
		dest,
		mType,
		rows,
		avgRow, extra, offset,
		i, j, k, rc;
	double start, end;
	MPI_Status status;

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &taskId);
	MPI_Comm_size(MPI_COMM_WORLD, &numTasks);

	if (numTasks < 2) {
		printf("Need at least two MPI tasks. Quitting...\n");
		MPI_Abort(MPI_COMM_WORLD, rc);
		exit(1);
	}

	numWorkers = numTasks - 1;

	if (taskId == MASTER) {
		printf("mpi has started with %d tasks.\n", numTasks);
		printf("Initializing arrays...\n");
		for (i = 0; i < NRA; ++i)
			for (j = 0; j < NCA; ++j)
				a[i][j] = i + j;

		for (i = 0; i < NCA; ++i)
			for (j = 0; j < NCB; ++j)
				b[i][j] = i * j;

		start = MPI_Wtime();
		avgRow = NRA / numWorkers;
		extra = NRA % numWorkers;
		offset = 0;
		mType = FROM_MASTER;
		// send tasks to workers
		for (dest = 1; dest <= numWorkers; ++dest) {
			rows = (dest <= extra) ? avgRow + 1 : avgRow;
			printf("Sending %d rows to task %d offset=%d\n", rows, dest, offset);
			MPI_Send(&offset, 1, MPI_INT, dest, mType, MPI_COMM_WORLD);
			MPI_Send(&rows, 1, MPI_INT, dest, mType, MPI_COMM_WORLD);
			MPI_Send(&a[offset][0], rows * NCA, MPI_DOUBLE, dest, mType, MPI_COMM_WORLD);
			MPI_Send(&b, NCA * NCB, MPI_DOUBLE, dest, mType, MPI_COMM_WORLD);
			offset = offset + rows;
		}

		mType = FROM_WORKER;
		// receive tasks from workers
		for (i = 1; i <= numWorkers; ++i) {
			source = i;
			MPI_Recv(&offset, 1, MPI_INT, source, mType, MPI_COMM_WORLD, &status);
			MPI_Recv(&rows, 1, MPI_INT, source, mType, MPI_COMM_WORLD, &status);
			MPI_Recv(&c[offset][0], rows * NCB, MPI_DOUBLE, source, mType, MPI_COMM_WORLD, &status);
			printf("Received results from task %d\n", source);
		}

		end = MPI_Wtime();

		// print result
		printf("Computing time = %1f \n", end - start);
		printf("Result matrix:\n");
		for (i = 0; i < 10; ++i) {
			printf("\n");
			for (j = 0; j < 10; ++j)
				printf("%6.2f   ", c[i][j]);
		}
		printf("Done\n");
	} else {
		mType = FROM_MASTER;
		MPI_Recv(&offset, 1, MPI_INT, MASTER, mType, MPI_COMM_WORLD, &status);
		MPI_Recv(&rows, 1, MPI_INT, MASTER, mType, MPI_COMM_WORLD, &status);
		MPI_Recv(&a, rows * NCA, MPI_DOUBLE, MASTER, mType, MPI_COMM_WORLD, &status);
		MPI_Recv(&b, NCA * NCB, MPI_DOUBLE, MASTER, mType, MPI_COMM_WORLD, &status);

		for (k = 0; k < NCB; ++k)
			for (i = 0; i < rows; ++i) {
				c[i][k] = 0.0;
				for (j = 0; j < NCA; ++j)
					c[i][k] = c[i][k] + a[i][j] * b[j][k];
			}

		mType = FROM_WORKER;
		MPI_Send(&offset, 1, MPI_INT, MASTER, mType, MPI_COMM_WORLD);
		MPI_Send(&rows, 1, MPI_INT, MASTER, mType, MPI_COMM_WORLD);
		MPI_Send(&c, rows * NCB, MPI_DOUBLE, MASTER, mType, MPI_COMM_WORLD);
	}

	MPI_Finalize();
	return 0;
}
