#!/bin/sh
#SBATCH --job-name=BITONICSORT
#SBATCH -t 00:29:00
#SBATCH -D /gpfs/u/home/PCP8/PCP8vlkc/scratch/final/sorting_algorithms/bitonicsort
#
#SBATCH --mail-type=ALL
#SBATCH --mail-user=valkc@rpi.edu

srun --ntasks 2 --overcommit -o bitonicsort_33554432_2.log ./bitonicsort.out 33554432 &
srun --ntasks 4 --overcommit -o bitonicsort_33554432_4.log ./bitonicsort.out 33554432 &
srun --ntasks 8 --overcommit -o bitonicsort_33554432_8.log ./bitonicsort.out 33554432 &
srun --ntasks 16 --overcommit -o bitonicsort_33554432_16.log ./bitonicsort.out 33554432 &
srun --ntasks 32 --overcommit -o bitonicsort_33554432_32.log ./bitonicsort.out 33554432 &
srun --ntasks 64 --overcommit -o bitonicsort_33554432_64.log ./bitonicsort.out 33554432 &
srun --ntasks 128 --overcommit -o bitonicsort_33554432_128.log ./bitonicsort.out 33554432 &
srun --ntasks 256 --overcommit -o bitonicsort_33554432_256.log ./bitonicsort.out 33554432 &
srun --ntasks 512 --overcommit -o bitonicsort_33554432_512.log ./bitonicsort.out 33554432 &
srun --ntasks 1024 --overcommit -o bitonicsort_33554432_1024.log ./bitonicsort.out 33554432 &
srun --ntasks 2048 --overcommit -o bitonicsort_33554432_2048.log ./bitonicsort.out 33554432 &
srun --ntasks 4096 --overcommit -o bitonicsort_33554432_4096.log ./bitonicsort.out 33554432 &
wait
