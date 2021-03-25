#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <ctime>
#ifdef _OPENMP
#include <omp.h>
#endif

//TODO add OpenMP header file
using namespace std;
void mxv(int m, int n, double *a, double *b, double *c);

int main(int argc, char *argv[])
{
   double *a,*b,*c;
   int i, j, m, n;

	if (argc < 2) {
		cout << "Usage: helloOMP.exe ${number of threads}" << endl;
		exit(-1);
	}
	m = strtol(argv[1], NULL, 10);
	n = strtol(argv[0], NULL, 10);

	if ( (a=(double *)malloc(m*sizeof(double))) == NULL )
	  perror("memory allocation for a");
	if ( (b=(double *)malloc(m*n*sizeof(double))) == NULL )
	  perror("memory allocation for b");
	if ( (c=(double *)malloc(n*sizeof(double))) == NULL )
	  perror("memory allocation for c");

	printf("Initializing matrix B and vector c\n");
	for (j=0; j<n; j++)
		c[j] = 2.0;
	for (i=0; i<m; i++)
		for (j=0; j<n; j++)
			b[i*n+j] = i;

	printf("Executing mxv function for m = %d n = %d\n",m,n);
	time_t start = clock();
	mxv(m, n, a, b, c);

    time_t end = clock();
	cout << "Matrix-vector multiplication (a[ " << m << " * " << n
			<< "] * b[" << n << " * 1] -> c["
			<< m << " * 1]" << " takes " << (end - start)
			<< " clicks (" << ((end-start)*1000/(CLOCKS_PER_SEC + 0.0))
			<< " milliseconds)\n";
	free(a);free(b);free(c);
	return(0);
}

void mxv(int m, int n, double *a, double *b, double *c) {
	int i, j;

//TODO add OpenMP parallel directive to parallelize the calculation
#pragma omp parallel for  shared(a,b,c) private (i,j) num_threads(4)
	for (i=0; i<m; i++) {
		a[i] = 0.0;
		for (j=0; j<n; j++)
			a[i] += b[i*n+j]*c[j];
	}
}
