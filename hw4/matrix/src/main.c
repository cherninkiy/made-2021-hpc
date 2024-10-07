#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>
#include <time.h>
#include <omp.h>
#include <cblas.h>
#include "matmul.h"
#include "matpower.h"


int Usage()
{
    printf("Usage: matpower -t [threads] -r [runs] -n [size] -p [power] -i [num] [comment]\n");
    printf("\n");
    printf("-t [threads]    number of runs\n");
    printf("-r [runs]       number of runs\n");
    printf("-n [size]       size of the matrix\n");
    printf("-p [power]      power exponent\n");
    printf("-i [num]        matmul implementation:\n");
    printf("                1 - ijk ordering\n");
    printf("                2 - jik ordering\n");
    printf("                3 - kij ordering\n");
    printf("                4 - kij, naive optimization\n");
    printf("                5 - cblas_dgemm optimization\n");
    printf("                6 - shtrassen algorithm\n");
    printf("\n");
    return 1;
}

double BenchmarkPower(MatMulFunc func, const double* A, double* C, size_t N, int p)
{
    struct timeval start, end;
    double r_time;

    gettimeofday(&start, NULL);
    
    CalcMatPower(A, C, N, p, func);

    gettimeofday(&end, NULL);
    
    r_time = end.tv_sec - start.tv_sec + ((double) (end.tv_usec - start.tv_usec)) / 1000000;
    return r_time;
}

int main(int argc, char** argv)
{
    if (argc < 11)
        return Usage();

    int n_threads = 2;
    if (sscanf(argv[2], "%d", &n_threads) != 1)
        return Usage();

    int n_runs = 3;
    if (sscanf(argv[4], "%d", &n_runs) != 1)
        return Usage();

    size_t N = 512;
    if (sscanf(argv[6], "%zu", &N) != 1)
        return Usage();

    int p = 2;
    if (sscanf(argv[8], "%d", &p) != 1)
        return Usage();

    size_t index = 1;
    if (sscanf(argv[10], "%zu", &index) != 1)
        return Usage();
    index -= 1;

    MatMulFunc funcs[] = {
        CalcMatMul_ijk,
        CalcMatMul_jik,
        CalcMatMul_kij,
        CalcMatMul_kij_opt,
        CalcMatMul_blas,
        CalcMatMul_Shtrassen
    };

    char* names[] = {
        "ijk ordering",
        "jik ordering",
        "kij ordering",
        "kij, naive optimization",
        "cblas_dgemm optimization",
        "shtrassen algorithm",
    };

    double *runtimes;
    double average_runtime;
    double *A, *B, *C;
    
    omp_set_num_threads(n_threads);
    printf("Number of threads %d\n", n_threads);

    printf("Number of runs %d\n", n_runs);
    printf("Matrix size %zu\n", N);

    A = (double*)malloc(N * N * sizeof(double));
    C = (double*)malloc(N * N * sizeof(double));
    runtimes = (double *)malloc(n_runs * sizeof(double));

    RandomMatrix(&A[0], N);

    printf("MatPower - %s\n", names[index]);

    average_runtime = 0.0;
    for(int n = 0; n < n_runs; n++)
    {
        runtimes[n] = BenchmarkPower(funcs[index], &A[0], &C[0], N, p);
        printf("runtime\t%s\t%lf\t%zu\t%d\t%d\n", names[index], runtimes[n], N, p, n_threads);
        average_runtime += runtimes[n]/n_runs;
    }

    printf("average runtime %lf seconds\n", average_runtime);
    printf("---------------------------------\n");

    free(A);
    free(C);
    free(runtimes);
    return 0;
}