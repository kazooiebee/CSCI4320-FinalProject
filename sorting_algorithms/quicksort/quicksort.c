// #include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

// #define BGQ 1 // when running BG/Q, comment out when running on mastiff
#ifdef BGQ
#include <hwi/include/bqc/A2_inlines.h>
#else
#define GetTimeBase MPI_Wtime
#endif

long long int long_long_int_random()
{
    long long int R = 0;
    int times = sizeof(long long int) / sizeof(int);
    int i;
    for (i = 0; i < times; i++)
    {
        R = (R << 32) | (long long int)rand();
    }

    return R;
}

double processor_frequency = 1600000000.0;
// A utility function to swap two elements
void swap(long long int *a, long long int *b)
{
    long long int t = *a;
    *a = *b;
    *b = t;
}

/* This function takes last element as pivot, places 
   the pivot element at its correct position in sorted 
    array, and places all smaller (smaller than pivot) 
   to left of pivot and all greater elements to right 
   of pivot */
long long int partition(long long int arr[], long long int low, long long int high)
{
    long long int pivot = arr[high]; // pivot
    long long int i = (low - 1);     // Index of smaller element

    for (long long int j = low; j <= high - 1; j++)
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

/* The main function that implements QuickSort 
 arr[] --> Array to be sorted, 
  low  --> Starting index, 
  high  --> Ending index */
void quickSort(long long int arr[], long long int low, long long int high)
{
    if (low < high)
    {
        /* pi is partitioning index, arr[p] is now 
           at right place */
        long long int pi = partition(arr, low, high);

        // Separately sort elements before
        // partition and after partition
        quickSort(arr, low, pi - 1);
        quickSort(arr, pi + 1, high);
    }
}

int main(int argc, char *argv[])
{
    int i;
    long long int n = 100000;
    long long int *arr;

    if (argc >= 2)
    {
        n = atoi(argv[1]);
    }
    arr = calloc(n, sizeof(long long int));

    printf("Initializing...\n");
    for (i = 0; i < n; i++)
    {
        arr[i] = long_long_int_random();
    }
    printf("Done initializing.\n");

    printf("Sorting...\n");
    quickSort(arr, 0, n - 1);
    printf("Done sorting.\n");

    for (i = 0; i < n; i++)
    {
        printf("%llu\n", arr[i]);
    }

    free(arr);
}