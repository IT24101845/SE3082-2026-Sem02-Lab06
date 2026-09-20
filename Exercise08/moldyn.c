#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <omp.h>

#define N 1000

int main()
{
    double *x = malloc(N * sizeof(double));
    double *force = malloc(N * sizeof(double));

    double potential = 0.0;
    double virial = 0.0;

    for (int i = 0; i < N; i++)
    {
        x[i] = (double)i * 0.01;
        force[i] = 0.0;
    }

    double start = omp_get_wtime();

    #pragma omp parallel for reduction(+:potential,virial)
    for (int i = 0; i < N; i++)
    {
        for (int j = i + 1; j < N; j++)
        {
            double distance = fabs(x[j] - x[i]) + 0.001;

            double r2 = distance * distance;
            double r6 = r2 * r2 * r2;

            double interaction = 1.0 / r6;
            double f = interaction / distance;

            /*
             * Shared force updates.
             */
            #pragma omp critical
            {
                force[i] += f;
                force[j] -= f;
            }

            potential += interaction;
            virial += f * distance;
        }
    }

    double stop = omp_get_wtime();

    printf("Potential energy = %f\n", potential);
    printf("Virial = %f\n", virial);
    printf("Force[0] = %f\n", force[0]);
    printf("Time = %f seconds\n", stop - start);

    free(x);
    free(force);

    return 0;
}