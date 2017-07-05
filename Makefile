CC = gcc
MPICC = mpicc
MPIJINX = /usr/lib64/openmpi/bin/mpicc

all: counting_openmp dissemination_openmp dissemination_mpi tournament_mpi openmp_mpi default_mpi default_openmp

counting_openmp: 
	$(CC) -g counting_openmp.c test_counting_openmp.c -o counting_openmp -fopenmp

dissemination_openmp:
	$(CC) -g dissemination_openmp.c -o dissemination_openmp -fopenmp -lm

dissemination_mpi:
	$(MPICC) dissemination_mpi.c -o dissemination_mpi -lm

tournament_mpi:
	$(MPICC) tournament_mpi.c test_tournament_mpi.c -o tournament_mpi -lm

openmp_mpi:
	$(MPICC) openmp_mpi.c -o openmp_mpi -lm -fopenmp

default_mpi:
	$(MPICC) default_mpi.c -o default_mpi

default_openmp: 
	$(CC) -g default_openmp.c -o default_openmp -fopenmp

clean:
	rm -f *.o counting_openmp dissemination_openmp dissemination_mpi tournament_mpi openmp_mpi default_mpi default_openmp


	





