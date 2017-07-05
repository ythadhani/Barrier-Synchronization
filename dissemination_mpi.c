#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "mpi.h"

static int my_id, num_processors;
void dissemination_barrier()
{

	int tag=1; 
	int msg=1, dst, src;
	int j , num_rounds;
	
	MPI_Status mpi_result;
	
	num_rounds = ceil(log(num_processors)/log(2));
	
	for(j=0; j<num_rounds; j++)
	{
	 
	    dst = my_id + pow(2,j);
    	    dst = dst %num_processors;
    
            src = (my_id - pow(2,j) + num_processors);  
            src = src%num_processors;  
  	  
            MPI_Send(&msg, 1, MPI_INT, dst,tag, MPI_COMM_WORLD);
            MPI_Recv(&msg, 1,MPI_INT, src,tag, MPI_COMM_WORLD, &mpi_result);
        }
  
}

int main(int argc, char **argv)
{
  int barriers = 10;
  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &num_processors);
  MPI_Comm_rank(MPI_COMM_WORLD, &my_id);
  
  double t1,t2;
  int i;
  t1 = MPI_Wtime();
  for(i=0;i<barriers;i++)
  {
	dissemination_barrier();
  }
  t2 = MPI_Wtime();
  double time_taken = (t2-t1)*1000000;
  time_taken = time_taken/barriers;
  printf("%lf\n",time_taken);
 
  MPI_Finalize();
  return 0;
}
