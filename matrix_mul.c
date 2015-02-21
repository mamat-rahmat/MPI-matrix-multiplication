#include <stdio.h>
#include <stdlib.h>
#include "mpi.h"


#define N 4

int main(int argc, char *argv[])
{
	int* a = NULL;
	int *b = NULL;
	int *c = NULL;

	int rank, size;
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);

	int i, j, k;

	b = malloc(N*N*sizeof(int));
	if(rank == 0)
	{
		a = malloc(N*N*sizeof(int));
		for(i=0; i<N; i++)
			for(j=0; j<N; j++)
				scanf("%d", a+(i*N)+j);

		for(i=0; i<N; i++)
			for(j=0; j<N; j++)
				scanf("%d", b+(i*N)+j);

		c = malloc(N*N*sizeof(int));
	}

	//elements per process
	int epp = N*N/size;

	// create a buffer that will hold the rows
	int *row = malloc(sizeof(int) * epp);

	MPI_Scatter(a, epp, MPI_INT, row, epp, MPI_INT, 0, MPI_COMM_WORLD);
	MPI_Bcast(b, N*N, MPI_INT, 0, MPI_COMM_WORLD);

	// create a buffer that will hold the result
	int *res = malloc(sizeof(int) * epp);
	for(i=0; i<epp/N; i++)
		for(j=0; j<N; j++)
		{
			res[i*N+j] = 0;
			for(k=0; k<N; k++)
				res[i*N+j] += row[i*N + k] * b[k*N + j];
		}

	MPI_Gather(res, epp, MPI_INT, c, epp, MPI_INT, 0, MPI_COMM_WORLD);

	//synchronize
	MPI_Barrier(MPI_COMM_WORLD);

	if(rank == 0)
	{
		for(i=0; i<N; i++)
		{
			for(j=0; j<N; j++)
				printf("%d ", c[(i*N)+j]);
			printf("\n");
		}
	}

	MPI_Finalize();
	return 0;
}