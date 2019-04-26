#!/bin/sh
#SBATCH --job-name=BUCKETSORT
#SBATCH -t 00:29:00
#SBATCH -D /gpfs/u/home/PCP8/PCP8vlkc/scratch/final/sorting_algorithms/bucketsort
#
#SBATCH --mail-type=ALL
#SBATCH --mail-user=valkc@rpi.edu

srun --ntasks 2 --overcommit -o bucketsort_16777216_2.log ./bucketsort.out 16777216 &
srun --ntasks 4 --overcommit -o bucketsort_16777216_4.log ./bucketsort.out 16777216 &
srun --ntasks 8 --overcommit -o bucketsort_16777216_8.log ./bucketsort.out 16777216 &
srun --ntasks 16 --overcommit -o bucketsort_16777216_16.log ./bucketsort.out 16777216 &
srun --ntasks 32 --overcommit -o bucketsort_16777216_32.log ./bucketsort.out 16777216 &
srun --ntasks 64 --overcommit -o bucketsort_16777216_64.log ./bucketsort.out 16777216 &
srun --ntasks 128 --overcommit -o bucketsort_16777216_128.log ./bucketsort.out 16777216 &
srun --ntasks 256 --overcommit -o bucketsort_16777216_256.log ./bucketsort.out 16777216 &
srun --ntasks 512 --overcommit -o bucketsort_16777216_512.log ./bucketsort.out 16777216 &
srun --ntasks 1024 --overcommit -o bucketsort_16777216_1024.log ./bucketsort.out 16777216 &
srun --ntasks 2048 --overcommit -o bucketsort_16777216_2048.log ./bucketsort.out 16777216 &
srun --ntasks 4096 --overcommit -o bucketsort_16777216_4096.log ./bucketsort.out 16777216 &
wait
