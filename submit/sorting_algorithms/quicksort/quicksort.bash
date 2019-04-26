#!/bin/sh
#SBATCH --job-name=QUICKSORT
#SBATCH -t 00:10:00
#SBATCH -D /gpfs/u/home/PCP8/PCP8vlkc/scratch/final/sorting_algorithms/quicksort
#
#SBATCH --mail-type=ALL
#SBATCH --mail-user=valkc@rpi.edu

srun --ntasks 1 --overcommit -o quicksort_1048576.log ./quicksort.out 1048576 &
srun --ntasks 1 --overcommit -o quicksort_2097152.log ./quicksort.out 2097152 &
srun --ntasks 1 --overcommit -o quicksort_4194304.log ./quicksort.out 4194304 &
srun --ntasks 1 --overcommit -o quicksort_8388608.log ./quicksort.out 8388608 &
srun --ntasks 1 --overcommit -o quicksort_16777216.log ./quicksort.out 16777216 &
srun --ntasks 1 --overcommit -o quicksort_33554432.log ./quicksort.out 33554432 &
wait
