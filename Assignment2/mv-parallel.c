#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <string.h>
#include <math.h>

int main(int argc, char *argv[])
{
	FILE *input1;
	FILE *input2;
	FILE *output_file;

	if (argc != 4)
	{
		printf("#usage: ./mpirun -np __ mv-parallel input_matrix1 input_vector2 output_vector3 \n");
		exit(1);
	}
	else
	{
		//try openning the files
		input1 = fopen(argv[1], "rb");
		if (input1 == NULL)
		{
			printf("Error! Could not open the file %s\n", argv[1]);
			exit(1);
		}

		input2 = fopen(argv[2], "rb");
		if (input2 == NULL)
		{
			printf("Error! Could not open the file %s\n", argv[2]);
			fclose(input1);
			exit(1);
		}

		output_file = fopen(argv[3], "wb");
		if (output_file == NULL)
		{
			printf("Error! Could not open the file %s\n", argv[3]);
			fclose(input1);
			fclose(input2);
			exit(1);
		}

		/* variables for matrix allocation in memory */
		int row1, col1, row2, col2;
		int i, j;
		double *matrix1, *matrix2, *output;

		fscanf(input1,"%d %d", &row1, &col1);
		fscanf(input2,"%d %d", &row2, &col2);

		//check if the matrix/vector from file1 is multiply-able to matrix/vector from file2
		if(row1 != col2)
		{
			printf("Error! Matrix/vector from file 1 and matrix/vector from file 2 must be row-column equivalent\n" );
			fclose(input1);
			fclose(input2);
			fclose(output_file);
			exit(1);
		}

		//at this point everything should be good so allocate the memory blocks
		matrix1 = (double *)calloc(row1*col1, sizeof(double *));

		matrix2 = (double *)calloc(row2*col2, sizeof(double *));

		output = (double *)calloc(row1*col1, sizeof(double *));

		//read matrix1 into memory
		
		for (i=0; i<row1; i++)
		{
			if(fscanf(input1, "%lf", &matrix1[i]) != 1)
			{
				exit(1);
			}
		}

		//read matrix2 into memory
		for (i=0; i<row2; i++)
		{
			if(fscanf(input2, "%lf", &matrix2[i]) != 1)
			{
				exit(1);
			}
		}

		/* Parallel Implementation */

		double *partial_product;
		int numProcs, gridsz[2];
		int period[2] = {0, 0}; /* DO NOT WRAP AROUND */
		int rank, rank_2d, coords[2];
		int cart_rank, cart_coords[2]; /* local rank and coordinate in 2D topology */
		MPI_Comm comm_2d, rowcomm, colcomm;
		int cart_size; 
		int ROW = 0; int COL = 1; //to keep track of dimensions of cart topology
		double start, end;

		MPI_Init(&argc, &argv);

		MPI_Comm_size(MPI_COMM_WORLD, &numProcs);
	 	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	 	/* computing the size of the grid */
	 	gridsz[ROW] = gridsz[COL] = sqrt(numProcs);

	 	cart_size = numProcs/gridsz[ROW];

	 	/* Allocate memory for partial dot products */
	 	partial_product = malloc(cart_size*sizeof(double));

	 	MPI_Cart_create(MPI_COMM_WORLD, 2, gridsz, period, 0, &comm_2d);

	 	MPI_Comm_rank(comm_2d, &rank_2d);           /* get 2D rank … */
		MPI_Cart_coords(comm_2d, rank_2d, 2, coords);  /* then 2D coords */

		/* create row and col based sub topologies */

		gridsz[ROW] = 0;
		gridsz[COL] = 1;
		MPI_Cart_sub(comm_2d, gridsz, &rowcomm);

		gridsz[ROW] = 1;
		gridsz[COL] = 0;
		MPI_Cart_sub(comm_2d, gridsz, &colcomm);

		/* Split input2 and distribute it across the processors in the grid */
		if (coords[COL] == 0 && coords[ROW] != 0) //first column
		{
			cart_coords[ROW] = coords[ROW];
			cart_coords[COL] = coords[ROW];
			MPI_Cart_rank(comm_2d, cart_coords, &cart_rank);
			MPI_Send(matrix2, cart_size, MPI_DOUBLE, cart_rank, 1, comm_2d);
		}
		if (coords[ROW] == coords[COL] && coords[ROW] != 0)
		{
			cart_coords[ROW] = coords[ROW];
			cart_coords[COL] = 0;
			MPI_Cart_rank(comm_2d, cart_coords, &cart_rank);
			MPI_Recv(matrix2, cart_size, MPI_DOUBLE, cart_rank, 1, comm_2d, MPI_STATUS_IGNORE);
		}

		/* do a column-wise bcast */
		cart_coords[0] = coords[COL];
		MPI_Cart_rank(colcomm, cart_coords, &cart_rank);
		MPI_Bcast(matrix2, cart_size, MPI_DOUBLE, cart_rank, colcomm);

		/* now that we have set up the topology and distributed the matrix2 
		 * we can compute the product and save in local memory 
	 	*/
		
		start = MPI_Wtime(); 
		for(i=0; i<cart_size; i++)
		{
			partial_product[i] = 0.0; //reset the values
			for(j=0; j<cart_size; j++)
			{
				partial_product[i] += matrix1[i*cart_size+j]*matrix2[j];
			}
		}
		end = MPI_Wtime();
		printf("Elapsed Time: %.20f\n", end-start);
		/* do the sum reduction along the rows */
		cart_coords[0] = 0;
		MPI_Cart_rank(rowcomm, cart_coords, &cart_rank);
		MPI_Reduce(partial_product, output, cart_size, MPI_DOUBLE, MPI_SUM, cart_rank, rowcomm);
			
		MPI_Comm_free(&comm_2d);
		MPI_Comm_free(&rowcomm);
		MPI_Comm_free(&colcomm);
		
		MPI_Finalize();

		/* write the output to the output file */

		fprintf(output_file, "%d %d\n", row1, col1);
		for (i=0; i<row1; i++)
		{
			for(j=0; j<col1; j++)
			{
				fprintf(output_file, "%lf", output[j]);
			}
			fprintf(output_file, "\n");
		}

		free(matrix1);
		free(matrix2);
		free(output);

		fclose(input1);
		fclose(input2);
		fclose(output_file);

		return 0;
	}
}
