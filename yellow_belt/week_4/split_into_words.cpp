#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

vector<string> SplitIntoWords(const string& s) {
    vector<string> result;
    auto curr_it = begin(s);
    auto prev_it = begin(s);
    while (curr_it != end(s)) {
        curr_it = find(prev_it, end(s), ' ');
        result.push_back(string(prev_it, curr_it));
        prev_it = curr_it + 1;
    }
    return result;
}

#ifdef LOCAL_BUILD
int main() {
    string s = "C Cpp Java Python";
    vector<string> words = SplitIntoWords(s);
    cout << words.size() << " ";
    for (auto it = begin(words); it != end(words); ++it) {
        if (it != begin(words)) {
            cout << "/";
        }
        cout << *it;
    }
    cout << endl;

    return 0;
}
#endif
