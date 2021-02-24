#include <iostream>
#include <vector>
#include <set>
#include <algorithm>
#include <numeric>

using namespace std;

#ifdef LOCAL_BUILD
template <class T>
ostream& operator << (ostream& os, const vector<T>& s) {
    bool first = true;
    for (const auto& x : s) {
        if (!first) {
            os << " ";
        }
        first = false;
        os << x;
    }
    return os;
}
#endif

template <typename RandomIt>
void MergeSort(RandomIt range_begin, RandomIt range_end) {
    if (range_end - range_begin < 2)
        return;

    vector<typename RandomIt::value_type> elements(range_begin, range_end);
    auto end_first_part = begin(elements) + elements.size() / 3;
    auto end_second_part = begin(elements) + 2 * elements.size() / 3;
    MergeSort(begin(elements), end_first_part);
    MergeSort(end_first_part, end_second_part);
    MergeSort(end_second_part, end(elements));

    vector<typename RandomIt::value_type> temp;
    std::merge(begin(elements), end_first_part, end_first_part, end_second_part, back_inserter(temp));
    std::merge(begin(temp), end(temp), end_second_part, end(elements), range_begin);
}

#ifdef LOCAL_BUILD
int main() {
    vector<int> v = {6, 4, 7, 6, 4, 4, 0, 1, 5};
    MergeSort(begin(v), end(v));
    for (int x : v) {
        cout << x << " ";
    }
    cout << endl;
    return 0;
}

#endif