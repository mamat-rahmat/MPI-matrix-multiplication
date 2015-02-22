#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "mpi.h"

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
	double x_startTime,x_countTime;

	char processor_name[MPI_MAX_PROCESSOR_NAME];
	int name_len;
	MPI_Get_processor_name(processor_name, &name_len);
	printf("Hello! from processor %s, task %d of %d, \n", processor_name, rank, size);
	
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
		x_startTime = MPI_Wtime();
	}
	else
	{
		MPI_Bcast(&N, 1, MPI_INT, 0, MPI_COMM_WORLD);
	}

	//elements per process
	ncut = int(sqrt(size));
	int epp = (N*N)/ncut;

	// create a buffer that will hold the rows and columns
	int *row = malloc(sizeof(int) * epp);
	int *col = malloc(sizeof(int) * epp);

	if(rank%ncut == 0)
		MPI_Scatter(a, epp, MPI_INT, row, epp, MPI_INT, 0, MPI_COMM_WORLD);
	//synchronize
	MPI_Barrier(MPI_COMM_WORLD);
	for(i=0; i<ncut; i++)
	{
		if(size / rank == i)
			MPI_Bcast(row, epp, MPI_INT, rank-(rank%ncut), MPI_COMM_WORLD);
		//synchronize
		MPI_Barrier(MPI_COMM_WORLD);
	}


	if(rank < ncut)
		MPI_Scatter(b, epp, MPI_INT, col, epp, MPI_INT, 0, MPI_COMM_WORLD);
	//synchronize
	MPI_Barrier(MPI_COMM_WORLD);
	for(i=0; i<ncut; i++)
	{
		if(rank%ncut == i)
			MPI_Bcast(col, epp, MPI_INT, i, MPI_COMM_WORLD);
		//synchronize
		MPI_Barrier(MPI_COMM_WORLD);
	}

	// create a buffer that will hold the result
	int *res = malloc(sizeof(int) * epp);
	for(i=0; i<epp/N; i++)
		for(j=0; j<epp/N; j++)
		{
			res[i*N+j] = 0;
			for(k=0; k<N; k++)
				res[i*N+j] += row[i*N + k] * col[k*N + j];
		}

	MPI_Gather(res, epp, MPI_INT, c, epp, MPI_INT, 0, MPI_COMM_WORLD);

	//synchronize
	MPI_Barrier(MPI_COMM_WORLD);

	if(rank == 0)
	{
		x_countTime = MPI_Wtime();
		x_milliseconds = (x_countTime-x_startTime)*1000;
		printf("hasil perkalian matriks AxB:\n");
		for(i=0; i<N; i++)
		{
			for(j=0; j<N; j++)
				printf("%d ", c[(i*N)+j]);
			printf("\n");
		}
		printf("waktu perhitungan: %f milisecond(s)\n", x_milliseconds);
		printf("jumlah elemen per proses: %d\n", epp);
	}

	MPI_Finalize();
	return 0;
}