#!/bin/sh
#SBATCH --job-name=SAMPLESORT
#SBATCH -t 00:29:00
#SBATCH -D /gpfs/u/home/PCP8/PCP8vlkc/scratch/final/sorting_algorithms/samplesort
#
#SBATCH --mail-type=ALL
#SBATCH --mail-user=valkc@rpi.edu

srun --ntasks 2 --overcommit -o samplesort_8388608_2.log ./samplesort.out 8388608 &
srun --ntasks 4 --overcommit -o samplesort_8388608_4.log ./samplesort.out 8388608 &
srun --ntasks 8 --overcommit -o samplesort_8388608_8.log ./samplesort.out 8388608 &
srun --ntasks 16 --overcommit -o samplesort_8388608_16.log ./samplesort.out 8388608 &
srun --ntasks 32 --overcommit -o samplesort_8388608_32.log ./samplesort.out 8388608 &
srun --ntasks 64 --overcommit -o samplesort_8388608_64.log ./samplesort.out 8388608 &
srun --ntasks 128 --overcommit -o samplesort_8388608_128.log ./samplesort.out 8388608 &
srun --ntasks 256 --overcommit -o samplesort_8388608_256.log ./samplesort.out 8388608 &
srun --ntasks 512 --overcommit -o samplesort_8388608_512.log ./samplesort.out 8388608 &
srun --ntasks 1024 --overcommit -o samplesort_8388608_1024.log ./samplesort.out 8388608 &
srun --ntasks 2048 --overcommit -o samplesort_8388608_2048.log ./samplesort.out 8388608 &
srun --ntasks 4096 --overcommit -o samplesort_8388608_4096.log ./samplesort.out 8388608 &
wait
