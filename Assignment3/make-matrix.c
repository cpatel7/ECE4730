#include <stdio.h>
#include <stdlib.h>
#include <string.h>


int main (int argc, char *argv[])
{
	int i = 0;
	int n = 0; //size of the matrix/vector
	int minRandom = 0;
	int maxRandom = 0;
	int isMatrix = 1;
	
	if (argc != 10 && argc != 9)
	{
		printf("usage: ./make-matrix -n matrix-size -l lower-bound -u upper-bound -o output-file \n");
		printf("usage: ./make-matrix -v -n vector-size -l lower-bound -u upper-bound -o output-file \n");
	}
	else
	{
		for (i=0; i<argc; i++)
		{
			if(i+1 < argc)
			{
				if (strcmp(argv[i], "-n") == 0)
				{
					n = atoi(argv[i+1]);
				}
				
				if (strcmp(argv[i], "-l") == 0)
				{
					minRandom = atoi(argv[i+1]);
				}
				
				if (strcmp(argv[i], "-u") == 0)
				{
					maxRandom = atoi(argv[i+1]);
				}

				if(strcmp(argv[i], "-v") == 0)
				{
					isMatrix = 0;
				}

				if (strcmp(argv[i], "-o") == 0)
				{					
					FILE *fp;
					fp = fopen(argv[i+1], "wb");
					
					if (fp == NULL)
					{
						printf("Error! Could not open/create file: %s",argv[i+1]);
					}
					else
					{						
						if(isMatrix)
						{
							fprintf(fp,"%d %d\n",n,n);						

							int j = 0; int i = 0;

							for(j=0; j<n; j++)
							{
								for(i=0;i<n;i++)
								{
									double num = 0.0;
									num = ((double)rand() * (maxRandom - minRandom)) / (double)RAND_MAX + minRandom;
									
									fprintf(fp,"%lf",num);
																	
								}
								
								fprintf(fp,"\n");							
							}														
						}
						else
						{
							fprintf(fp,"%d %d\n",n,1);
							
							int k = 0;

							for(k=0; k<n; k++)
							{
								double num = 0.0;
								num = ((double)rand() * (maxRandom - minRandom)) / (double)RAND_MAX + minRandom;

								fprintf(fp,"%lf",num);
							}					
							
							fprintf(fp,"\n");		
						}
						
						fclose(fp);
					}
					
				}
			}
		}
	}
	return 0;
}
