#include <iostream>
#include <vector>
#include <set>
#include <queue>
#include <algorithm>
#include <numeric>

using namespace std;

set<int>::const_iterator FindNearestElement(const set<int>& numbers, int border) {
    auto low_bound = numbers.lower_bound(border);
    if (low_bound == begin(numbers))
        return low_bound;

    auto prev_low_bound = prev(low_bound);
    if (low_bound == end(numbers))
        return prev_low_bound;

    bool is_left = abs(*prev_low_bound - border) <= abs(*low_bound - border);
    return is_left ? prev_low_bound : low_bound;
}

#ifdef LOCAL_BUILD
int main() {
    set<int> numbers = {1, 4, 6};
    cout <<
            *FindNearestElement(numbers, 0) << " " <<
            *FindNearestElement(numbers, 3) << " " <<
            *FindNearestElement(numbers, 5) << " " <<
            *FindNearestElement(numbers, 6) << " " <<
            *FindNearestElement(numbers, 100) << endl;

    set<int> empty_set;
    cout << (FindNearestElement(empty_set, 8) == end(empty_set)) << endl;

    return 0;
}

#endif