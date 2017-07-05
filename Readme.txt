Instructions for compiling and running our code.

To compile: (using make)
For a standalone machine (Like a laptop):
In the MakeFile replace $(MPIJINX) with $(MPICC) and then type make

On jinx:
MakeFile will have $(MPIJINX). Then type make

Running the code on a standalone machine (like a laptop)
1. For OpenMP (Dissemination)
./dissemination_openmp (#of threads)
for example:
./dissemination_openmp 5

2. For OpenMP (Counting)
./counting_openmp (#of threads)
For example:
./counting_openmp 5

3. For Default OpenMP
./default_openmp (#of threads)
For example:
./default_openmp 5

4. For MPI (Dissemination)
mpirun -np (#of processes) dissemination_mpi
For example:
mpirun -np 5 dissemination_mpi

5. For MPI (Tournament)
mpirun -np (#of processes) tournament_mpi
For example:
mpirun -np 5 tournament_mpi

6. For Default MPI
mpirun -np (#of processes) default_mpi
For example:
mpirun -np 5 default_mpi


5. For OpenMP-MPI
mpirun -np (#of processes) openmp_mpi (#of threads)
For example:
mpirun -np 5 openmp_mpi 5

To run the code on Jinx, instead of mpirun, use /usr/lib64/openmpi/bin/mpirun (For MPI and OpenMP-MPI cases)
