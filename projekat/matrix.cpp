#include "matrix.h"

matrix serial_multiply(const matrix& first, const matrix& second) {
    matrix result(first.rows, second.cols);
    for (int i = 0; i < first.rows; i++) {
        for (int j = 0; j < second.cols; j++) {
            for (int k = 0; k < first.cols; k++) {
                result.elements.at(i).at(j) += first.elements.at(i).at(k) * second.elements.at(k).at(j);
            }
        }
    }
    return result;
}

matrix parallel_multiply(const matrix& first, const matrix& second){
    matrix result(first.rows, second.cols);
    tbb::parallel_for(tbb::blocked_range2d<int>(0, first.rows, 0, second.cols), [&](const tbb::blocked_range2d<int>& br) {
        for (int i = br.rows().begin(), i_end = br.rows().end(); i < i_end; i++) {
            for (int j = br.cols().begin(), j_end = br.cols().end(); j < j_end; j++) {
                for (int k = 0; k < first.cols; k++) {
                    result.elements.at(i).at(j) += first.elements.at(i).at(k) * second.elements.at(k).at(j);
                }
            }
        }
    });

    return result;
}

matrix el_per_thread_multiply(const matrix& first, const matrix& second){
    matrix result(first.rows, second.cols);
    tbb::task_list tl;
    matrix_element_task* et = (matrix_element_task*) new (tbb::task::allocate_root()) tbb::empty_task;
    et->set_ref_count(first.rows * second.cols + 1);
    for (int i = 0; i < first.rows; i++) {
        for (int j = 0; j < second.cols; j++) {
            matrix_element_task* m = new (tbb::task::allocate_root()) matrix_element_task(i, j, first, second, result);
            m->tasks.push_back(et);
            tl.push_back(*m);
        }
    }
    et->spawn_and_wait_for_all(tl);
    et->execute();
    et->destroy(*et);

    return result;
}

matrix dimension_per_thread_multiply(const matrix& first, const matrix& second){
    return matrix();
}

matrix hyperthreading_multiply(const matrix& first, const matrix& second){
    return matrix();
}

matrix::matrix() : elements(), rows(0), cols(0) {};
matrix::matrix(const matrix& m) : elements(m.elements), rows(m.rows), cols(m.cols) {};
matrix::matrix(int _rows, int _cols) : rows(_rows), cols(_cols), elements(_rows, std::vector<double>(_cols)) {};

std::istream& operator>>(std::istream& input, matrix& m){
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

bool check_dimensions(const matrix& a, const matrix& b) {
    if (a.cols != b.rows) return false;
    return true;
}

matrix_element_task::matrix_element_task(int row_, int col_, const matrix& m1_, const matrix& m2_, matrix& result_) 
    : row(row_), col(col_), m1(m1_), m2(m2_), result(result_) {}

tbb::task* matrix_element_task::execute(){
    __TBB_ASSERT(ref_count() == 0, NULL);
    for (int i = 0; i < m1.cols; i++)
        result.elements.at(row).at(col) += m1.elements.at(row).at(i) * m2.elements.at(i).at(col);
    for (matrix_element_task* task : tasks)
        if (task->decrement_ref_count() == 0) spawn(*task);
    return NULL;
}
