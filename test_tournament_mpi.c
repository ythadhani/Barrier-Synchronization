#include <stdio.h>
#include "gtmpi.h"
#include <mpi.h>

int main(int argc, char *argv[])
{
  int my_id, num_processes;
  int barriers = 10;
  MPI_Init(&argc, &argv);

  MPI_Comm_size(MPI_COMM_WORLD, &num_processes);
  MPI_Comm_rank(MPI_COMM_WORLD, &my_id);
  gtmpi_init(num_processes);
  
  double t1,t2;
  int i;
  //Returns an elapsed wall clock time in seconds on the calling processor. 
  t1 = MPI_Wtime();
  for(i=0;i<barriers;i++)
  {
	gtmpi_barrier();
  }
  t2 = MPI_Wtime();
  double time_taken = (t2-t1)*1000000;
  time_taken = time_taken/barriers;
  printf("%lf\n",time_taken);
	  
  gtmpi_finalize();
  MPI_Finalize();

  return 0;
}

