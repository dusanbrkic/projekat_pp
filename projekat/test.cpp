#include "test.h"
#define T 10 // Number of test cases

void test1(const matrix& m1, const matrix& m2, matrix& m3) {
	tbb::tick_count start_time;
	tbb::tick_count end_time;
	double serial_time, parallel_time, acc, avg_acc1(0), avg_acc2(0), avg_acc3(0), avg_acc4(0);

	for (int i = 0; i < T; i++) {

		start_time = tbb::tick_count::now();
		serial_multiply(m1, m2, m3);
		end_time = tbb::tick_count::now();

		serial_time = (end_time - start_time).seconds() * 1000;
		std::cout << "Serial finished... Time took: " << serial_time << "ms" << std::endl;
		m3.reset();

		start_time = tbb::tick_count::now();
		parallel_multiply(m1, m2, m3);
		end_time = tbb::tick_count::now();

		parallel_time = (end_time - start_time).seconds() * 1000;
		std::cout << "Parallel finished... Time took: " << parallel_time << "ms" << std::endl;
		acc = serial_time / parallel_time;
		std::cout << "Acceleration: " << acc << std::endl;
		avg_acc1 += acc;
		m3.reset();

		start_time = tbb::tick_count::now();
		el_per_thread_multiply(m1, m2, m3);
		end_time = tbb::tick_count::now();

		parallel_time = (end_time - start_time).seconds() * 1000;
		std::cout << "TBB task: one element per thread finished... Time took: " << parallel_time << "ms" << std::endl;
		acc = serial_time / parallel_time;
		std::cout << "Acceleration: " << acc << std::endl;
		avg_acc2 += acc;
		m3.reset();

		start_time = tbb::tick_count::now();
		dimension_per_thread_multiply(m1, m2, m3);
		end_time = tbb::tick_count::now();

		parallel_time = (end_time - start_time).seconds() * 1000;
		std::cout << "TBB task: dimension per thread finished... Time took: " << parallel_time << "ms" << std::endl;
		acc = serial_time / parallel_time;
		std::cout << "Acceleration: " << acc << std::endl;
		avg_acc3 += acc;
		m3.reset();

		start_time = tbb::tick_count::now();
		hyperthreading_multiply(m1, m2, m3);
		end_time = tbb::tick_count::now();

		parallel_time = (end_time - start_time).seconds() * 1000;
		std::cout << "TBB task: hyperthreading finished... Time took: " << parallel_time << "ms" << std::endl;
		acc = serial_time / parallel_time;
		std::cout << "Acceleration: " << acc << std::endl;
		avg_acc4 += acc;
		m3.reset();
	}
	std::cout << "Average Acceleration par_for: " << avg_acc1 / T << std::endl;
	std::cout << "Average Acceleration el_per_thread: " << avg_acc2 / T << std::endl;
	std::cout << "Average Acceleration dim_per_thread: " << avg_acc3 / T << std::endl;
	std::cout << "Average Acceleration hyperthreading: " << avg_acc4 / T << std::endl;
	std::cout << std::endl;
	std::cout << "Testing finished." << std::endl;
	std::cout << std::endl;

}
