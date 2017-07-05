
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "omp.h"
#include <sys/time.h>
#define NUM_ROUNDS 10000


// allnodes[i].partnerflags[r][k] points to allnodes[j].myflags[r][k]

int num_threads, num_rounds;
 
typedef struct flags {
	
	int myflags[2][NUM_ROUNDS];
	int *partnerflags[2][NUM_ROUNDS];  //because of 'points to'
} flags;



void dissemination_barrier(flags* local_flags, int* parity, int* sense){

  int i;
 
  for (i= 0; i<num_rounds; i++){

    #pragma omp critical
  {
     //setting partner flags to sense
    *local_flags->partnerflags[*parity][i] = *sense;

   }  
     //spinning on my flags not being sense
     while(local_flags->myflags[*parity][i]!=*sense);
 }


  //use of alternate sets of flags
  if (*parity == 1){
    *sense = !*sense;
  }
  
  *parity = 1-*parity;

}


int main(int argc, char **argv){

 

  if (argc==0){

    printf("Too less args\n");
  }


  num_threads=atoi(argv[1]);
     
  omp_set_num_threads(num_threads);  //setting number of threads/processors in main!
  
  //shared allnodes : array [0....P-1] of flags
  //array of structures
  
  flags all_nodes[num_threads];

  num_rounds = ceil(log(num_threads)/log(2)); //number rounds are in the global section of code
  //parallel section of code (going to implemented by multiple processors/threads)
  //need to keep consistency for the number of rounds (every thread must see the same value of num_rounds)

  
  double *thread_times = (double*)calloc(num_threads,sizeof(double));
  #pragma omp parallel shared(all_nodes,num_rounds)
  { 
  //you want the all_nodes structure and rounds to be shared amongst all procs
  //do we need private variables to a thread?
  
   int my_id, i, j, k, r, val;  
  my_id = omp_get_thread_num();                 //obtaining my ID
  
  int parity = 0;
  int sense = 1;
  
  
  flags* local_flags = &all_nodes[my_id];
  
  //initializing done by a single thread
  #pragma omp single
  {
  for(i= 0 ; i<num_threads ; i++){
    for(r= 0; r<2; r++ ){
  	  for (k = 0; k<num_rounds; k++){
           all_nodes[i].myflags[r][k] = 0 ;
  	  }
  	}
  }
}
  
    #pragma omp critical
    {
    for(j = 0; j<num_threads;j++){
  	  for (k=0; k<num_rounds; k++){
  	    val = pow(2,k);
  		    if (j == ((my_id+val)%num_threads)) {
            for(r=0; r<2;r++){
  			      all_nodes[my_id].partnerflags[r][k] = &all_nodes[j].myflags[r][k];
 
 	 	        }
          }
        }
      }
    }

      double t1,t2;
				
      int iter;
      int barriers = 10;
      t1 = omp_get_wtime();
      for(iter=0;iter<barriers;iter++)
      {
		dissemination_barrier(local_flags,&parity,&sense);
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

