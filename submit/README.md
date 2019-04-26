# CSCI 4320 Final Project: Comparing the Parallel Scalability of Bucketsort, Samplesort, and Bitonic Mergesort to Serial Quicksort

### Compilation
Each of the sorting algorithms are in their own folder. They each have a Makefile, so they can all be compiled by simply running make within the working directory.

### Usage
All of the programs take one command line argument n, the size of the array. This is how to compile and perform samplesort on an array of 100 random elements with 4 MPI ranks:

```
cd sorting_algorithms/samplesort
make
mpirun -np 4 ./samplesort.out 100
```

The code here is assuming you are not on the Blue Gene Q. uncomment the line 

```
// define BGQ 1
```

and recompile to get accurate time measurements on the Blue Gene Q system.