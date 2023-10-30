#include <iostream>
#include <fstream>
#include <stdexcept>
#include <vector>

class Matrix
{
public:
    Matrix() : num_rows(0), num_cols(0) {}
    Matrix(int num_rows_, int num_cols_) {
        Reset(num_rows_, num_cols_);
    }

    void Reset(int num_rows_, int num_cols_) {
        if (num_rows_ < 0 || num_cols_ < 0) {
            throw std::out_of_range(" ");
        }

        if (num_rows_ == 0 || num_cols_ == 0) {
            num_rows_ = 0;
            num_cols_ = 0;
        }

        data.clear();
        num_rows = num_rows_;
        num_cols = num_cols_;
        data.assign(num_rows_, std::vector<int>(num_cols_));
    }

    int At(int i, int j) const {
        if (i < 0 || i >= num_rows || j < 0 || j >= num_cols) {
            throw std::out_of_range(" ");
        }
        return data[i][j];
    }

    int& At(int i, int j){
        if (i < 0 || i >= num_rows || j < 0 || j >= num_cols) {
            throw std::out_of_range(" ");
        }
        return data[i][j];
    }

    int GetNumRows() const {
        return num_rows;
    }

    int GetNumColumns() const {
        return num_cols;
    }

private:
    int num_rows;
    int num_cols;
    std::vector<std::vector<int>> data;
};

std::istream& operator>>(std::istream& stream, Matrix& mat) {
    int n = 0;
    int m = 0;
    stream >> n >> m;
    mat.Reset(n, m);
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            stream >> mat.At(i, j);
        }
    }
    return stream;
}

std::ostream& operator<<(std::ostream& stream, const Matrix& mat) {
    stream << mat.GetNumRows() << " " << mat.GetNumColumns() << std::endl;
    for (int i = 0; i < mat.GetNumRows(); i++) {
        for (int j = 0; j < mat.GetNumColumns(); j++) {
            stream << mat.At(i, j) << " ";
        }
        stream << std::endl;
    }
    return stream;
}

bool operator==(const Matrix& left, const Matrix& right) {
    if (left.GetNumRows()    != right.GetNumRows() ||
        left.GetNumColumns() != right.GetNumColumns()) {
        return false;
    }

    for (int i = 0; i < left.GetNumRows(); i++) {
        for (int j = 0; j < left.GetNumColumns(); j++) {
            if (left.At(i, j) != right.At(i, j)) {
                return false;
            }
        }
    }

    return true;
}

Matrix operator+(const Matrix& left, const Matrix& right) {
    if (left.GetNumRows()    != right.GetNumRows() ||
        left.GetNumColumns() != right.GetNumColumns()) {
        throw std::invalid_argument(" ");
    }
    Matrix sum(left.GetNumRows(), left.GetNumColumns());
    for (int i = 0; i < left.GetNumRows(); i++) {
        for (int j = 0; j < left.GetNumColumns(); j++) {
            sum.At(i, j) = left.At(i, j) + right.At(i, j);
        }
    }
    return sum;
}


int main() {
#ifdef LOCAL_BUILD
    std::ifstream in("tests/matrix_sum.txt");
    std::cin.rdbuf(in.rdbuf());
#endif
    {
        Matrix one;
        Matrix two;

        std::cin >> one >> two;
        std::cout << one + two << std::endl;
    }

    {
        Matrix one;
        Matrix two(0, 0);

        std::cout << (one == two) << std::endl;
        std::cout << (one + two) << std::endl;
    }
    return 0;
}
