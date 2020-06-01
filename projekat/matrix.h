#pragma once

#include <vector>
#include <fstream>
#include <tbb/parallel_for.h>
#include <tbb/blocked_range2d.h>
#include <tbb/task.h>
#include <tbb/task_scheduler_init.h>
#include <list>

class matrix {
private:
	int rows, cols;
	std::vector<std::vector<int>> elements;
public:
	matrix();
	matrix(int, int);
	matrix(const matrix& m);

	int getRows();
	int getCols();
	void reset(); // resets all matrix elements to 0


	// computes the result matrix using single thread
	friend void serial_multiply(const matrix&, const matrix&, matrix&);

	// computes the result matrix using parallel_for
	friend void parallel_multiply(const matrix&, const matrix&, matrix&);

	// that spawns one task per element of the result matrix
	friend void el_per_thread_multiply(const matrix&, const matrix&, matrix&);

	// spawns one task per row/column (dimension) of the result matrix
	friend void dimension_per_thread_multiply(const matrix&, const matrix&, matrix&);

	// calculates the result using N tasks
	friend void hyperthreading_multiply(const matrix&, const matrix&, matrix&);

	friend std::istream& operator>>(std::istream& input, matrix& m);
	friend std::ostream& operator<<(std::ostream& output, const matrix& m);

	// checks if dimensions of matrixes are valid for multiplication
	friend bool check_dimensions(const matrix&, const matrix&);

	friend class parallel_for_functor;
	friend class matrix_element_task;
	friend class matrix_dimension_task;
	friend class hyperthread_task;
};

// Functor that contains the body of the parallel_for method
class parallel_for_functor {
private:
	const matrix& m1, m2;
	matrix& m3;
public:
	parallel_for_functor(const matrix&, const matrix&, matrix&);
	void operator()(const tbb::blocked_range2d<int>&) const;
		
};

// used to create tasks that calculate one element per thread
class matrix_element_task : public tbb::task {
private:
	int row, col;
	const matrix& m1, m2;
	matrix& result;
public:
	std::list<matrix_element_task*> tasks;
	matrix_element_task(int, int, const matrix&, const matrix&, matrix&);
	tbb::task* execute();
};

// used to create tasks that calculate one entire row/col per thread
class matrix_dimension_task : public tbb::task {
private:
	int d;
	const matrix& m1, m2;
	matrix& result;
public:
	std::list<matrix_dimension_task*> tasks;
	matrix_dimension_task(int, const matrix&, const matrix&, matrix&);
	tbb::task* execute();
};

// used to create tasks that calculate 1/(N) elements per thread
class hyperthread_task : public tbb::task {
private:
	int hash; // in range [0, N-1] that decides which elements of the result matrix this task will compute
	const matrix& m1, m2;
	matrix& result;
public:
	std::list<hyperthread_task*> tasks;
	hyperthread_task(int, const matrix&, const matrix&, matrix&);
	tbb::task* execute();
};