#!/bin/sh
#SBATCH --job-name=SAMPLESORT
#SBATCH -t 00:29:00
#SBATCH -D /gpfs/u/home/PCP8/PCP8vlkc/scratch/final/sorting_algorithms/samplesort
#
#SBATCH --mail-type=ALL
#SBATCH --mail-user=valkc@rpi.edu
srun --ntasks 2 --overcommit -o samplesort_2097152_2.log ./samplesort.out 2097152 &
srun --ntasks 4 --overcommit -o samplesort_2097152_4.log ./samplesort.out 2097152 &
srun --ntasks 8 --overcommit -o samplesort_2097152_8.log ./samplesort.out 2097152 &
srun --ntasks 16 --overcommit -o samplesort_2097152_16.log ./samplesort.out 2097152 &
srun --ntasks 32 --overcommit -o samplesort_2097152_32.log ./samplesort.out 2097152 &
srun --ntasks 64 --overcommit -o samplesort_2097152_64.log ./samplesort.out 2097152 &
srun --ntasks 128 --overcommit -o samplesort_2097152_128.log ./samplesort.out 2097152 &
srun --ntasks 256 --overcommit -o samplesort_2097152_256.log ./samplesort.out 2097152 &
srun --ntasks 512 --overcommit -o samplesort_2097152_512.log ./samplesort.out 2097152 &
srun --ntasks 1024 --overcommit -o samplesort_2097152_1024.log ./samplesort.out 2097152 &
srun --ntasks 2048 --overcommit -o samplesort_2097152_2048.log ./samplesort.out 2097152 &
srun --ntasks 4096 --overcommit -o samplesort_2097152_4096.log ./samplesort.out 2097152 &
wait
