#include <stdio.h>
#include <stdlib.h>

#define MAXBUF 1000

int main(int argc, char *argv[])
{
	FILE *input1;
	FILE *input2;
	FILE *output_file;

	if (argc != 4)
	{
		printf("#usage: ./mv-serial input_matrix1 input_vector2 output_vector3 \n");
	}
	else
	{
		//try openning the files
		input1 = fopen(argv[1], "rb");
		if (input1 == NULL)
		{
			printf("Error! Could not open the file %s", argv[1]);
			exit(1);
		}

		input2 = fopen(argv[2], "rb");
		if (input2 == NULL)
		{
			printf("Error! Could not open the file %s", argv[2]);
			fclose(input1);
			exit(1);
		}

		output_file = fopen(argv[3], "wb");
		if (output_file == NULL)
		{
			printf("Error! Could not open the file %s", argv[3]);
			fclose(input1);
			fclose(input2);
			exit(1);
		}

		int row1, col1, row2, col2;
		int i, j, k;
		double **matrix1, **matrix2, **output;

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
		matrix1 = (double **)calloc(row1, sizeof(double *));
		for(i=0; i<row1; i++)
		{
			matrix1[i] = (double *)calloc(col1, sizeof(double));
		}

		matrix2 = (double **)calloc(row2, sizeof(double *));
		for(i=0; i<row2; i++)
		{
			matrix2[i] = (double *)calloc(col2, sizeof(double));
		}

		output = (double **)calloc(row1, sizeof(double *));
		for(i=0; i<row1; i++)
		{
			output[i] = (double *)calloc(col2, sizeof(double));
		}

		//read matrix1 into memory
		
		for (i=0; i<row1; i++)
		{
			for (j=0; j<col1; j++)
			{
				if(fscanf(input1, "%lf", &matrix1[i][j]) != 1)
				{
					exit(1);
				}
			}
		}

		//read matrix2 into memory
		for (i=0; i<row2; i++)
		{
			for (j=0; j<col2; j++)
			{
				fscanf(input2, "%lf", &matrix2[i][j]);
			}
		}

		double product = 0.0;
		//multiply
		for (i=0; i<row1; i++)
		{
			for (j=0; j<col2; j++)
			{
				k = 0;
				while(k < row2)
				{
					product += (matrix1[i][k] * matrix2[k][j]);
					k++;
				}
				output[i][j] = product;

				//reset product
				product = 0.0;
			}
		}

		//store output in binary file
		fprintf(output_file, "%d %d\n", row1, col2);
		for (i=0; i<row1; i++)
		{
			for (j=0; j<col2; j++)
			{
				fprintf(output_file, "%lf", output[i][j]);
			}
			fprintf(output_file, "\n");
		}

		//free memory and close file
		i = 0;
		while (i<row1)
		{
			free(matrix1[i]);
			i++;
		}

		i = 0;
		while(i<row2)
		{
			free(matrix2[i]);
			i++;
		}

		i = 0;
		while (i<row1)
		{
			free(output[i]);
			i++;
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
