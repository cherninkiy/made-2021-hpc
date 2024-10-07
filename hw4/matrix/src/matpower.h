// matrix power function
#include "matmul.h"

void MakeIdentity(double* A, size_t N);

void CopyMatrix(const double* A, double* B, size_t N);

void CalcMatPower(const double* A, double* C, size_t N, int p, MatMulFunc func);