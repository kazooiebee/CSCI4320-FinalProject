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
// Simple function to swap elements.
void swap(int *a, int *b)
{
    int t = *a;
    *a = *b;
    *b = t;
}

/* 
Parition array into elements above and elements below pivot.
 */
int partition(int *arr, int low, int high)
{
    int pivot = arr[high]; // pivot
    int i = (low - 1);     // Index of smaller element

    for (int j = low; j <= high - 1; j++)
    {
        // If current element is smaller than or
        // equal to pivot
        if (arr[j] <= pivot)
        {
            i++; // increment index of smaller element
            swap(&arr[i], &arr[j]);
        }
    }
    swap(&arr[i + 1], &arr[high]);
    return (i + 1);
}

/* 
quicksort partitions array into 2 parts pased on the pivot (high in this case)
then performs quicksort on the two smaller partitioned arrays.
*/
void quickSort(int *arr, int low, int high)
{
    if (low < high)
    {
        // partition array
        int part = partition(arr, low, high);

        quickSort(arr, low, part - 1);
        quickSort(arr, part + 1, high);
    }
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
    quickSort(arr, 0, n - 1); // perform sort
    double end_cycles = GetTimeBase();
    printf("Done sorting.\n");

    double execution_time = ((double)(end_cycles - start_cycles)) /
                            processor_frequency;

    printf("%f\n", execution_time);

    free(arr);
}