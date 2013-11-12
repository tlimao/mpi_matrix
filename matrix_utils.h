/*
 * matrix_utils.h
 *
 *  Created on: 04/11/2013
 *      Author: Thiago
 */

#ifndef MATRIX_UTILS_H_
#define MATRIX_UTILS_H_

#include "stdio.h"
#include <stdlib.h>

double** loadMatrix(char* pFileName);

double** getSubMatrix(double** pMatrix, int pMrank, int pSrank, int pRow, int pColumn);

#endif /* MATRIX_UTILS_H_ */
