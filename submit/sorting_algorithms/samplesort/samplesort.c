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

int cmp(const void *a, const void *b)
{
    return (*(int *)a - *(int *)b);
}

void *ssort(int *arr, int size)
{
    qsort(arr, size, sizeof(int), cmp);
    return NULL;
}

// merges elements from multiple ranks, each are sorted, to one sorted new array.
void multimerge(int *orig_array, int *start_idx, int *lens, int mpi_size, int *new_array, int len)
{
    int i, j, min, idx;
    int max = orig_array[0];

    for (i = 0; i < mpi_size; i++)
    {
        if (max < orig_array[start_idx[i] + lens[i] - 1])
        {
            max = orig_array[start_idx[i] + lens[i] - 1]; // determine max of entire array as baseline 
        }
    }

    for (i = 0; i < len; i++)
    {
        min = max;
        idx = 0;
        for (j = 0; j < mpi_size; j++)
        {
            if (lens[j] > 0 && orig_array[start_idx[j]] < min)
            {
                idx = j;
                min = orig_array[start_idx[j]];
            }
        }

        new_array[i] = min;
        start_idx[idx]++;
        lens[idx]--;
    }
}

void *samplesort(int *arr, int n, int mpi_rank, int mpi_size, void *serialsort(int *, int))
{
    int i, j, k;
    k = n / mpi_size;
    int *samples = calloc(mpi_size, sizeof(int));
    int *all_samples = NULL;
    int *all_samples_merged = NULL;
    int *local = calloc(k, sizeof(int));
    int *pivots = calloc(mpi_size - 1, sizeof(int));
    int *class_start = calloc(mpi_size, sizeof(int));
    int *class_end = calloc(mpi_size, sizeof(int));
    int *pivot_lens = calloc(mpi_size, sizeof(int));
    int *pivot_starts = calloc(mpi_size, sizeof(int));
    int *recvb = calloc(n, sizeof(int));
    int *all_lengths = NULL;
    int *all_starts = NULL;

    if (mpi_rank == 0)
    {
        all_samples = calloc(mpi_size * mpi_size, sizeof(int));
        all_samples_merged = calloc(mpi_size * mpi_size, sizeof(int));
        all_lengths = calloc(mpi_size, sizeof(int));
        all_starts = calloc(mpi_size, sizeof(int));
    }

    // STEP 1:
    // scatter data from root to all processes
    MPI_Scatter(arr, k, MPI_INT, local, k, MPI_INT, 0, MPI_COMM_WORLD);

    // local sort
    (*serialsort)(local, k);

    // STEP 2: localization
    // gather and merge samples, broadcast pivots
    for (i = 0; i < mpi_size; i++)
    {
        samples[i] = local[i * (k / mpi_size)];
    }

    MPI_Gather(samples, mpi_size, MPI_UNSIGNED, all_samples, mpi_size, MPI_UNSIGNED, 0, MPI_COMM_WORLD);

    if (mpi_rank == 0)
    {
        int *start_idx = calloc(mpi_size, sizeof(int));
        int *end_idx = calloc(mpi_size, sizeof(int));

        for (i = 0; i < mpi_size; i++)
        {
            start_idx[i] = i * mpi_size;
            end_idx[i] = mpi_size;
        }

        multimerge(all_samples, start_idx, end_idx, mpi_size, all_samples_merged, mpi_size * mpi_size);

        for (i = 0; i < mpi_size - 1; i++)
        {
            pivots[i] = all_samples_merged[(i + 1) * mpi_size];
        }

        free(start_idx);
        free(end_idx);
    }

    MPI_Bcast(pivots, mpi_size - 1, MPI_INT, 0, MPI_COMM_WORLD);
    // partition local data

    int dataindex = 0;
    for (i = 0; i < mpi_size - 1; i++)
    {
        class_start[i] = dataindex;
        class_end[i] = 0;

        while (dataindex < k && local[dataindex] <= pivots[i])
        {
            class_end[i]++;
            dataindex++;
        }
    }
    class_start[mpi_size - 1] = dataindex;
    class_end[mpi_size - 1] = k - dataindex;

    // ith class gathered by processor i
    for (i = 0; i < mpi_size; i++)
    {
        MPI_Gather(&class_end[i], 1, MPI_INT, pivot_lens, 1, MPI_INT, i, MPI_COMM_WORLD);

        if (i == mpi_rank)
        {
            pivot_starts[0] = 0;

            for (j = 1; j < mpi_size; j++)
            {
                pivot_starts[j] = pivot_starts[j - 1] + pivot_lens[j - 1];
            }
        }

        MPI_Gatherv(&local[class_start[i]], class_end[i], MPI_INT, recvb, pivot_lens, pivot_starts, MPI_INT, i, MPI_COMM_WORLD);
    }
    int sum = pivot_lens[mpi_size - 1] + pivot_starts[mpi_size - 1];
    free(local);
    local = calloc(sum, sizeof(int));
    multimerge(recvb, pivot_starts, pivot_lens, mpi_size, local, sum);
    MPI_Gather(&sum, 1, MPI_INT, all_lengths, 1, MPI_INT, 0, MPI_COMM_WORLD);

    if (mpi_rank == 0)
    {
        all_starts[0] = 0;
        for (i = 1; i < mpi_size; i++)
        {
            all_starts[i] = all_starts[i - 1] + all_lengths[i - 1];
        }
    }

    MPI_Gatherv(local, sum, MPI_INT, arr, all_lengths, all_starts, MPI_INT, 0, MPI_COMM_WORLD);

    // free all memory used
    free(samples);
    free(local);
    free(pivots);
    free(class_start);
    free(class_end);
    free(pivot_lens);
    free(pivot_starts);
    free(recvb);

    if (mpi_rank == 0)
    { // free memory of root
        free(all_samples);
        free(all_samples_merged);
        free(all_lengths);
        free(all_starts);
    }
}

int main(int argc, char *argv[])
{
    int n;
    int i;
    int *arr = NULL;
    int mpi_size = -1;
    int mpi_rank = -1;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &mpi_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &mpi_rank);

    if (argc < 2)
    {
        if (mpi_rank == 0)
        {
            fprintf(stderr, "ERROR: Incorrect arguments.\nUSAGE: %s [ARRAY SIZE]\n", argv[0]);
        }
        MPI_Barrier(MPI_COMM_WORLD);

        MPI_Finalize();
        exit(1);
    }
    else
    {
        n = atoi(argv[1]);
    }

    if (mpi_rank == 0)
    {
        arr = calloc(n, sizeof(int));
        for (i = 0; i < n; i++)
        { // initialize array as random integers
            arr[i] = rand();
        }
    }

    double start_cycles = GetTimeBase();
    samplesort(arr, n, mpi_rank, mpi_size, &ssort);
    double end_cycles = GetTimeBase();

    if (mpi_rank == 0)
    {
        for (i = 1; i < n; i++)
        {
            if (arr[i - 1] > arr[i])
            {
                fprintf(stderr, "%d, %d\n", arr[i - 1], arr[i]); // print elements out of place (this should not print anything.)
            }
        }

        double execution_time = ((double)(end_cycles - start_cycles)) /
                                processor_frequency;

        printf("%f\n", execution_time); // print execution time.
        fflush(stdout);
        free(arr);
    }

    MPI_Barrier(MPI_COMM_WORLD);

    MPI_Finalize();
    return 0;
}