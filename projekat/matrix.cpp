#include "matrix.h"
#define N 8

void serial_multiply(const matrix& first, const matrix& second, matrix& result) {
    for (int i = 0; i < first.rows; i++) {
        for (int j = 0; j < second.cols; j++) {
            for (int k = 0; k < first.cols; k++) {
                result.elements.at(i).at(j) += first.elements.at(i).at(k) * second.elements.at(k).at(j);
            }
        }
    }
}

void parallel_multiply(const matrix& first, const matrix& second, matrix& result){
    tbb::parallel_for(tbb::blocked_range2d<int>(0, first.rows, 0, second.cols), [&](const tbb::blocked_range2d<int>& br) {
        for (int i = br.rows().begin(), i_end = br.rows().end(); i < i_end; i++) {
            for (int j = br.cols().begin(), j_end = br.cols().end(); j < j_end; j++) {
                for (int k = 0; k < first.cols; k++) {
                    result.elements.at(i).at(j) += first.elements.at(i).at(k) * second.elements.at(k).at(j);
                }
            }
        }
    });
}

void el_per_thread_multiply(const matrix& first, const matrix& second, matrix& result){
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
}

void dimension_per_thread_multiply(const matrix& first, const matrix& second, matrix& result){
    int dimension;
    tbb::task_list tl;
    matrix_dimension_task* et = (matrix_dimension_task*) new (tbb::task::allocate_root()) tbb::empty_task;
    if (first.rows > second.cols) dimension = first.rows;
    else dimension = second.cols;
    et->set_ref_count(dimension + 1);
    for (int i = 0; i < dimension; i++) {
            matrix_dimension_task* m = new (tbb::task::allocate_root()) matrix_dimension_task(i, first, second, result);
            m->tasks.push_back(et);
            tl.push_back(*m);
    }
    
    et->spawn_and_wait_for_all(tl);
    et->execute();
    et->destroy(*et);
}

void hyperthreading_multiply(const matrix& first, const matrix& second, matrix& result){
    tbb::task_list tl;
    hyperthread_task* et = (hyperthread_task*) new (tbb::task::allocate_root()) tbb::empty_task;
    et->set_ref_count(N + 1);
    for (int i = 0; i < N; i++) {
        hyperthread_task* m = new (tbb::task::allocate_root()) hyperthread_task(i, first, second, result);
        m->tasks.push_back(et);
        tl.push_back(*m);
    }

    et->spawn_and_wait_for_all(tl);
    et->execute();
    et->destroy(*et);
}

matrix::matrix() : elements(), rows(0), cols(0) {};
matrix::matrix(const matrix& m) : elements(m.elements), rows(m.rows), cols(m.cols) {}
matrix::matrix(int _rows, int _cols) : rows(_rows), cols(_cols), elements(_rows, std::vector<int>(_cols)) {}

int matrix::getRows() { return rows; }
int matrix::getCols() { return cols; }

void matrix::reset(){
    for (int i = 0; i < rows; i++)
        for (int j = 0; j < cols; j++)
            elements.at(i).at(j) = 0;
}

std::istream& operator>>(std::istream& input, matrix& m){
    input >> m.rows;
    input >> m.cols;
    m.elements = std::vector<std::vector<int>>(m.rows, std::vector<int>(m.cols));
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

matrix_dimension_task::matrix_dimension_task(int d_, const matrix& m1_, const matrix& m2_, matrix& result_)
: d(d_), m1(m1_), m2(m2_), result(result_) {}

tbb::task* matrix_dimension_task::execute(){
    __TBB_ASSERT(ref_count() == 0, NULL);
    //One dimension is passed through the constructor (d), the row/column (d) represents is determined by the flag
    //flag = 1 -> this task fills the (d)-th row
    //flag = 2 -> this task fills the (d)-th column
    int flag = 1 ? 2 : m1.rows > m2.cols;
    if (flag == 1)
        for (int i = 0; i < m2.cols; i++)
            for (int j = 0; j < m1.cols; j++)
                result.elements.at(d).at(i) += m1.elements.at(d).at(j) * m2.elements.at(j).at(i);
    else
        for (int i = 0; i < m1.rows; i++)
            for (int j = 0; j < m1.cols; j++)
                result.elements.at(i).at(d) += m1.elements.at(i).at(j) * m2.elements.at(j).at(d);
    for (matrix_dimension_task* task : tasks)
        if (task->decrement_ref_count() == 0) spawn(*task);
    return NULL;
}

hyperthread_task::hyperthread_task(int hash_, const matrix& m1_, const matrix& m2_, matrix& result_) 
: hash(hash_), m1(m1_), m2(m2_), result(result_) {}

tbb::task* hyperthread_task::execute() {
    __TBB_ASSERT(ref_count() == 0, NULL);
    for (int i = hash; i < m1.rows * m2.cols; i += N) {
        for (int k = 0; k < m1.cols; k++) {
            // formula is row * no_of_columns + column --> (i)-th element in the result matrix (from right to left)
            // column --> (i) % no_of_columns | row --> (i) / no_of_columns
            result.elements.at(i/m2.cols).at(i%m2.cols) += m1.elements.at(i/m2.cols).at(k) * m2.elements.at(k).at(i%m2.cols);
        }
    }
    for (hyperthread_task* task : tasks)
        if (task->decrement_ref_count() == 0) spawn(*task);
    return NULL;
}
