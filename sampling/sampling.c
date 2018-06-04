#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <math.h>

struct timeval sampletime;

//Time values
double t_time, seq_time, temp_time = 0.0;

//Statisctics values
double  *samples, sum, median, std_dev;

int T, N, i = 0, j;
FILE *fp;

//Function to sort samples in ascending order
int compare (const void *a, const void *b)
{
	double* arg1 = (double *)a;
	double* arg2 = (double *)b;
	if( *arg1 < *arg2 ) return -1;
	else if( *arg1 == *arg2 ) return 0;
	return 1;
}

void main(int argc, char *argv[])
{
	//Process input parameters
	if ( argc == 3 )
	{
		T = atoi(argv[1]); // The time between each recording is defined by the user when he calls the process (T in microseconds).
		N = atoi(argv[2]); // Number of samples defined by the user when he calls the process.
	}
	else
	{
		printf("Wrong number of arguments\n");
		exit(1);
	}
	
	samples = (double*) malloc(N * sizeof(double));
	
	N++; //increasing number of samples by one, the first sample gets discarded (no relative computation possible)
	
	while (1)
	{
		gettimeofday( &sampletime, NULL );
			
		seq_time = (double) (sampletime.tv_usec / 1.0e6 + sampletime.tv_sec); // current timesatmp (saved as sec,msec);
		t_time = seq_time - temp_time; // last timestamp gets substracted from current one
	
		//Keep samples from the second and on		
		if (!((N-1) == atoi(&argv[2][0])))
		{
			samples[i] = t_time;
			i++;
			sum = sum + t_time;
		}
	
		temp_time = seq_time; //copy current timestamp to be the next-last timestamp
		
		N--; // reducing remaining samples by 1 (works as loop counter)
		usleep(T);
		
		if (N == 0) break; // when all samples are taken, end the process
	}
	
	//Sort samples for statistical analysis
	qsort(samples, i, sizeof(samples[0]), compare);
		
	//Minimum, Maximum calculation
	//Minimum = samples[0]
	//Maximum = samples[i-1]
	
	//Mean calculation
	double mean = sum / i;
	
	//Median calculation
	if(i % 2 == 0)
		median = (samples[i / 2] + samples[i / 2 + 1]) / 2;
    else
		median = samples[i / 2 + 1];
	
	//Standard deviation calculation
	for (j = 0; j < i; j++)
		std_dev += pow(samples[j] - mean, 2);
	std_dev = sqrt(std_dev / i);
	
	//DONE SAMPLING
	//Writing results in a file
	
	fp = fopen ("results.txt", "w");
	if (fp == NULL)
	{
		printf("Error opening file!\n");
		exit(1);
	} 
	
	for (j = 0; j < i; j++)
		fprintf(fp, "%f\n", samples[j]);
	
	fprintf(fp, "Minimum: %f\n", samples[0]);
	fprintf(fp, "Maximum: %f\n", samples[i-1]);
	fprintf(fp, "Mean: %f\n", mean);
	fprintf(fp, "Median: %f\n", median);
	fprintf(fp, "Standard Deviation: %f\n", std_dev);
	
	fclose(fp);
	exit(0);
}
