#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

// #define BGQ 1 // when running BG/Q, comment out when running on mastiff
#ifdef BGQ
#include <hwi/include/bqc/A2_inlines.h>
#else
#define GetTimeBase MPI_Wtime
#endif

double processor_frequency = 1600000000.0;

// compare function for integers, to be used by qsort.
int compare(const void* p1, const void* p2) {
    return *((int*) p1) - *((int*) p2);
}

int main(int argc, char *argv[])
{

    MPI_Init(&argc, &argv);
    int i;
    int n = 100000;
    int *arr;

    if (argc >= 2)
    {
        n = atoi(argv[1]);
    }
    arr = calloc(n, sizeof(int));

    printf("Initializing...\n");
    for (i = 0; i < n; i++)
    { // initialize an array of random positive integers
        arr[i] = rand();
    }
    printf("Done initializing.\n");

    printf("Sorting...\n");
    double start_cycles = GetTimeBase();
    qsort(arr, n, sizeof(int), &compare); // perform sort
    double end_cycles = GetTimeBase();
    printf("Done sorting.\n");

    double execution_time = ((double)(end_cycles - start_cycles)) /
                            processor_frequency;

    printf("%f\n", execution_time);

    free(arr);
}