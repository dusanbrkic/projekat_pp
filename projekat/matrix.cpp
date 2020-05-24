#include "matrix.h"

matrix matrix::serial_multiply(const matrix& other) const
{
    matrix result(rows, other.cols);
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < other.cols; j++) {
            for (int k = 0; k < cols; k++) {
                result.elements.at(i).at(j) += elements.at(i).at(k) * other.elements.at(k).at(j);
            }
        }
    }
    return result;
}

matrix::matrix() : elements(), rows(0), cols(0) {};
matrix::matrix(const matrix& m) : elements(m.elements), rows(m.rows), cols(m.cols) {};
matrix::matrix(int _rows, int _cols) : rows(_rows), cols(_cols), elements(_rows, std::vector<double>(_cols)) {};

std::istream& operator>>(std::istream& input, matrix& m){
    std::string s;
    input >> m.rows;
    input >> m.cols;
    m.elements = std::vector<std::vector<double>>(m.rows, std::vector<double>(m.cols));
    for (int i = 0; i < m.rows; i++) {
        for (int j = 0; j < m.cols; j++) {
            input >> m.elements.at(i).at(j);
        }
    }
    return input;
}

std::ostream& operator<<(std::ostream& output, const matrix& m){
    output << '[';
    for (int i = 0; i < m.rows; i++) {
        for (int j = 0; j < m.cols; j++) {
            output << m.elements.at(i).at(j) << " ";
        }
        output << std::endl;
    }
    output << ']';
    return output;
}
