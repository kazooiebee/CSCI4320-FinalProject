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

void swap(int *a, int i, int j)
{
    int tmp = a[i];
    a[i] = a[j];
    a[j] = tmp;
}

void parallelbitonicMerge(int *arr, int n, int k, int j, int mpi_size, int mpi_rank)
{
    int i;
    MPI_Request req_next, req_prev, send_req_next, send_req_prev;
    MPI_Status stat;
    int global_idx;
    int *prev_local = NULL, *next_local = NULL;

    for (i = 0; i < n / mpi_size; i++)
    {
        global_idx = i + n / mpi_size * mpi_rank;
        int ij = global_idx ^ j;
        int ij_rank = ij / (n / mpi_size);
        if (ij > global_idx)
        {
            if (ij_rank != mpi_rank)
            {
                if (next_local == NULL)
                {
                    next_local = calloc(n / mpi_size, sizeof(int));
                    MPI_Irecv(next_local, n / mpi_size, MPI_INT, ij_rank, 0, MPI_COMM_WORLD, &req_next);
                    MPI_Isend(arr, n / mpi_size, MPI_INT, ij_rank, 0, MPI_COMM_WORLD, &send_req_next);
                    MPI_Wait(&req_next, &stat);
                    MPI_Wait(&send_req_next, &stat);
                }
                // send yourself to ij_rank, receive from ij_rank info you need
                //printf("RANK %d: received %d from rank %d\n", mpi_rank, tmp, ij_rank);

                if ((global_idx & k) == 0 && arr[i] > next_local[i])
                    arr[i] = next_local[i];
                if ((global_idx & k) != 0 && arr[i] < next_local[i])
                    arr[i] = next_local[i];
            }
            else
            {
                if ((global_idx & k) == 0 && arr[i] > arr[ij - n / mpi_size * mpi_rank])
                    swap(arr, i, ij - n / mpi_size * mpi_rank);
                if ((global_idx & k) != 0 && arr[i] < arr[ij - n / mpi_size * mpi_rank])
                    swap(arr, i, ij - n / mpi_size * mpi_rank);
            }
        }
        else
        {
            // see if we need to compare from another rank.
            if (ij_rank != mpi_rank)
            {
                if (prev_local == NULL)
                {
                    prev_local = calloc(n / mpi_size, sizeof(int));
                    MPI_Irecv(prev_local, n / mpi_size, MPI_INT, ij_rank, 0, MPI_COMM_WORLD, &req_prev);
                    MPI_Isend(arr, n / mpi_size, MPI_INT, ij_rank, 0, MPI_COMM_WORLD, &send_req_prev);
                    MPI_Wait(&req_prev, &stat);
                    MPI_Wait(&send_req_prev, &stat);
                }
                //printf("RANK %d: received %d from rank %d\n", mpi_rank, tmp, ij_rank);

                if ((ij & k) == 0 && arr[i] < prev_local[i])
                    arr[i] = prev_local[i];
                if ((ij & k) != 0 && arr[i] > prev_local[i])
                    arr[i] = prev_local[i];
            }
        }
    }

    if (prev_local != NULL)
        free(prev_local);
    if (next_local != NULL)
        free(next_local);
}

void bitonicsort(int *arr, int n, int mpi_rank, int mpi_size)
{
    int k, j;

    k = n / mpi_size;

    if (mpi_rank == 0)
    {
        MPI_Scatter(arr, k, MPI_INT, MPI_IN_PLACE, k, MPI_INT, 0, MPI_COMM_WORLD);
    }
    else
    {
        arr = calloc(k, sizeof(int));
        MPI_Scatter(arr, k, MPI_INT, arr, k, MPI_INT, 0, MPI_COMM_WORLD);
    }

    for (k = 2; k <= n; k *= 2)
    {
        for (j = k >> 1; j > 0; j = j >> 1)
        {
            parallelbitonicMerge(arr, n, k, j, mpi_size, mpi_rank);
        }
    }

    if (mpi_rank == 0)
    {
        MPI_Gather(MPI_IN_PLACE, n / mpi_size, MPI_INT, arr, n / mpi_size, MPI_INT, 0, MPI_COMM_WORLD);
    }
    else
    {
        MPI_Gather(arr, n / mpi_size, MPI_INT, arr, n / mpi_size, MPI_INT, 0, MPI_COMM_WORLD);
    }

    if (mpi_rank != 0)
    {
        free(arr);
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
        {
            arr[i] = rand();
        }
    }

    double start_cycles = GetTimeBase();
    bitonicsort(arr, n, mpi_rank, mpi_size);
    double end_cycles = GetTimeBase();

    if (mpi_rank == 0)
    {
        for (i = 1; i < n; i++)
        {
            // printf("%d\n", arr[i]);
            if (arr[i - 1] > arr[i])
            {
                fprintf(stderr, "%d, %d\n", arr[i - 1], arr[i]);
            }
        }

        double execution_time = ((double)(end_cycles - start_cycles)) /
                                processor_frequency;

        printf("%f\n", execution_time);
        fflush(stdout);
        free(arr);
    }

    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Finalize();
    return 0;
}