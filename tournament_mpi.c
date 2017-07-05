#include <stdlib.h>
#include <stdio.h>
#include <mpi.h>
#include <math.h>
#include "gtmpi.h"

typedef struct round
{
	int role;
	int opponent;
	char flag;
	
}round_t;

static int WINNER = 0;
static int LOSER = 1;
static int BYE = 2;
static int CHAMPION = 3;
static int DROPOUT = 4;
static int UNUSED = 5;

static round_t** rounds;
static int num_procs, num_rounds;

void gtmpi_init(int num_threads)
{
	num_procs = num_threads;
	num_rounds = ceil(log(num_procs)/log(2)) +1;
	rounds  = (round_t**)malloc(num_procs*sizeof(round_t*));
	int i,k;

/*
Example with 8 processors:
       R0     R1     R2     R3      

P0    DROP   WIN    WIN     CHAMPION

P1    DROP   LOSE   UNUSED  UNUSED

P2    DROP   WIN    LOSE    UNUSED

P3    DROP   LOSE   UNUSED  UNUSED

P4    DROP   WIN    WIN     LOSE

P5    DROP   LOSE   UNUSED  UNUSED

P6    DROP   WIN    LOSE    UNUSED

P7    DROP   LOSE   UNUSED  UNUSED

*/
	
	for(i=0;i<num_procs;i++)
	{
		rounds[i] = (round_t*)malloc(num_rounds*sizeof(round_t));	
		for(k=0;k<num_rounds;k++)
		{
			int a = pow(2,k);
			int b = pow(2,k-1);
			rounds[i][k].flag = 0;
			if(k==0)
			{
				rounds[i][k].role = DROPOUT;
			}
			else if((i%a ==0) && ((i+b) < num_procs) && (a < num_procs))
			{
				rounds[i][k].role = WINNER;
			}
			else if((i % a ==0) && ((i + b) >= num_procs))
			{
				rounds[i][k].role = BYE;
			}
			else if( i % a == b)
			{
				rounds[i][k].role = LOSER;
			}
			else if((i==0) && a>=num_procs)
			{
				rounds[i][k].role = CHAMPION;
			}
			else
			{
				rounds[i][k].role = UNUSED;
			}
			
			if(rounds[i][k].role == LOSER)
			{
				rounds[i][k].opponent = i-b;
			}
			else if((rounds[i][k].role == WINNER) || (rounds[i][k].role == CHAMPION))
			{
				rounds[i][k].opponent = i+b;
			}
		}
	}
}

void gtmpi_barrier()
{
	int proc_id; 
	MPI_Comm_rank(MPI_COMM_WORLD,&proc_id);
	static char sense = 1;
	MPI_Status status;
	char break_loop = 0;
	int i=0; //round number
	while(!break_loop) //arrival
	{
		i=i+1;
		switch(rounds[proc_id][i].role)
		{
			case 0 :
				MPI_Recv(&rounds[proc_id][i].flag,sizeof(sense),MPI_CHAR,MPI_ANY_SOURCE,0,MPI_COMM_WORLD,&status);
				/*Winner waits for the loser to come and notify him that he has won hence MPI_Recv. This flips the flag at the 					position ascertained by the formulae in the MCS paper*/
				break;

			case 1 :
				MPI_Send(&sense,sizeof(sense),MPI_CHAR,rounds[proc_id][i].opponent,0,MPI_COMM_WORLD);
				//Loser needs to notify the winner that he has won
				MPI_Recv(&rounds[proc_id][i].flag,sizeof(sense),MPI_CHAR,MPI_ANY_SOURCE,0,MPI_COMM_WORLD,&status);
				/*This has to do with the wakeup phase when the winner of each round wakes up the loser. This essentially 					causes the loser to spin at this point until it is notified in the wakeup phase*/
				break_loop=1;
				break;

			case 2 :
				break;
	
			case 3 :
				MPI_Recv(&rounds[proc_id][i].flag,sizeof(sense),MPI_CHAR,MPI_ANY_SOURCE,0,MPI_COMM_WORLD,&status);
				MPI_Send(&sense,sizeof(sense),MPI_CHAR,rounds[proc_id][i].opponent,0,MPI_COMM_WORLD);
				break_loop=1;
				break;

			case 4 :
				break;

		}
	}
	
	break_loop = 0;

	while(!break_loop) //Wake-up phase
	{
		i = i-1;
		switch(rounds[proc_id][i].role)
		{
			case 0 :
				MPI_Send(&sense,sizeof(sense),MPI_CHAR,rounds[proc_id][i].opponent,0,MPI_COMM_WORLD);
				//Winner to wake up the loser in the wakeup phase
				break;

			case 1 :
				break;

			case 2 :
				break;

			case 3 :
				break;

			case 4 :
				break_loop = 1;
				break; /*Eventually as all processors awaken, we eventually reach this condition which causes the while loop 						to break*/
		}
	}
	sense = !sense; //Necessary to make sure that the processors don't fall past the next barrier
}

void gtmpi_finalize()
{
	int i;
	for(i=0;i<num_procs;i++)
	{
		free(rounds[i]);
	}
	free (rounds);
}
