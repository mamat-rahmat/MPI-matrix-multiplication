#include <stdio.h>
#include <stdlib.h>
#include "mpi.h"


#define N 4

/* global */
/*
int a[N][N] = {
    { 1, 2, 3, 4 },
    { 5, 6, 7, 8 },
    { 9, 10, 11, 12 },
    { 13, 14, 15, 16 } 
};

int b[N][N] = {
    { 17, 18, 19, 20 },
    { 21, 22, 23, 24 },
    { 25, 26, 27, 28 },
    { 29, 30, 31, 32 },
};

int c[N][N];
*/

int a[N*N], b[N*N], c[N*N];

int main(int argc, char *argv[])
{
	int rank, size, i, j, k;
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);

	if(rank == 0)
	{
		for(i=0; i<N; i++)
			for(j=0; j<N; j++)
				scanf("%d", a+(i*N)+j);

		for(i=0; i<N; i++)
			for(j=0; j<N; j++)
				scanf("%d", b+(i*N)+j);
	}

	//elements per process
	int epp = N*N/size;

	// create a buffer that will hold the rows
	int *row = malloc(sizeof(int) * epp);
	// create a buffer that will hold the result
	int *res = malloc(sizeof(int) * epp);


	MPI_Scatter(a, epp, MPI_INT, row, epp, MPI_INT, 0, MPI_COMM_WORLD);
	MPI_Bcast(b, N*N, MPI_INT, 0, MPI_COMM_WORLD);

	for(i=0; i<epp/N; i++)
		for(j=0; j<N; j++);
		{
			res[i*N+j] = 0;
			for(k=0; k<N; k++)
				res[i*N+j] += row[i*N + k] * b[k*N + j];
		}

	MPI_Gather(res, epp, MPI_INT, c, epp, MPI_INT, 0, MPI_COMM_WORLD);

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