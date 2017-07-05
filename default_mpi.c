#include <stdio.h>
#include <mpi.h>

int main(int argc, char *argv[])
{
  int my_id, num_processes;
  int barriers = 10;
  MPI_Init(&argc, &argv);

  MPI_Comm_size(MPI_COMM_WORLD, &num_processes);
  MPI_Comm_rank(MPI_COMM_WORLD, &my_id);
  
  double t1,t2;
  int i;
  t1 = MPI_Wtime();
  for(i=0;i<barriers;i++)
  {
	MPI_Barrier(MPI_COMM_WORLD);
  }
  t2 = MPI_Wtime();
  double time_taken = (t2-t1)*1000000;
  time_taken = time_taken/barriers;
  printf("%lf\n",time_taken);
	  
  MPI_Finalize();

  return 0;
}

