#include <stdio.h>
#include <stdlib.h>
#include <string.h>


int main (int argc, char *argv[])
{
	if(argc != 2)
	{
		printf("usage: print-matrix input_file\n");
	}
	else
	{
		FILE *input;
		
		double val;
		int n;
		int m;
		int isMatrix = 1;
		
		input = fopen(argv[1], "rb");
		
		if (input == NULL)
		{
			printf("Could not open file:%s\n",argv[1]);
			exit(1);
		}
		else
		{
			fscanf(input,"%d %d", &n, &m);
			
			if(m == 1)
			{
				isMatrix = 0;
			}
			
			if (isMatrix)
			{
				printf("The matrix is: \n");
				int i = 1;
				
				while(fscanf(input, "%lf", &val) == 1)
				{		
					if (i%n == 0)
					{
						printf("%lf\n", val);
					}						
					else
					{	   
				    	printf("%lf, ", val);
			    	}
			    	i++;				    				    
				}				
			}
			else
			{
				printf("The vector is: \n");
				int i = 1;
				
				while(fscanf(input, "%lf", &val) == 1)
				{		
					if (i%n == 0)
					{
						printf("%lf\n", val);
					}						
					else
					{	   
				    	printf("%lf, ", val);
			    	}
			    	i++;				    				    
				}
			}		    
			
			fclose(input);
		}
	}
}
