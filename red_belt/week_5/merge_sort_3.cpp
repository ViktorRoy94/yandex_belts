#include "test_runner.h"
#include <algorithm>
#include <memory>
#include <vector>

using namespace std;

template <typename RandomIt>
void MergeSort(RandomIt range_begin, RandomIt range_end) {
    size_t size = distance(range_begin, range_end);
    if (size < 2) {
        return;
    }

    vector<typename RandomIt::value_type> elements(make_move_iterator(range_begin),
                                                   make_move_iterator(range_end));
    auto end_first_part = begin(elements) + size / 3;
    auto end_second_part = begin(elements) + 2 * size / 3;
    MergeSort(begin(elements), end_first_part);
    MergeSort(end_first_part, end_second_part);
    MergeSort(end_second_part, end(elements));

    vector<typename RandomIt::value_type> temp;
    std::merge(make_move_iterator(begin(elements)),
               make_move_iterator(end_first_part),
               make_move_iterator(end_first_part),
               make_move_iterator(end_second_part),
               back_inserter(temp));
    std::merge(make_move_iterator(begin(temp)),
               make_move_iterator(end(temp)),
               make_move_iterator(end_second_part),
               make_move_iterator(end(elements)),
               range_begin);

}

void TestIntVector() {
    vector<int> numbers = {6, 1, 3, 9, 1, 9, 8, 12, 1};
    MergeSort(begin(numbers), end(numbers));
    ASSERT(is_sorted(begin(numbers), end(numbers)));
}

int main() {
    TestRunner tr;
    RUN_TEST(tr, TestIntVector);
    return 0;
}
