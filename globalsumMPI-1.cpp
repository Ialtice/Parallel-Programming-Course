/*
 * globalSumMPI.cpp
 *
 *  Created on: Oct 27, 2020
 *      Author: Isaac Altice
 */

#include <iostream>
#include <ctime>
#include <time.h>
#include <math.h>
#include <mpi.h>

using namespace std;

class Sum{

public:
	int globalSumSerial(int first, int end);
	int globalSumMPILinear(int first, int last);
	int globalSumMPITree(int first, int last);
	int globalSumMPIReducer(int first, int last);

private:
	int calculateLocalSum(int first, int last, int my_rank, int p);
};

int Sum::globalSumSerial(int first, int last){
	int start = clock();
	int sum = 0;
	for (int i = first; i <= last; i++)
		sum += i;
	int end = clock();
	cout << "Global Sum in linear takes " << (end - start) << " clicks ("
	           << ((end-start) * 1000 / (CLOCKS_PER_SEC + 0.0)) << " milliseconds)\n";


	return sum;
}

int Sum::calculateLocalSum(int first, int last, int my_rank, int p){
	int sum = 0;
	int chunk_size = (last - first + p) / p;
	int my_First_Index = my_rank * chunk_size;
	int my_first = first + my_First_Index;
	int my_last = (my_first + chunk_size - 1 > last ) ? last: my_first + chunk_size -1;


	for (int i = my_first; i <= my_last; i++)
		sum += i;

	return sum;
}
int Sum::globalSumMPILinear(int first, int last){
	int sum = 0;

	int my_rank, p;
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
	MPI_Comm_size(MPI_COMM_WORLD, &p);
	int start = clock();
	int my_sum = calculateLocalSum(first, last, my_rank,p);

	int tag = 0;
	if (my_rank == 0){ // the master task
			MPI_Status status;
			printf("Hello From process 0: Num processes: %d\n",p);
			sum = my_sum;
			int tmp;
			for (int source = 1; source < p; source++) {
				MPI_Recv(&tmp, 1, MPI_INT, source, tag,MPI_COMM_WORLD, &status);
				sum += tmp;
			}
			cout << "Rank: " << my_rank << " Global sum parallel from " << first << " to " << last << ": " << sum << endl;
			int end = clock();
			cout << "Global Sum in parallel  takes " << (end - start)
				<< " clicks (" << ((end-start)/(CLOCKS_PER_SEC+0.0))
				<< " seconds, " << ((end-start)*1000/(CLOCKS_PER_SEC + 0.0)) << "milliseconds)\n";
		}
		else{  // worker tasks
			/* create message */
				int dest = 0;
				MPI_Send(&my_sum, 1, MPI_INT,dest, tag, MPI_COMM_WORLD);
		}

	return sum;
}
int Sum::globalSumMPITree(int first, int last){
	int rank,p;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &p);
	int start =clock();
	int my_sum = calculateLocalSum(first,last,rank,p);

	int tag = 1;
	MPI_Status status;
	int sum = my_sum, tmp =0;

	int iLevel = 0;
	for (int stride =1; stride < p; stride *=2){
		if(rank % stride ==0){
			if(rank % (2 *stride) == 0){
				MPI_Recv(&tmp, 1, MPI_INT, rank + stride, tag, MPI_COMM_WORLD, &status);
				sum +=tmp;
			}
			else{
				MPI_Send(&sum,1,MPI_INT, rank - stride, tag, MPI_COMM_WORLD);
			}
		}
		iLevel++;
	}
	if(rank == 0){
		cout << "Sum in parallel (tree) from " << first << " to " << last << " is equal to " << sum << endl;
		int end = clock();
		cout << "Global Sum in Parallel (Tree) takes " << (end-start) << " clicks (" << ((end-start) * 1000
				/ (CLOCKS_PER_SEC + 0.0)) << " milliseconds)" << endl;
	}
	return sum;
}
int Sum::globalSumMPIReducer(int first, int last){
	int sum = 0;
	int start = clock();
	int rank,p;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &p);

	int my_sum = calculateLocalSum(first,last,rank,p);

	int master = 0;
	MPI_Reduce(&my_sum, &sum,1,MPI_INT, MPI_SUM, master, MPI_COMM_WORLD);

	if (rank == master){
		int end = clock();
		cout << "Global sum (MPI Reduce) takes " << (end-start) <<" clicks (" << ((end-start) * 1000
				/ (CLOCKS_PER_SEC + 0.0)) << " milliseconds)" << endl;
		cout << "Sum = " << sum << endl;
	}
	return sum;
}

int main(int argc, char *argv[]){
	// Preparation
	int first = (int) strtol(argv[1], NULL, 10);
	int last = (int) strtol(argv[2], NULL, 10);


	Sum sum;
	sum.globalSumSerial(first, last);

	MPI_Init(&argc, &argv);



	sum.globalSumMPILinear(first, last);

	sum.globalSumMPITree(first,last);

	sum.globalSumMPIReducer(first,last);

	MPI_Finalize();

	return 0;
}
