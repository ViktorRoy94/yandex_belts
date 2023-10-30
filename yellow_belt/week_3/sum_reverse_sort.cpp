#include "sum_reverse_sort.h"
#include <algorithm>
#include <iostream>

int Sum(int x, int y) {
    return x + y;
}
string Reverse(string s) {
    return string(s.rbegin(), s.rend());
}
void Sort(vector<int>& nums) {
    sort(nums.begin(), nums.end());
}

#ifdef LOCAL_BUILD
template <class T>
ostream& operator << (ostream& os, const vector<T>& s) {
    os << "{";
    bool first = true;
    for (const auto& x : s) {
        if (!first) {
            os << ", ";
        }
        first = false;
        os << x;
    }
    return os << "}";
}

int main() {
    std::vector<int> vec = {5, 4, 3, 2, 1};
    std::cout << Sum(3, 5) << std::endl;
    std::cout << Reverse("reversed_string") << std::endl;
    Sort(vec);
    std::cout << vec << std::endl;
}
#endif
