#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "mpi.h"
#include <omp.h>

static int number_of_threads;
static char global_sense;
static int count;
static int num_processors, my_id;

void gtmp_init(int num_threads)
{
	number_of_threads = num_threads;
	global_sense = 1;
	count = number_of_threads;
}

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

void gtmp_barrier()
{
	char local_sense;
	local_sense= !global_sense;
	#pragma omp critical
    	{
      		count = count -1;
		if(count==0)
		{
			count = number_of_threads;
			dissemination_barrier();
			global_sense = local_sense;
		}      	
    	}
	while(global_sense!=local_sense);
}

int main(int argc, char **argv)
{
  int barriers = 10;
  int no_of_threads = atoi(argv[1]);
  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &num_processors);
  MPI_Comm_rank(MPI_COMM_WORLD, &my_id);
  

  omp_set_num_threads(no_of_threads);
  gtmp_init(no_of_threads);
  double *thread_times = (double*)calloc(number_of_threads,sizeof(double));

  #pragma omp parallel
  {
    double t1,t2;
				
    int i;
    t1 = omp_get_wtime();
    for(i=0;i<barriers;i++)
    {
	gtmp_barrier();
    }
    t2 = omp_get_wtime();
    double time_taken = (t2-t1)*1000000;
    thread_times[omp_get_thread_num()] = time_taken/barriers;
  }  
  double sum=0;
  int j;
  for(j=0;j<number_of_threads;j++)
  {
	sum =sum+thread_times[j];
  }
  printf("%lf\n",sum/number_of_threads);
  free(thread_times);
  MPI_Finalize();
  return 0;
}
