#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <omp.h>

#define NPOINTS 2000
#define MAXITER 2000

struct complex {
    double real;
    double imag;
};

int main()
{
    int i, j, iter;
    int numoutside = 0;

    double area, error, ztemp;
    struct complex z, c;

    double start = omp_get_wtime();

    #pragma omp parallel
    {
        int tid = omp_get_thread_num();
        int nthreads = omp_get_num_threads();

        /*
         * Divide the outer loop equally among threads.
         */
        int start_i = tid * NPOINTS / nthreads;
        int end_i = (tid + 1) * NPOINTS / nthreads;

        int local_numoutside = 0;

        for (i = start_i; i < end_i; i++)
        {
            for (j = 0; j < NPOINTS; j++)
            {
                c.real = -2.0 +
                         2.5 * (double)i / (double)NPOINTS +
                         1.0e-7;

                c.imag = 1.125 *
                         (double)j / (double)NPOINTS +
                         1.0e-7;

                z = c;

                for (iter = 0; iter < MAXITER; iter++)
                {
                    ztemp = (z.real * z.real) -
                            (z.imag * z.imag) +
                            c.real;

                    z.imag = z.real * z.imag * 2 + c.imag;
                    z.real = ztemp;

                    if ((z.real * z.real +
                         z.imag * z.imag) > 4.0e0)
                    {
                        local_numoutside++;
                        break;
                    }
                }
            }
        }

        /*
         * Safely combine results from all threads.
         */
        #pragma omp atomic
        numoutside += local_numoutside;
    }

    area = 2.0 * 2.5 * 1.125 *
           (double)(NPOINTS * NPOINTS - numoutside) /
           (double)(NPOINTS * NPOINTS);

    error = area / (double)NPOINTS;

    double end = omp_get_wtime();

    printf("Area of Mandlebrot set = %12.8f +/- %12.8f\n",
           area, error);

    printf("Time = %f seconds\n", end - start);

    return 0;
}