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
	void reset(); //sets all elements to 0

	friend void serial_multiply(const matrix&, const matrix&, matrix&);
	friend void parallel_multiply(const matrix&, const matrix&, matrix&);
	friend void el_per_thread_multiply(const matrix&, const matrix&, matrix&);
	friend void dimension_per_thread_multiply(const matrix&, const matrix&, matrix&);
	friend void hyperthreading_multiply(const matrix&, const matrix&, matrix&);

	friend std::istream& operator>>(std::istream& input, matrix& m);
	friend std::ostream& operator<<(std::ostream& output, const matrix& m);
	friend bool check_dimensions(const matrix&, const matrix&);

	friend class matrix_element_task;
	friend class matrix_dimension_task;
	friend class hyperthread_task;
};

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

class hyperthread_task : public tbb::task {
private:
	int hash; // An integer in range [0, N-1] that decides which elements of the result matrix this task will compute
	const matrix& m1, m2;
	matrix& result;
public:
	std::list<hyperthread_task*> tasks;
	hyperthread_task(int, const matrix&, const matrix&, matrix&);
	tbb::task* execute();
};