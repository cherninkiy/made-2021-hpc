#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>
#include <time.h>
#include <cblas.h>

#include "matmul.h"

void ZeroMatrix(double* A, size_t N)
{
    size_t i, j;

    for(i=0; i<N; i++)
    {
        for(j=0; j<N; j++)
        {
            A[i*N + j] = 0.0;
        }
    }
}

void RandomMatrix(double* A, size_t N)
{
    srand(time(NULL));

    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < N; j++)
        {
            A[i * N + j] = (double)rand() / RAND_MAX;
        }
    }
}

void CalcMatMul_ijk(const double* A, const double* B, double* C, size_t N)
{
    ZeroMatrix(&C[0], N);

    size_t i, j, k;

    #pragma omp parallel for private(i, j, k) shared(A, B, C)
    for (i = 0; i < N; i++)
    {
        for(j = 0; j < N; j++)
        {
            for(k = 0; k < N; k++)
                C[i * N + j] = C[i * N + j] + A[i * N + k] * B[k * N + j];
        }
    }
}

void CalcMatMul_jik(const double* A, const double* B, double* C, size_t N)
{
    size_t i, j, k;

    ZeroMatrix(&C[0], N);

    #pragma omp parallel for private(i, j, k) shared(A, B, C)
    for (j = 0; j < N; j++)
    {
        for(i = 0; i < N; i++)
        {
            for(k = 0; k < N; k++)
                C[i * N + j] = C[i * N + j] + A[i * N + k] * B[k * N + j];
        }
    }
}

void CalcMatMul_kij(const double* A, const double* B, double* C, size_t N)
{
    size_t i, j, k;

    ZeroMatrix(&C[0], N);

    #pragma omp parallel for private(i, j, k) shared(A, B, C)
    for (k = 0; k < N; k++)
    {
        for(i = 0; i < N; i++)
        {
            for(j = 0; j < N; j++)
                C[i * N + j] = C[i * N + j] + A[i * N + k] * B[k * N + j];
        }
    }
}

void CalcMatMul_kij_opt(const double* A, const double* B, double* C, size_t N)
{
    size_t i, j, k;
    size_t dummy = 0;

    ZeroMatrix(&C[0], N);

    #pragma omp parallel for private(i, j, k) shared(A, B, C)
    for (k = 0; k < N; k++)
    {
        for(i = 0; i < N; i++)
        {
            dummy = i * N;
            for(j = 0; j < N; j++)
                C[dummy + j] = C[dummy + j] + A[dummy + k] * B[k * N + j];
        }
    }
}

void CalcMatMul_blas(const double* A, const double* B, double* C, size_t N)
{
    cblas_dgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans,
        N, N, N, 1.0, A, N, B, N, 0.0, C, N);
}

void CalcMatSum(const double* A, const double* B, double* C, size_t N)
{
    size_t i, j;

    #pragma omp parallel for private(i, j) shared(A, B, C)
    for (i = 0; i < N; i++)
    {
        for(j = 0; j < N; j++)
        {
            C[i * N + j] = A[i * N + j] + B[i * N + j];
        }
    }
}

void CalcMatSubt(const double* A, const double* B, double* C, size_t N)
{
    size_t i, j;

    #pragma omp parallel for private(i, j) shared(A, B, C)
    for (i = 0; i < N; i++)
    {
        for(j = 0; j < N; j++)
        {
            C[i * N + j] = A[i * N + j] - B[i * N + j];
        }
    }
}

size_t NextPowerOfTwo(size_t N)
{
    size_t i = 1;
    while(i < N)
    {
        i*= 2;
    }
    return i;
}

void AlignMatrix(const double* A, size_t N, double* D, size_t Nnew)
{
    size_t i, j;

    #pragma omp parallel for private(i, j) shared(A, D)
    for (i = 0; i < N; i++)
    {
        for(j = 0; j < N; j++)
        {
            D[i * N + j] = A[i * N + j];
        }
    }

    for (i = N; i < Nnew; i++)
        D[i * N + i] = 1;
}

void SplitMatrix(const double* A, size_t N, double* A11, double* A12, double* A21, double* A22, size_t Nnew)
{
    size_t i, j;

    #pragma omp parallel for private(i, j) shared(A, A11, A12, A21, A22)
    for (i = 0; i < Nnew; i++)
    {
        for(j = 0; j < Nnew; j++)
        {
            A11[i * Nnew + j] = A[i * N + j];
            A12[i * Nnew + j] = A[i * N + j + Nnew];
            A21[i * Nnew + j] = A[(i + Nnew) * N + j];
            A22[i * Nnew + j] = A[(i + Nnew) * N + j + Nnew];
        }
    }
}

void JoinMatrix(const double* A11, const double* A12, const double* A21, const double* A22, size_t N, double* A)
{
    size_t Nnew = N*2;

    size_t i, j;

    #pragma omp parallel for private(i, j) shared(A, A11, A12, A21, A22)
    for (i = 0; i < N; i++)
    {
        for (j = 0; j < N; j++)
        {
            A[i * Nnew + j] = A11[i * N + j];
            A[i * Nnew + j + N] = A12[i * N + j];
            A[(i + N) * Nnew + j] = A21[i * N + j];
            A[(i + N) * Nnew + j + N] = A22[i * N + j];
        }
    }
}

void MultiplyShtrassen(const double* A, const double* B, double* C, size_t N)
{
    double *A11, *A12, *A21, *A22;
    double *B11, *B12, *B21, *B22;
    double *S1, *S2;
    double *P1, *P2, *P3, *P4, *P5, *P6, *P7;
    double *C11, *C12, *C21, *C22;

    size_t Nnew = N / 2;
    A11 = (double*)calloc(sizeof(double), Nnew * Nnew);
    A12 = (double*)calloc(sizeof(double), Nnew * Nnew);
    A21 = (double*)calloc(sizeof(double), Nnew * Nnew);
    A22 = (double*)calloc(sizeof(double), Nnew * Nnew);
    SplitMatrix(A, N, A11, A12, A21, A22, Nnew);

    B11 = (double*)calloc(sizeof(double), Nnew * Nnew);
    B12 = (double*)calloc(sizeof(double), Nnew * Nnew);
    B21 = (double*)calloc(sizeof(double), Nnew * Nnew);
    B22 = (double*)calloc(sizeof(double), Nnew * Nnew);
    SplitMatrix(B, N, B11, B12, B21, B22, Nnew);

    MatMulFunc MulFunc;
    MulFunc = (Nnew <= 128) ? CalcMatMul_kij : CalcMatMul_Shtrassen;

    S1 = (double*)calloc(sizeof(double), Nnew * Nnew);
    S2 = (double*)calloc(sizeof(double), Nnew * Nnew);

    // P1 = (A11 + A22)(B11 + B22)
    P1 = (double*)calloc(sizeof(double), Nnew * Nnew);
    CalcMatSum(A11, A22, S1, Nnew);
    CalcMatSum(B11, B22, S2, Nnew);
    MulFunc(S1, S2, P1, Nnew);

    // P2 = (A21 + A22)*B11
    P2 = (double*)calloc(sizeof(double), Nnew * Nnew);
    CalcMatSum(A21, A22, S1, Nnew);
    MulFunc(S1, B22, P2, Nnew);

    // P3 = A11*(B12 - B22)
    P3 = (double*)calloc(sizeof(double), Nnew * Nnew);
    CalcMatSubt(B12, B22, S1, Nnew);
    MulFunc(A11, S1, P3, Nnew);

    // P4 = A22*(B21 - B11)
    P4 = (double*)calloc(sizeof(double), Nnew * Nnew);
    CalcMatSubt(B21, B11, S1, Nnew);
    MulFunc(A22, S1, P4, Nnew);

    // P5 = (A11 + A12)*B22
    P5 = (double*)calloc(sizeof(double), Nnew * Nnew);
    CalcMatSubt(A11, A12, S1, Nnew);
    MulFunc(S1, B22, P5, Nnew);

    // P6 = (A21 - A11)(B11 + B12)
    P6 = (double*)calloc(sizeof(double), Nnew * Nnew);
    CalcMatSubt(A21, A11, S1, Nnew);
    CalcMatSum(B11, B12, S2, Nnew);
    MulFunc(S1, S2, P6, Nnew);

    // P7 = (A12 - A22)(B21 + B22)
    P7 = (double*)calloc(sizeof(double), Nnew * Nnew);
    CalcMatSubt(A12, A22, S1, Nnew);
    CalcMatSum(B21, B22, S2, Nnew);
    MulFunc(S1, S2, P6, Nnew);

    // C11 = P1 + P4 - P5 + P7
    C11 = (double*)calloc(sizeof(double), Nnew * Nnew);
    CalcMatSum(P1, P4, S1, Nnew);
    CalcMatSubt(S1, P5, S2, Nnew);
    CalcMatSum(S2, P7, C11, Nnew);

    // C12 = P3 + P5
    C12 = (double*)calloc(sizeof(double), Nnew * Nnew);
    CalcMatSum(P3, P5, C12, Nnew);

    // C21 = P2 + P4
    C21 = (double*)calloc(sizeof(double), Nnew * Nnew);
    CalcMatSum(P2, P4, C21, Nnew);

    // C22 = P1 - P2 + P3 + P6
    C22 = (double*)calloc(sizeof(double), Nnew * Nnew);
    CalcMatSubt(P1, P2, S1, Nnew);
    CalcMatSum(S1, P3, S2, Nnew);
    CalcMatSum(S2, P6, C22, Nnew);

    JoinMatrix(C11, C12, C21, C22, Nnew, C);

    free(S1);
    free(S2);

    free(P1);
    free(P2);
    free(P3);
    free(P4);
    free(P5);
    free(P6);
    free(P7);

    free(A11);
    free(A12);
    free(A21);
    free(A22);

    free(B11);
    free(B12);
    free(B21);
    free(B22);

    free(C11);
    free(C12);
    free(C21);
    free(C22);
}

void CalcMatMul_Shtrassen(const double* A, const double* B, double* C, size_t N)
{
    size_t Nnew = NextPowerOfTwo(N);

    double *Anew, *Bnew, *Cnew;

    Anew = (double*)calloc(sizeof(double), Nnew * Nnew);
    AlignMatrix(A, N, Anew, Nnew);

    Bnew = (double*)calloc(sizeof(double), Nnew * Nnew);
    AlignMatrix(B, N, Bnew, Nnew);

    Cnew = (double*)calloc(sizeof(double), Nnew * Nnew);

    MultiplyShtrassen(Anew, Bnew, Cnew, Nnew);

    size_t i, j;

    #pragma omp parallel for private(i, j) shared(C, Cnew)
    for (i = 0; i < N; i++)
    {
        for (j = 0; j < N; j++)
        {
            C[i * N + j] = Cnew[i * Nnew + j];
        }
    }

    free(Anew);
    free(Bnew);
    free(Cnew);
}