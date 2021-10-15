#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>
#include <time.h>
#include "matmul.h"
#include "matvec.h"


int Usage()
{
    printf("Usage: matmul -r [runs] -n [size] -i [num] [comment]\n");
    printf("\n");
    printf("-r [runs]   number of runs\n");
    printf("-n [size]   size of the matricies\n");
    printf("-i [num]    implementation:\n");
    printf("            1 - ijk ordering\n");
    printf("            2 - jik ordering\n");
    printf("            3 - kij ordering\n");
    printf("            4 - kij, naive optimization\n");
    printf("            5 - cblas_dgemm optimization\n");
    printf("            6 - shtrassen algorithm\n");
    printf("\n");
    return 1;
}

double Benchmark(MatMulFunc func, const double* A, const double* B, double* C, size_t N)
{
    struct timeval start, end;
    double r_time;

    gettimeofday(&start, NULL);
    
    func(A, B, C, N);

    gettimeofday(&end, NULL);
    
    r_time = end.tv_sec - start.tv_sec + ((double) (end.tv_usec - start.tv_usec)) / 1000000;
    return r_time;
}

int main(int argc, char** argv)
{
    if (argc < 7)
        return Usage();

    int n_runs = 3;
    if (sscanf(argv[2], "%d", &n_runs) != 1)
        return Usage();

    size_t N = 512;
    if (sscanf(argv[4], "%zu", &N) != 1)
        return Usage();

    size_t index = 6;
    if (sscanf(argv[6], "%zu", &index) != 1)
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
    
    printf("Number of runs %d\n", n_runs);
    printf("Matrix size %zu\n", N);

    
    A = (double*)malloc(N * N * sizeof(double));
    B = (double*)malloc(N * N * sizeof(double));
    C = (double*)malloc(N * N * sizeof(double));
    runtimes = (double *)malloc(n_runs * sizeof(double));

    RandomMatrix(&A[0], N);
    RandomMatrix(&B[0], N);

    printf("MatMul - %s\n", names[index]);

    average_runtime = 0.0;
    for(int n = 0; n < n_runs; n++)
    {
        runtimes[n] = Benchmark(funcs[index], &A[0], &B[0], &C[0], N);
        printf("runtime\t%s\t%lf\t%zu\t%s\n", names[index], runtimes[n], N, argv[7]);
        average_runtime += runtimes[n]/n_runs;
    }

    printf("average runtime %lf seconds\n", average_runtime);
    printf("---------------------------------\n");

    free(A);
    free(B);
    free(C);
    free(runtimes);
    return 0;
}
