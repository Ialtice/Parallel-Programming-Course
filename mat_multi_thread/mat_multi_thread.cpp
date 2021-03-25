/*
 * mat_multi_thread.cpp
 *
 *  Created on: Nov 5, 2020
 *      Author: ITCS 3145 @ UNCC
 */

#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <iterator>
#include <ctime>
#include <thread>


// TODO include thread head file

using namespace std;

const int MAX_NAME_LENGTH = 500;
class Matrix {
private:
	vector< vector<double> > data;

public:
	Matrix() = default;
	Matrix(long rows, long cols) : data(rows, vector<double>(cols)) {};
	Matrix(const string &mat_file);
	void writeMatrix(string filename);

	long rows();
	long cols();
	double get(long rowIndex, long colIndex);
	void set(long rowIndex, long colIndex, double value);

	bool isMatrix();
	bool isMultipliable(Matrix& another);

	Matrix operator*(Matrix& another);

	void display();
	void push_back(vector<double> a_row);
	Matrix subMatrix(long first_row_index, long last_row_index,
			long first_col_index, long last_col_index);
	static void multipyThread(Matrix& a_mat, Matrix& b_mat, Matrix &c_matrix,
			long threadID, long numOfThreads);
};

/**
 * Read a file into 2D vector, the data member variable
 */
Matrix::Matrix(const string &mat_file) {
	ifstream inFile(mat_file.c_str());
	string inLine;
	while(getline(inFile, inLine)) {
	  istringstream inLineStream(inLine);
	  vector<double> inLineData(
		(istream_iterator<double>(inLineStream)),
		istream_iterator<double>());
	  data.push_back(inLineData);
	}
}

/**
 * Write matrix to a file
 */
void Matrix::writeMatrix(string filename) {
    ofstream myfile;
    myfile.open (filename);

	for (long i = 0; i < (long) (data.size()); i++) {
		myfile << data[i][0];
		for (long j = 1; j < (long) (data[i].size()); j++)
			myfile << "\t" << data[i][j];
		myfile << endl;
	}
	myfile.close();
}

/**
 * retrieve the number of rows of the matrix
 */
long Matrix::rows() {
	return data.size();
}

/**
 * retrieve the number of columns of the matrix
 */
long Matrix::cols() {
	return data[0].size();
}

/**
 * retrieve the element of the matrix at the index of rowIndex, colIndex
 */
double Matrix::get(long rowIndex, long colIndex) {
	return data[rowIndex][colIndex];
}

/**
 * set the element value in the rowIndex and colIndex of the matrix
 */
void Matrix::set(long rowIndex, long colIndex, double value) {
	data[rowIndex][colIndex] = value;
}

/**
 * examine whether all rows have the same number of columns
 */
bool Matrix::isMatrix() {
	long cols = data[0].size();
	for (long i = 1; i < (long) (data.size()); i++){
		if (cols != (long) (data[i].size()))
			return false;
	}
	return true;
}

/**
 * examine whether the matrix can be used to multiplied by another matrix
 */
bool Matrix::isMultipliable(Matrix& another) {
	return (this->cols() == another.rows());
}

/**
 * insert a row to the matrix
 */
void Matrix::push_back(vector<double> a_row) {
	data.push_back(a_row);
}

/**
 * multiply this matrix with another matrix
 */
Matrix Matrix::operator*(Matrix& another) {
	Matrix c_matrix;
	for (long i = 0; i < rows(); i ++) {
		vector<double> c_row;
		for (long j = 0; j < another.cols(); j ++) {
			double sum = 0;
			for (long k = 0; k < cols(); k ++){
				sum += data[i][k] * another.get(k, j);
			}
			c_row.push_back(sum);
		}
		c_matrix.push_back(c_row);
	}
	return c_matrix;
}

/**
 * display all elements in the matrix
 */
void Matrix::display(){
	// Displaying the 2D vector
	for (long i = 0; i < (long) (data.size()); i++) {
		for (long j = 0; j < (long) (data[i].size()); j++)
			cout << data[i][j] << " ";
		cout << endl;
	}
}

/**
 * retrieve submatrix of the matrix based on given bound of row start, row end,
 * column start and column end
 */
Matrix Matrix::subMatrix(long first_row_index, long last_row_index,
		long first_col_index, long last_col_index) {
	Matrix sub;
	for (long i = first_row_index; i <= last_row_index; i ++) {
		vector<double> c_row;
		for (long j = first_col_index; j < last_col_index; j ++)
			c_row.push_back(data[i][j]);

		sub.push_back(c_row);
	}
	return sub;
}

/**
 * matrix multiplication working thread function
 * parameters:
 * 		a_mat : matrix a  (input)
 * 		b_mat : matrix b  (input)
 * 		c_mat : matrix c  (output)
 * 		threadID          (input)
 * 		numOfThreads      (input)
 */
void Matrix::multipyThread(Matrix& a_mat, Matrix& b_mat, Matrix &c_matrix,
					long threadID, long numOfThreads) {
	//TODO calculate row-wise block size of matrix a
	int row_block_size = (a_mat.rows() + numOfThreads -1) / numOfThreads;

    //TODO calculate the first row index of the block of matrix a for the threadID
	int a_mat_first_row_index = threadID * row_block_size;

    //TODO calculate the last row index of the block of matrix a for the threadID
	int a_mat_last_row_index = a_mat_first_row_index + row_block_size -1 > a_mat.rows() -1 ?
			a_mat.rows() -1 : a_mat_first_row_index + row_block_size -1;

	// iteratively calculate the element value of matrix c in the row i and column j by
	// summing up corresponding multiplications of each element in i-th of matrix a with
	// an element in the corresponding position of the j-th column of matrix b
	for (long i = a_mat_first_row_index; i <= a_mat_last_row_index; i ++) {
		for (long j = 0; j < b_mat.cols(); j ++) {
			double sum = 0;
			for (long k = 0; k < a_mat.cols(); k ++){
				sum += a_mat.get(i, k) * b_mat.get(k, j);
			}

			//TODO set the sum value to the cell (i, j) of matrix c
			c_matrix.set(i, j, sum);

		}
	}
}

int main(int argc, char *argv[]){
	int num_of_thread = (int) strtol(argv[1], NULL, 10);
    Matrix a_mat(argv[2]); // read a matrix from the input file 1
    Matrix b_mat(argv[3]); // read a matrix from the input file 1

	time_t start = clock();
    //serial execution of matrix multiplication
    //Matrix c_mat = a_mat * b_mat; // matrix multiplication of matrix 1 and 2

    //parallel execution of matrix multiplication
    Matrix c_mat((long) (a_mat.rows()), (long) (b_mat.cols()));
    vector<thread> thread_pool;

    for (int i = 0; i < num_of_thread; i++) {
        // TODO create a thread with the working function Matrix::multipyThread
        // followed by ref(a_mat), ref(b_mat), ref(c_mat),i, num_of_thread
    	thread th(Matrix::multipyThread, ref(a_mat), ref(b_mat), ref(c_mat),i,num_of_thread); // @suppress("Invalid arguments")

    	// TODO push movable thread that is created in previous statement to thread_pool
    	thread_pool.push_back(move(th));
    }

    for (thread & th : thread_pool) {
    	//TODO main thread waits for each spawned thread to finish
    	//if the spawned thread is joinable
    	if (th.joinable())
    		th.join();
    }

    time_t end = clock();
    c_mat.writeMatrix(argv[4]);
    cout << "Matrix multiplication (A[ " << a_mat.rows() << " * " << a_mat.cols()
			<< "] * B[" << b_mat.rows() << " * " << b_mat.cols() << "] -> C["
			<< c_mat.rows() << " * " << c_mat.cols() << "]" << " takes " << (end - start)
						<< " clicks (" << ((end-start)*1000/(CLOCKS_PER_SEC + 0.0))
						<< " milliseconds)\n";

	return 0;
}
