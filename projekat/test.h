#pragma once

#include <iostream>
#include "matrix.h"
#include <tbb/tick_count.h>

// Used to calculate average acceleration of 10 calculations and write the results to console
void test1(const matrix& m1, const matrix& m2, matrix& m3);