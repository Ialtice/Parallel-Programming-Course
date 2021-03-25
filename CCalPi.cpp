/*
 ============================================================================
 Name        : CCalPi.cpp
 Author      : Isaac Altice
 Description : Calculate pi which is
               the summation of 4 * (-1)^i / (2*i+1), where i>=1 and i < n
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <ctime>
#include <time.h>
#include <math.h>
#include <mpi.h>

using namespace std;

class PI{
public:
	double computingPiSerial(int n);
	double computingPiByP2PMPI(int n);
	double computingPiByPTreeMPI(int n);
	double computingPiByCollectiveMPI(int n);
private:
	double calculateLocalSum(int last, int my_rank, int p);
};

int main(int argc, char *argv[]) {
	int n;
	n = (int) strtol(argv[1], NULL, 10);
	PI pi;

	//pi.computingPiSerial(n);
	MPI_Init(&argc, &argv);
	pi.computingPiByP2PMPI(n);
	//pi.computingPiByPTreeMPI(n);
	//pi.computingPiByCollectiveMPI(n);
	MPI_Finalize();


	return EXIT_SUCCESS;
}

double PI::computingPiSerial(int n){
	double sum = 0;
	double factor;

	for (long i = 0; i < n; i++){
		if (i % 2 == 0){
			factor = 1.0;
		} else {
			factor = -1.0;
		}

		sum = sum + 4 * factor / (2*i+1);
	}
	printf("Pi sum in serial is equal to %f\n", sum);

	return sum;
}
double PI::calculateLocalSum(int n, int my_rank, int p){
	double sum = 0;
	double factor;
	int chunk_size = (n + p) / p;
	int my_First_Index = my_rank * chunk_size;
	int my_first =  my_First_Index;
	int my_last = (my_first + chunk_size - 1 > n ) ? n: my_first + chunk_size -1;

	for (long i = my_first; i < my_last; i++){
		if (i % 2 == 0){
			factor = 1.0;
		} else {
			factor = -1.0;
		}
		sum = sum + 4 * factor / (2*i+1);
	}

	return sum;
}
double PI::computingPiByP2PMPI(int n){
	double sum;
	int my_rank, p;
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
	MPI_Comm_size(MPI_COMM_WORLD, &p);
	int start = clock();
	double my_sum = calculateLocalSum(n, my_rank,p);

	int tag = 0;
	if (my_rank == 0){ // the master task
			MPI_Status status;
			sum = my_sum;
			double tmp;
			for (int source = 1; source < p; source++) {
				MPI_Recv(&tmp, 1, MPI_DOUBLE, source, tag,MPI_COMM_WORLD, &status);
				sum += tmp;

			}
			cout << "Sum parallel (linear) from 0" << " to " << n << ": " << sum << endl;
			int end = clock();
			cout << "Sum in parallel (linear) takes " << (end - start)
				<< " clicks (" << ((end-start)/(CLOCKS_PER_SEC+0.0))
				<< " seconds, " << ((end-start)*1000/(CLOCKS_PER_SEC + 0.0)) << "milliseconds)\n";
		}
		else{  // worker tasks
			/* create message */
				int dest = 0;
				MPI_Send(&my_sum, 1, MPI_DOUBLE,dest, tag, MPI_COMM_WORLD);
		}

	return sum;
}
double PI::computingPiByPTreeMPI(int n){
	int rank,p;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &p);
	int start =clock();
	double my_sum = calculateLocalSum(n,rank,p);

	int tag = 1;
	MPI_Status status;
	double sum = my_sum, tmp =0;

	int iLevel = 0;
	for (int stride =1; stride < p; stride *=2){
		if(rank % stride ==0){
			if(rank % (2 *stride) == 0){
				MPI_Recv(&tmp, 1, MPI_DOUBLE, rank + stride, tag, MPI_COMM_WORLD, &status);
				sum +=tmp;
			}
			else{
				MPI_Send(&sum,1,MPI_DOUBLE, rank - stride, tag, MPI_COMM_WORLD);
			}
		}
		iLevel++;
	}
	if(rank == 0){
		cout << "Sum parallel (tree) from 0" << " to " << n << ": " << sum << endl;
		int end = clock();
		cout << "Sum in parallel (tree) takes " << (end - start)
			<< " clicks (" << ((end-start)/(CLOCKS_PER_SEC+0.0))
			<< " seconds, " << ((end-start)*1000/(CLOCKS_PER_SEC + 0.0)) << "milliseconds)\n";
	}
	return sum;
}
double PI::computingPiByCollectiveMPI(int n){
	double sum = 0;
	int start = clock();
	int rank,p;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &p);

	double my_sum = calculateLocalSum(n,rank,p);

	int master = 0;
	MPI_Reduce(&my_sum, &sum,1,MPI_DOUBLE, MPI_SUM, master, MPI_COMM_WORLD);

	if (rank == master){
		cout << "Sum parallel (Collective) from 0" << " to " << n << ": " << sum << endl;
		int end = clock();
		cout << "Sum in parallel (Collective) takes " << (end - start)
			<< " clicks (" << ((end-start)/(CLOCKS_PER_SEC+0.0))
			<< " seconds, " << ((end-start)*1000/(CLOCKS_PER_SEC + 0.0)) << "milliseconds)\n";
	}
	return sum;
}
