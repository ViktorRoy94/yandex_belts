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
    auto middle_it = begin(elements) + elements.size() / 2;
    MergeSort(begin(elements), middle_it);
    MergeSort(middle_it, end(elements));

    std::merge(begin(elements), middle_it, middle_it, end(elements), range_begin);
}

#ifdef LOCAL_BUILD
int main() {
    vector<int> v = {6, 4, 7, 6, 4, 4, 0, 1};
//    vector<int> v = {3, 2, 1, 0};
    MergeSort(begin(v), end(v));
    for (int x : v) {
        cout << x << " ";
    }
    cout << endl;
    return 0;
}
#endif