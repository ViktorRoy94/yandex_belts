#include <iostream>
#include <map>
#include <string>

using namespace std;

map<char, int> BuildCharCounters(string word) {
    map<char, int> result;
    for (char c : word) {
        result[c]++;
    }
    return result;
}

int main() {
    int n;
    cin >> n;
    for (int i=0; i < n; i++ ) {
        string word_1, word_2;
        cin >> word_1 >> word_2;

        if (BuildCharCounters(word_1) == BuildCharCounters(word_2))
            cout << "YES" << endl;
        else
            cout << "NO" << endl;
    }
    return 0;
}
