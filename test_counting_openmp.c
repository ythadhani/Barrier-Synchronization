#include <stdio.h>
#include "gtmp.h"
#include <omp.h>
#include<stdlib.h>

int main(int argc, char *argv[])
{
	int barriers = 10;
	int num_threads = atoi(argv[1]);
	
	gtmp_init(num_threads);
	omp_set_num_threads(num_threads);
	double *thread_times = (double*)calloc(num_threads,sizeof(double));
	
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
	int j;
	for(j=0;j<num_threads;j++)
	{
		printf("%lf\n",thread_times[j]);
	}
	free(thread_times);
	return 0;
}
