#include <iostream>
#include <set>
#include <string>

int main() {
    int n;
    std::cin >> n;

    std::set<std::string> unique_strings;
    for (int i=0; i < n; i++ ) {
        std::string str;
        std::cin >> str;

        unique_strings.insert(str);
    }
    std::cout << unique_strings.size();

    return 0;
}
