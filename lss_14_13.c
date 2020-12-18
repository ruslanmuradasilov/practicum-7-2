#include "lss_14_13.h"

extern int fl_e, fl_d;

size_t lss_memsize_14_13(int n)
{
    return 2 * n * sizeof(double);
}

int sign(double x)
{
    return (x > 0) - (x < 0);
}

int isMatrixSymmetric(int n, const double *A)
{
    int i, j;
    for (i = 0; i < n; i++)
    {
        for (j = i + 1; j < n; j++)
        {
            if (A[i * n + j] != A[j * n + i])
                return -1;//не симметрична
        }
    }
    return 0;
}

int lss_14_13(int n, double *A, double *B, double *X, double *tmp)
{
    int i, j, k;
    double sum;
    double *D = tmp;
    double *Y = tmp + n;

    if (fl_d) printf("Checking if matrix is symmetric...\n");

    if (isMatrixSymmetric(n, A))
    {
        if (fl_e) printf("Error! Matrix is not symmetric!\n");
        return -1;//матрица не симметрична
    }

    if (fl_d) printf("Matrix is symmetric.\nStarting Cholesky decomposition...\n");

    //разложение Холецкого: A = R*DR
    for (i = 0; i < n; i++)
    {
        sum = 0;
        for (k = 0; k < i; k++)
            sum += A[k * n + i] * A[k * n + i] * D[k];
        D[i] = sign(A[i * n + i] - sum);
        if (D[i] == 0)
        {
            if (fl_e) printf("Error! Matrix is not symmetric!\n");
            return -2;//найден нулевой главный минор
        }
        A[i * n + i] = sqrt(fabs(A[i * n + i] - sum));

        for (j = i + 1; j < n; j++)
        {
            sum = 0;
            for (k = 0; k < i; k++)
                sum += A[k * n + i] * A[k * n + j] * D[k];
            A[i * n + j] = (A[i * n + j] - sum) / (A[i * n + i] * D[i]);
        }
    }

    //превращаем R = A в верхне треугольную матрицу
    for (i = 1; i < n; i++)
        for (j = 0; j < i; j++)
            A[i * n + j] = 0;

    if (fl_d) printf("Cholesky decomposition completed successfully!\n");

    if (fl_d) printf("Starting to solve linear system...\n");

    //решаем R*Y = B
    for (i = 0; i < n; i++)
    {
        sum = 0;
        for (k = 0; k < i; k++)
            sum = sum + A[k * n + i] * Y[k];
        Y[i] = (B[i] - sum) / A[i * n + i];
    }
    //решаем RDX = Y
    for (i = n - 1; i >= 0; i--)
    {
        sum = 0;
        for (k = i + 1; k < n; k++)
            sum = sum + A[i * n + k] * X[k] * D[k];
        X[i] = (Y[i] - sum) / (A[i * n + i] * D[i]);
    }

    if (fl_d) printf("Linear system solved successfully!\n");

    return 0;
}