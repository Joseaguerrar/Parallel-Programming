# Hybrid MPI/OpenMP distributed broadcast

## Description

This program distributes a range of integers between MPI processes and OpenMP threads.
Each MPI process receives the range of integers to distribute, and each OpenMP thread
receives the range of integers to process.

## Usage
To compile the program, use the following command:
```
make clean
make
```
To run the program, use the following command:
```
mpirun -np 4 bin/hybrid_distr_bcast 10 20
```
where `10` and `20` are the start and end of the range of integers to distribute. and `4` is the number of processes.