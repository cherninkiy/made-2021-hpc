// matrix by matrix multiplication functions

typedef void (*MatMulFunc)(const double* A, const double* B, double* C, size_t N);

void ZeroMatrix(double* A, size_t N);

void RandomMatrix(double* A, size_t N);

void CalcMatMul_ijk(const double* A, const double* B, double* C, size_t N);

void CalcMatMul_jik(const double* A, const double* B, double* C, size_t N);

void CalcMatMul_kij(const double* A, const double* B, double* C, size_t N);

void CalcMatMul_kij_opt(const double* A, const double* B, double* C, size_t N);

void CalcMatMul_blas(const double* A, const double* B, double* C, size_t N);

void CalcMatSum(const double* A, const double* B, double* C, size_t N);

void CalcMatSubt(const double* A, const double* B, double* C, size_t N);

void CalcMatMul_Shtrassen(const double* A, const double* B, double* C, size_t N);
