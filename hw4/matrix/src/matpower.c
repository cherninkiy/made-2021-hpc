#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include "matpower.h"


void MakeIdentity(double* A, size_t N)
{
    int i;

    for (i=0; i<N*N; i++)
        A[i] = (double)0.0;

    
    for (i=0; i<N; i++)
        A[i] = 1.0;
}

void CopyMatrix(const double* A, double* B, size_t N)
{
    int i, j;

    for (i = 0; i < N; i++)
    {
        for (j = 0; j < N; j++)
        {
            B[i * N + j] = A[i * N + j];
        }    
    }
}

void CalcMatPower(const double* A, double* C, size_t N, int p, MatMulFunc func)
{

	double* B;
    B = (double*)malloc(N * N * sizeof(double));
    MakeIdentity(&B[0], N);

	double* A2;
    A2 = (double*)malloc(N * N * sizeof(double));
    CopyMatrix(&A[0], &A2[0], N);

	double* D;
    D = (double*)malloc(N * N * sizeof(double));


    while (p)
    {
		if (p & 1)
        {   // res *= a
			func(&A[0], &B[0], &C[0], N);
            CopyMatrix(&C[0], &B[0], N);
        }

        // a *= a
        func(&A2[0], &A2[0], &D[0], N);
        CopyMatrix(&D[0], &A2[0], N);
       
		p >>= 1;
    }
}

