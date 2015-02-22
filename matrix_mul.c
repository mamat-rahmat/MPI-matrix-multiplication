#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include "mpi.h"

float diffclock(clock_t clock1,clock_t clock2) //penghitungan waktu
{
    float diffticks=clock1-clock2;
    float diffms=(diffticks)/(CLOCKS_PER_SEC/1000);
    return diffms;
}

int main(int argc, char *argv[])
{
	if (argc != 2) {
		fprintf(stderr, "Usage: matrixmpi <spasi> ukuan_matriks\n");
		exit(1);
	}
	int N = atoi(argv[1]);
	
	int *a = NULL;
	int *b = NULL;
	int *c = NULL;

	int rank, size;
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);

	int i, j, k;
	double x_milliseconds=0.0;
	clock_t x_startTime,x_countTime;
	
	if(rank == 0)
	{
		system("clear");
		srand(time(NULL));
		printf("ukuran matriks: %d\n", N);
		MPI_Bcast(&N, 1, MPI_INT, 0, MPI_COMM_WORLD);

		a = malloc(N*N*sizeof(int));
		for(i=0; i<N; i++)
			for(j=0; j<N; j++)
				a[i*N + j] = (rand()%19)-9;
		
		b = malloc(N*N*sizeof(int));
		for(i=0; i<N; i++)
			for(j=0; j<N; j++)
				b[i*N + j] = (rand()%19)-9;

		c = malloc(N*N*sizeof(int));

		printf("matriks A:\n");
		for(i=0; i<N; i++)
		{
			for(j=0; j<N; j++)
				printf("%d ", a[(i*N)+j]);
			printf("\n");
		}
		printf("\n");
		printf("matriks B:\n");
		for(i=0; i<N; i++)
		{
			for(j=0; j<N; j++)
				printf("%d ", b[(i*N)+j]);
			printf("\n");
		}
		printf("\n");
		x_startTime=clock();
	}
	else
	{
		MPI_Bcast(&N, 1, MPI_INT, 0, MPI_COMM_WORLD);
		b = malloc(N*N*sizeof(int));
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
		x_countTime=clock();
		x_milliseconds=diffclock(x_countTime,x_startTime);
		printf("hasil perkalian matriks AxB:\n");
		for(i=0; i<N; i++)
		{
			for(j=0; j<N; j++)
				printf("%d ", c[(i*N)+j]);
			printf("\n");
		}
		printf("waktu perhitungan: %f milisecond(s)\n", x_milliseconds);
	}

	MPI_Finalize();
	return 0;
}