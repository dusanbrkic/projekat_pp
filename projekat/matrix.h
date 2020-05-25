#pragma once

#include <iostream>
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
	std::vector<std::vector<double>> elements;
public:
	matrix();
	matrix(int, int);
	matrix(const matrix& m);

	friend matrix serial_multiply(const matrix&, const matrix&);
	friend matrix parallel_multiply(const matrix&, const matrix&);
	friend matrix el_per_thread_multiply(const matrix&, const matrix&);
	friend matrix dimension_per_thread_multiply(const matrix&, const matrix&);
	friend matrix hyperthreading_multiply(const matrix&, const matrix&);

	friend std::istream& operator>>(std::istream& input, matrix& m);
	friend std::ostream& operator<<(std::ostream& output, const matrix& m);
	friend bool check_dimensions(const matrix&, const matrix&);

	friend class matrix_element_task;
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