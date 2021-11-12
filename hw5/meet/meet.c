#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <mpi.h>


int getNext(int psize, int prank, int* msg)
{
        int pnext, found;
        int counter, i;

        srand(time(NULL));
        pnext = rand() % psize;
	if(pnext == prank)
		pnext = (pnext+1) % psize;

        found = 0;
	counter = msg[0];

	while(found != 1)
	{
	        for(i = 0; i < counter; ++i)
	                if(pnext == msg[i+1])
                        	break;

                if(i == counter)
                	found = 1;
                else
                        pnext = (pnext+1) % psize;

		if(pnext == prank)
			pnext = (pnext+1) % psize;
	}

	return pnext;
}

int main(int argc, char** argv)
{
	int psize;
	int prank;
	int ierr;
	MPI_Status status;
	const int MSG_TAG = 0;

	ierr = MPI_Init(&argc, &argv);
	ierr = MPI_Comm_size(MPI_COMM_WORLD, &psize);
	ierr = MPI_Comm_rank(MPI_COMM_WORLD, &prank);

	int* msg;
	msg = (int*)malloc(sizeof(int)*(psize+1));
	msg[0] = 0;

	if(prank != 0)
	{
		MPI_Recv(msg, psize+1, MPI_INT, MPI_ANY_SOURCE, MSG_TAG, MPI_COMM_WORLD, &status);
	}

        msg[0] += 1;
        msg[msg[0]] = prank;
	
	if (msg[0] < psize)
	{
        	int pnext;
        	pnext = getNext(psize, prank, msg);
		printf("rank_%d meets rank_%d\n", prank, pnext);
	
		MPI_Send(msg, psize+1, MPI_INT, pnext, MSG_TAG, MPI_COMM_WORLD);
	}

	free(msg);

	MPI_Finalize();

	return 0;
}
