#include <iostream>
#include <vector>
#include <set>
#include <algorithm>
#include <numeric>

using namespace std;

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

int main() {
    int n;

#ifdef LOCAL_BUILD
    n = 3;
#else
    cin >> n;
#endif
    std::vector<int> vec(n);
    iota(rbegin(vec), rend(vec), 1);
    do {
        cout << vec << endl;
    } while (prev_permutation(begin(vec), end(vec)));
}