#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <mpi.h>

// #define BGQ 1 // when running BG/Q, comment out when testing on mastiff

#ifdef BGQ
#include <hwi/include/bqc/A2_inlines.h>
#else
#define GetTimeBase MPI_Wtime
#endif

double time_in_secs = 0;
double g_processor_frequency = 1600000000.0; // processing speed for BG/Q
unsigned long long start_cycles = 0;
unsigned long long end_cycles = 0;


int main(int argc, char *argv[])
{
  int mpi_myrank, mpi_commsize;

  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &mpi_commsize);
  MPI_Comm_rank(MPI_COMM_WORLD, &mpi_myrank);

  //TIMING STUFF
  start_cycles = GetTimeBase();


  end_cycles = GetTimeBase();

  time_in_secs = ((double)(end_cycles - start_cycles)) / g_processor_frequency;
  printf("Time: %f\n", time_in_secs);

  MPI_Finalize();
  return 0;
}
