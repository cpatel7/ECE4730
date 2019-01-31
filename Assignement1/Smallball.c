#include <stdio.h>
#include<mpi.h>
#include<stdlib.h>

int main(int argc, char **argv)
{
	MPI_Init(&argc, &argv);

	int rank = 0;
	int value;
	int flag = 1;
	int size = 0;

	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	
	// Get the name of the processor
    char processor_name[MPI_MAX_PROCESSOR_NAME];
    int name_len;
    MPI_Get_processor_name(processor_name, &name_len);
    
    if (rank != 0)
    {
    	MPI_Recv(&value, 1, MPI_INT, (rank-1), 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    	printf("Process %d received %d from process %d\n", rank, value, rank-1);
    	value++;
    }
    
    
    if (((rank+size)%size) == 0 && flag == 1) 
    {
		value = 0; 
		flag = 0;
    }
    
	MPI_Ssend(&value, 1, MPI_INT, (rank+1)%size , 0, MPI_COMM_WORLD);
    
    if(((rank+size)%size) == 0 && flag == 0)
    {
		MPI_Recv(&value, 1, MPI_INT, (size-1), 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		printf("Process %d received %d from process %d\n", rank, value, size-1);
    }
    
	MPI_Finalize();
}
