/*
 * test.cpp
 *
 *  Created on: Nov 25, 2020
 *      Author: ITCS 3145
 *
 */

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
//TODO add OpenMP related head file
#ifdef _OPENMP
#include <omp.h>
#else
#define omp_get_thread_num() 0
#define omp_get_num_threads() 0
#endif


using namespace std;

char message[] = "Happy Thanksgiving!";
void hello() {
	int my_rank = omp_get_thread_num(); // @suppress("Invalid arguments")//TODO retrieve thread rank
	int thread_count = omp_get_num_threads(); // @suppress("Invalid arguments")//TODO retrieve total number of threads

	printf("%s [from %d out of %d threads]\n", message, my_rank, thread_count);
}

int main(int argc, char *argv[]){
	if (argc < 2) {
		cout << "Usage: helloOMP.exe ${number of threads}" << endl;
		exit(-1);
	}
	int threads = strtol(argv[1], NULL, 10);
	cout << "start" << endl;
	printf("---------------Enter parallel region-------------\n");
	//TODO add OpenMP parallel directive with the threads as the input to
	//     set up the number of threads that the parallel construct need to use
	#pragma omp parallel num_threads(threads)
	{
		hello();
		if (omp_get_thread_num() == 0)
			printf("---------------Main Thread do...-------------\n");
	}


	printf("---------------Exit parallel region-------------\n");
	cout << "end" << endl;
	return 0;
}



