#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

#define N 1000000
#define STRIP_SIZE 8

int main()
{
    float *A = malloc(N * sizeof(float));
    float *B = malloc(N * sizeof(float));
    float *C = malloc(N * sizeof(float));

    if (A == NULL || B == NULL || C == NULL)
    {
        printf("Memory allocation failed\n");
        return 1;
    }

    // Initialize arrays
    for (int i = 0; i < N; i++)
    {
        A[i] = i;
        B[i] = 2.0f;
    }

    double start = omp_get_wtime();

    // Strip mining + OpenMP
    #pragma omp parallel for schedule(static)
    for (int start_index = 0; start_index < N; start_index += STRIP_SIZE)
    {
        int end = start_index + STRIP_SIZE;

        if (end > N)
            end = N;

        for (int i = start_index; i < end; i++)
        {
            C[i] = A[i] * B[i];
        }
    }

    double end = omp_get_wtime();

    printf("C[0] = %.2f\n", C[0]);
    printf("C[999999] = %.2f\n", C[999999]);
    printf("Time = %f seconds\n", end - start);

    free(A);
    free(B);
    free(C);

    return 0;
}