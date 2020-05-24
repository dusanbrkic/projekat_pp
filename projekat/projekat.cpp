// projekat.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <fstream>
#include "matrix.h"

bool check_dimensions(const matrix& a, const matrix& b) {
    if (a.cols != b.rows) return false;
    return true;
}

int main()
{
    if (__argc < 3) {
        std::cout << "Expected 2 command line arguments." << std::endl;
        return -1;
    }
    std::ifstream input(__argv[1]);
    if (!input) {
        std::cout << "File doesn't exist." << std::endl;
        return -1;
    }

    std::ofstream output(__argv[2]);

    matrix  m1, m2, m3;

    input >> m1 >> m2;
    input.close();

    if (!check_dimensions(m1, m2)) {
        std::cout << "Invalid Matrix Dimensions!" << std::endl;
        return -1;
    }

    output << "Serial:" << std::endl << m1.serial_multiply(m2);

    std::cout << "Serial finished..." << std::endl;

    output << "Parallel:" << std::endl << m1.parallel_multiply(m2) << std::endl;

    std::cout << "Parallel finished..." << std::endl;

    output << "TBB task:" << std::endl << m1.tbb_task_multiply(m2);

    std::cout << "TBB task finished..." << std::endl;

    output.close();


    return 0;
}