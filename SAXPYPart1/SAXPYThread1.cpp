/*
 * SAXPYThread1.cpp
 *
 *  Created on: Nov 10, 2020
 *
 */


#include <iostream>
#include <random>
#include <thread>
#include <ctime>
using namespace std;

int randGen(const int& min, const int& max) {
    static thread_local mt19937 generator(hash<thread::id>()(this_thread::get_id())); // @suppress("Ambiguous problem")
    uniform_int_distribution<int> distribution(min, max);
    return distribution(generator);
}

void initialSAXPYSerial(long n, float* x, float* y) {
	if ((x == NULL) || (y == NULL)) return;

	for (int i = 0; i < n; i++) {
		x[i] = randGen(0, 10);
		y[i] = randGen(0, 10);
	}
}

void doSAXPYSerial(long n, float a, float* x, float* y) {
	if ((x == NULL) || (y == NULL)) return;

	for (long i = 0; i < n; i++)
		y[i] = a * x[i] + y[i];
}

/**
 * TODO thread function of initializing x and y in SAXPY
 */
void initialSAXPYThreadFunction(long n, float* x, float* y, int threadID, int numOfThreads) {
	if ((x == NULL) || (y == NULL)) return;

	// TODO Determine the block for each thread
	long block_size = (n + numOfThreads - 1) / numOfThreads;

	// TODO Calculate thread-specific start index
	long my_first = threadID * block_size;

	// TODO Calculate thread-specific end index
	long my_last = my_first + block_size - 1 > n -1 ? n -1 : my_first + block_size -1;

	cout << "Thread " << threadID << " numOfThreads = " << numOfThreads << " n = "
			<< n << " block_size = " << block_size << " my_first = "
			<< my_first << " my_last = " << my_last << endl;

	// Thread-specific initialization of x and y
	for (int i = my_first; i < my_last; i++) {
		x[i] = randGen(0, 10);
		y[i] = randGen(0, 10);
	}
}

/**
 * TODO thread function of computing SAXPY
 * SAXPY y[i] = a * x[i] + y[i], where i in the thread-specific local range
 */
void doSAXPYThreadFunction(int n, float a, float* x, float* y, int threadID, int numOfThreads) {
	if ((x == NULL) || (y == NULL)) return;

	// Determine the block for each thread
	int block_size = (n + numOfThreads - 1) / numOfThreads;

	// Calculate thread-specific start index
	int my_first = block_size * threadID;

	// Calculate thread-specific end index
	int my_last = my_first + block_size - 1 > n ? n : my_first + block_size - 1;

	cout << "Thread " << threadID << " numOfThreads = " << numOfThreads << " n = "
			<< n << " block_size = " << block_size << " my_first = "
			<< my_first << " my_last = " << my_last << endl;

	// TODO Thread-specific calculation of SAXPY
	for (int i = my_first; i <= my_last; i++){
		y[i] = a * x[i] + y[i];
	}

}

int main(){
	int digits = 0, numOfThreads = 0;
	float a = 0.0;

	// Decide how many threads for calculation
	cout << "How many threads for calculation? Please type an integer number: " << flush;
	scanf("%d", &numOfThreads);

	// Decide the vector dimension
	cout << "Please type an integer (less than 30): " << flush;
	scanf("%d", &digits);

	// Decide the vector dimension
	cout << "Please type a scalar value: " << flush;
	scanf("%f", &a);

	// Generate random vector x and y of n elements
	long n = 1 << digits;
	float *x = (float *) malloc (n * sizeof(float));
	float *y = (float *) malloc (n * sizeof(float));

	time_t start = clock();

	// TODO Initialize x and y of SAXPY in parallel
	vector<thread> thread_pool(numOfThreads);
	for (int threadID = 0; threadID < numOfThreads; threadID++){
		thread th (initialSAXPYThreadFunction, n, x, y, threadID,numOfThreads); // @suppress("Invalid arguments") // @suppress("Symbol is not resolved")
		thread_pool.push_back(move(th));
	}

	for(thread & th: thread_pool){
		if (th.joinable())
			th.join();
	}

	// TODO Calculate SAXPY in parallel
	for (int threadID = 0; threadID < numOfThreads; threadID++){
		thread th (doSAXPYThreadFunction, n, a, x, y, threadID,numOfThreads); // @suppress("Invalid arguments") // @suppress("Symbol is not resolved")
		thread_pool.push_back(move(th));
	}

	for(thread & th: thread_pool){
		if (th.joinable())
			th.join();
	}



	time_t end = clock();
	cout << "It takes " << (end - start)
		<< " clicks (" << ((end-start) * 1000 / (CLOCKS_PER_SEC + 0.0))
		<< " milliseconds)\n";
	return 0;
}
