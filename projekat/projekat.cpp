// projekat.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <fstream>
#include "matrix.h"
#include <tbb/tick_count.h>

int main(int argc, char* argv[])
{
    if (__argc < 3) {
        std::cout << "Expected 2 command line arguments." << std::endl;
        return -1;
    }
    std::ifstream input(argv[1]);
    if (!input) {
        std::cout << "File doesn't exist." << std::endl;
        return -1;
    }

    std::ofstream output(argv[2]);

    matrix  m1, m2;

    input >> m1 >> m2;
    input.close();

    if (!check_dimensions(m1, m2)) {
        std::cout << "Invalid Matrix Dimensions!" << std::endl;
        return -1;
    }

    matrix m3(m1.getRows(), m2.getCols());

    tbb::tick_count start_time;
    tbb::tick_count end_time;

    start_time = tbb::tick_count::now();
    serial_multiply(m1, m2, m3);
    end_time = tbb::tick_count::now();
    output << "Serial:" << std::endl << m3 << std::endl;

    std::cout << "Serial finished... Time took: " << (end_time - start_time).seconds() * 1000 << "ms" << std::endl;
    m3.reset();

    start_time = tbb::tick_count::now();
    parallel_multiply(m1, m2, m3);
    end_time = tbb::tick_count::now();
    output << "Parallel:" << std::endl << m3 << std::endl;

    std::cout << "Parallel finished... Time took: " << (end_time - start_time).seconds() * 1000 << "ms" << std::endl;
    m3.reset();

    start_time = tbb::tick_count::now();
    el_per_thread_multiply(m1, m2, m3);
    end_time = tbb::tick_count::now();
    output << "TBB task: one element per thread:" << std::endl << m3 << std::endl;

    std::cout << "TBB task: one element per thread finished... Time took: " << (end_time - start_time).seconds() * 1000 << "ms" << std::endl;
    m3.reset();

    start_time = tbb::tick_count::now();
    dimension_per_thread_multiply(m1, m2, m3);
    end_time = tbb::tick_count::now();
    output << "TBB task: dimension per thread:" << std::endl << m3 << std::endl;

    std::cout << "TBB task: dimension per thread finished... Time took: " << (end_time - start_time).seconds() * 1000 << "ms" << std::endl;
    m3.reset();

    start_time = tbb::tick_count::now();
    hyperthreading_multiply(m1, m2, m3);
    end_time = tbb::tick_count::now();
    output << "TBB task: hyperthreading:" << std::endl << m3 << std::endl;

    std::cout << "TBB task: hyperthreading finished... Time took: " << (end_time - start_time).seconds() * 1000 << "ms" << std::endl;
    m3.reset();

    output.close();


    return 0;
}