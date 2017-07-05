#include <omp.h>
#include "gtmp.h"


static int number_of_threads;
static int count;
static char global_sense;

void gtmp_init(int num_threads)
{
	number_of_threads = num_threads;
	global_sense = 1;
	count = number_of_threads;
}

void gtmp_barrier()
{
	char local_sense;
	local_sense= !global_sense;
	//This critical section must be executed by only one thread at a time
	#pragma omp critical
    	{
      		count = count -1;
		if(count==0)
		{
			count = number_of_threads;
			global_sense = local_sense;
		}      	
    	}
	while(global_sense!=local_sense);  
        //Spinning on sense reversal ensures that threads don't fall past consecutive barriers when count is set to zero
}

void gtmp_finalize()
{

}

