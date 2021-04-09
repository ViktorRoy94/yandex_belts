#include "test_runner.h"
#include <vector>
#include <future>
#include <numeric>
using namespace std;

template <typename Iterator>
class Page {
public:
    Page(Iterator begin, Iterator end)
        : begin_(begin)
        , end_(end)
        , size_(distance(end, begin))
    {
    }

    size_t size() const
    {
        return size_;
    }

    Iterator begin() const
    {
        return begin_;
    }

    Iterator end() const
    {
        return end_;
    }

private:
    Iterator begin_;
    Iterator end_;
    size_t size_;
};

template <typename Iterator>
class Paginator {
public:
    Paginator(Iterator begin, Iterator end, size_t page_size)
    {
        for (Iterator it = begin; it < end; it += page_size) {
            if (it + page_size < end) {
                pages_.push_back(Page(it, it + page_size));
            } else {
                pages_.push_back(Page(it, end));
            }
        }
    }

    size_t size() const
    {
        return pages_.size();
    }

    auto begin() const
    {
        return pages_.begin();
    }

    auto end() const
    {
        return pages_.end();
    }

private:
    vector<Page<Iterator>> pages_;
};



template <typename C>
auto Paginate(C& c, size_t page_size) {
    return Paginator(begin(c), end(c), page_size);
}

template <typename ContainerOfVectors>
int64_t ComputeSumOfMatrixPart(const ContainerOfVectors& matrix) {
    int64_t sum = 0;
    for (const auto& row : matrix) {
        sum += accumulate(row.begin(), row.end(), 0);
    }
    return sum;
}

int64_t CalculateMatrixSum(const vector<vector<int>>& matrix) {
    vector<future<int64_t>> futures;
    for (auto page : Paginate(matrix, 2000)) {
        futures.push_back(async([=]{
            return ComputeSumOfMatrixPart(page);
        }));
    }

    int64_t result = 0;
    for (auto& f : futures) {
        result += f.get();
    }
    return result;

//    return accumulate(futures.begin(), futures.end(), 0,
//                      [](const future<int64_t>& f1, const future<int64_t>& f2){
//                        return f1.get() + f2.get();
//                      }
//    );
}

void TestCalculateMatrixSum() {
    {
        const vector<vector<int>> matrix = {
            {1, 2, 3, 4},
            {5, 6, 7, 8},
            {9, 10, 11, 12},
            {13, 14, 15, 16}
        };
        ASSERT_EQUAL(CalculateMatrixSum(matrix), 136);
    }
    {
        const vector<vector<int>> matrix = {
            {1, 2, 3, 4, 1, 1},
            {5, 6, 7, 8, 1, 1},
            {9, 10, 11, 12, 1, 1},
            {13, 14, 15, 16, 1, 1},
            {1, 1, 1, 1, 1, 1},
            {1, 1, 1, 1, 1, 1}
        };
        ASSERT_EQUAL(CalculateMatrixSum(matrix), 156);
    }
    {
        const vector<vector<int>> matrix = {
            {1, 1, 1},
            {1, 1, 1},
            {1, 1, 1}
        };
        ASSERT_EQUAL(CalculateMatrixSum(matrix), 9);
    }
    {
        const vector<vector<int>> matrix = {
            {1, 1},
            {1, 1}
        };
        ASSERT_EQUAL(CalculateMatrixSum(matrix), 4);
    }
//    {
//        const vector<vector<int>> matrix = {
//            {1}
//        };
//        ASSERT_EQUAL(CalculateMatrixSum(matrix), 1);
//    }
}

int main() {
    TestRunner tr;
    RUN_TEST(tr, TestCalculateMatrixSum);
}
