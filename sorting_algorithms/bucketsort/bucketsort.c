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

void bucketsort(int *arr, int n, int mpi_rank, int mpi_size)
{
    unsigned long long int div = 1;
    int i, s, size, max, idx, local_len;
    int *local, *lens, *starts;
    int buffer[2];
    s = 0;
    lens = NULL;
    starts = NULL;
    local = NULL;

    if (mpi_rank == 0)
    {
        lens = calloc(mpi_size, sizeof(int));
        starts = calloc(mpi_size, sizeof(int));

        max = arr[0];
        for (i = 1; i < n; i++)
        {
            if (arr[i] > max)
            {
                max = arr[i];
            }
        }

        while (max > div)
        {
            s = 0;

            for (i = 0; i < mpi_size; i++)
            {
                lens[i] = 0;
            }

            for (i = 0; i < n; i++)
            {
                idx = (arr[i] / div) % mpi_size;
                lens[idx]++;
            }

            starts[0] = 0;
            for (i = 1; i < mpi_size; i++)
            {
                starts[i] = starts[i - 1] + lens[i - 1];
            }

            MPI_Scatter(lens, 1, MPI_INT, &local_len, 1, MPI_INT, 0, MPI_COMM_WORLD);

            if (local != NULL)
            {
                free(local);
            }

            if (local_len != 0)
            {
                local = calloc(local_len, sizeof(int));
            }
            else
            {
                local = NULL;
            }

            for (i = 0; i < n; i++)
            {
                idx = (arr[i] / div) % mpi_size;

                if (idx == 0)
                {
                    local[s] = arr[i];
                    s++;
                }
                else
                {
                    MPI_Send(&(arr[i]), 1, MPI_INT, idx, 0, MPI_COMM_WORLD);
                }
            }

            for (i = 1; i < mpi_size; i++)
            {
                MPI_Send(arr, 0, MPI_INT, i, 0, MPI_COMM_WORLD);
            }

            MPI_Gatherv(local, local_len, MPI_INT, arr, lens, starts, MPI_INT, 0, MPI_COMM_WORLD);
            div *= mpi_size;
        }

        MPI_Scatter(lens, 1, MPI_INT, &local_len, 1, MPI_INT, 0, MPI_COMM_WORLD);

        for (i = 1; i < mpi_size; i++)
        {
            MPI_Send(arr, 2, MPI_INT, i, 0, MPI_COMM_WORLD);
        }

        if (local != NULL)
        {
            free(local);
        }
        free(lens);
        free(starts);
    }
    else
    {
        while (1)
        {
            MPI_Scatter(NULL, 1, MPI_INT, &local_len, 1, MPI_INT, 0, MPI_COMM_WORLD);
            if (local != NULL)
            {
                free(local);
            }

            if (local_len == 0)
            {
                local = NULL;
            }
            else
            {
                local = calloc(local_len, sizeof(int));
            }

            s = 0;
            size = 1;
            while (1)
            {
                MPI_Status stat;
                MPI_Probe(0, 0, MPI_COMM_WORLD, &stat);
                MPI_Get_count(&stat, MPI_INT, &size);
                MPI_Recv(buffer, size, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

                if (size == 0)
                {
                    // gather results and repeat process.
                    // first gather sizes
                    MPI_Gatherv(local, local_len, MPI_INT, NULL, NULL, NULL, MPI_INT, 0, MPI_COMM_WORLD);
                    break;
                }
                else if (size == 1)
                {
                    // receive number and add to local list
                    local[s] = buffer[0];
                    s++;
                }
                else
                {
                    // all done, free and get out of here.
                    fflush(stdout);
                    return;
                }
            }
        }
    }

    return;
}

int main(int argc, char *argv[])
{
    int mpi_size = -1;
    int mpi_rank = -1;
    int i, n, *arr;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &mpi_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &mpi_rank);
    arr = NULL;

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
    bucketsort(arr, n, mpi_rank, mpi_size);
    double end_cycles = GetTimeBase();

    MPI_Barrier(MPI_COMM_WORLD);

    if (mpi_rank == 0)
    {
        // printf("%d\n", arr[0]);
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

    MPI_Finalize();
    return 0;
}
