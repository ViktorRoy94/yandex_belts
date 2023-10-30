#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

template <typename It >
void PrintReversedRange (It range_begin , It range_end ) {
    auto it = range_end;
    while (it != range_begin) {
        it--;
        cout << *it << " ";
    }
}

void PrintVectorPart(const vector<int>& numbers) {
    auto it = find_if(begin(numbers), end(numbers), [](const int& num) { return num < 0; });
    if (it == end(numbers)) {
        PrintReversedRange(begin(numbers), end(numbers));
    } else {
        PrintReversedRange(begin(numbers), it);
    }

}

#ifdef LOCAL_BUILD
int main() {
  PrintVectorPart({6, 1, 8, -5, 4});
  cout << endl;
  PrintVectorPart({-6, 1, 8, -5, 4});  // ничего не выведется
  cout << endl;
  PrintVectorPart({6, 1, 8, 5, 4});
  cout << endl;
  return 0;
}
#endif