/*
 * main.c
 *
 *  Created on: 04/11/2013
 *      Author: Thiago
 */
#include <mpi.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#include "matrix_utils.h"

/* Globals */
int gRows;
int gColumns;
int gRow;
int gColumn;
int gMaxRank;
int gSrank;
int gLsize;
int gDim[2];
int gCoord[2];
double** gMatrix;
double** gSubMatrix;
double* gDiagonal;
int gRank;
int gSize;
MPI_Comm gCartComm;

int main(int argc, char** argv)
{
	int i, j;

	gMatrix = loadMatrix("matrix.m");

	if ( gMatrix != NULL )
	{

		if ( gRows % gColumns != 0 )
		{
			printf("ERRO: A matriz n\706o \202 quadrada\n");
			exit(1);
		}

		else
		{
			MPI_Init(&argc, &argv);
			MPI_Comm_rank(MPI_COMM_WORLD, &gRank);
			MPI_Comm_size(MPI_COMM_WORLD, &gSize);

			gLsize = floor(sqrt(gSize));

			gMaxRank = (int) ( powf(gLsize, 2) - 1 );

			if  ( gMaxRank + 1 != gSize )
			{
				printf("ERRO: N\227mero de processos precisa ser um quadrado perfeito.\n");
				MPI_Finalize();
				exit(1);
			}

			int lPeriod[] = { 0, 0 };

			MPI_Comm lRowComm;
			MPI_Comm lColumnComm;

			int lRowOnly[] = {0, 1};
			int lColumnOnly[] = {1, 0};

			gDim[0] = gLsize;
			gDim[1] = gLsize;

			// Comunicadores
			MPI_Cart_create(MPI_COMM_WORLD, 2, gDim, lPeriod, 0, &gCartComm);
			MPI_Cart_coords(gCartComm, gRank, 2, gCoord);

			printf("%d|%d Linha %d, Coluna %d [Cartesian Communicator].\n", gRank, gSize, gCoord[0], gCoord[1]);

			gRow = gCoord[0];
			gColumn = gCoord[1];

			gSrank = gRows / gLsize;

			printf("%d|%d Lendo Rank %d matrix.\n", gRank, gSize, gSrank);

			gSubMatrix = getSubMatrix(gMatrix, gRows, gSrank, gRow, gColumn);

			// Comunicadores para linha e coluna
			MPI_Cart_sub(gCartComm, lRowOnly, &lRowComm);
			MPI_Cart_sub(gCartComm, lColumnOnly, &lColumnComm);

			// Broadcasting do vetor contendo a diagonal da matriz
			gDiagonal = malloc(gSrank * sizeof(double));

			if ( gRow == gColumn )
			{
				for ( i = 0 ; i < gSrank ; i++ )
				{
					gDiagonal[i] = gSubMatrix[i][i];
				}
			}

			int lDholder[] = { gRow };

			int lCastRank;

			MPI_Cart_rank(lRowComm, lDholder, &lCastRank);

			if ( gColumn == gRow )
			{
				printf("%d|%d Fazendo broadcast\n", gRank, gSize);
			}

			else
			{
				printf("%d|%d Recebendo broadcast\n", gRank, gSize);
			}

			MPI_Bcast(gDiagonal, gSrank, MPI_DOUBLE, lCastRank, lRowComm);

			if ( gColumn != gRow )
			{
				printf("%d|%d Diagonais: \n", gRank, gSize);
			}

			for ( i = 0 ; i < gSrank ; i++ )
			{
				printf("%d|%d Diagonal %d: %lf\n", gRank, gSize, i, gDiagonal[i]);
			}

			// Multiplicação da matrix pelos elementos da diagonal
			for ( i = 0 ; i < gSrank ; i++ )
			{
				for ( j = 0 ; j < gSrank ; j++ )
				{
					gSubMatrix[i][j] *= gDiagonal[i];
				}
			}

			// Soma dos elementos por coluna
			double* lColumnSum = malloc(gSrank * sizeof(double));
			double* gColumnSum = malloc(gSrank * sizeof(double));

			for ( i = 0 ; i < gSrank ; i++ )
			{
				for ( j = 0 ; j < gSrank ; j++ )
				{
					lColumnSum[j] += gSubMatrix[i][j];
				}
			}

			MPI_Reduce(lColumnSum, gColumnSum, gSrank, MPI_DOUBLE, MPI_SUM, 0, lColumnComm);

			if ( gRow == 0 )
			{
				for ( j = 0 ; j < gSrank ; j++ )
				{
					printf("%d|%d Soma Parcial da Coluna %d: %lf\n", gRank, gSize, j, gColumnSum[j]);
				}
			}

			double lSum = 0;
			double gSum = 0;

			for ( i = 0 ; i < gSrank; i++ )
			{
				lSum += gColumnSum[i];
			}

			MPI_Reduce(&lSum, &gSum, 1, MPI_DOUBLE, MPI_SUM, 0, lRowComm);

			if ( gRank == 0 )
			{
				printf("\nSoma Total: %lf\n", gSum);
			}

			MPI_Finalize();

			// Liberando memória
			free(gMatrix);
			free(gDiagonal);
		}
	}

	else
	{
		printf("ERRO: Matriz n\706o carregada!!\n");
		MPI_Finalize();
		exit(1);
	}
}
