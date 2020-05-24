#pragma once

#include <iostream>
#include <vector>
#include <fstream>
#include <string>

class matrix {
private:
	int rows, cols;
	std::vector<std::vector<double>> elements;
public:

	struct InvalidInputFileFormat : std::runtime_error {
		InvalidInputFileFormat() : runtime_error("Invalid Input File Format") {}
	};

	struct InvalidMatrixDimensions : std::runtime_error {
		InvalidMatrixDimensions() : runtime_error("Invalid Matrix Dimensions") {}
	};

	matrix();
	matrix(int, int);
	matrix(const matrix& m);

	matrix serial_multiply(const matrix& other) const;

	friend std::istream& operator>>(std::istream& input, matrix& m);
	friend std::ostream& operator<<(std::ostream& output, const matrix& m);
	friend bool check_dimensions(const matrix&, const matrix&);

};