/*
 * matrix_utils.c
 *
 *  Created on: 04/11/2013
 *      Author: Thiago
 */
#include "matrix_utils.h"

extern gRows;
extern gColumns;

double** loadMatrix(char* pFileName)
{
	int i, j;

	int lRows, lColumns;

	/* Open File */
	FILE* lFile = fopen(pFileName, "r");

	if ( lFile == NULL )
	{
		printf("Arquivo inv\240lido %s\n", pFileName);
		return NULL;
	}

	/* Reads number of matrix rows */
	fscanf(lFile, "%d", &lRows);
	gRows = lRows;

	/* Reads number of matrix columns */
	fscanf(lFile, "%d", &lColumns);
	pColumns = lColumns;

	double** lMatrix;

	lMatrix = (double**) malloc(sizeof(double*)*lRows);

	for ( i = 0 ;  i < lColumns ; i++ )
	{
		lMatrix[i] = (double*) malloc(sizeof(double)*lColumns);
	}

	for ( i = 0 ;  i < lRows ; i++ )
	{
		for  ( j = 0 ;  j <lColumns ; j++ )
		{
			fscanf(lFile, "%lf", &lMatrix[i][j]);
		}
	}

	fclose(lFile);

	return lMatrix;
}

double** getSubMatrix(double** pMatrix, int pMrank, int pSrank, int pRow, int pColumn)
{
	int i, j;

	int lII, lJJ;

	double** lSubMatrix = (double**) malloc(sizeof(double*)*pSrank);

	for ( i = 0 ; i < pSrank ; i++ )
	{
		lSubMatrix[i] = (double*) malloc(sizeof(double)*pSrank);
	}

	for ( i = 0 , lII = pRow * pSrank ; lII <= pRow * pSrank + pSrank - 1 ; i++ , lII++ )
	{
		for ( j = 0 , lJJ = pColumn * pSrank ; lJJ <= pColumn * pSrank + pSrank - 1 ; j++ , lJJ++ )
		{
			lSubMatrix[i][j] = pMatrix[lII][lJJ];
		}
	}

	return lSubMatrix;
}
