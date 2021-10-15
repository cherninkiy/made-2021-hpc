#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>
#include <time.h>
#include "matvec.h"

void ZeroVector(double* v, size_t N)
{
    for(size_t i = 0; i < N; i++)
        v[i] = 0.0;
}

void RandomVector(double* v, size_t N)
{
    srand(time(NULL));

    for(size_t i = 0; i < N; i++)
        v[i] = rand() / RAND_MAX;
}

void CalcMatVecMul(const double* A, const double* u, double* v, size_t N)
{
    ZeroVector(&v[0], N);

    size_t i, j;
    for (i = 0; i < N; i++)
    {
        for(j = 0; j < N; j++)
        {
            v[i] = v[i] + A[i * N + j] * u[j];
        }
    }
}
