#include <iostream>
#include <algorithm>
#include <vector>
#include <string>

bool str_comp (std::string i, std::string j) {
    std::transform(i.begin(), i.end(), i.begin(),
                   [](char c){ return tolower(c); });
    std::transform(j.begin(), j.end(), j.begin(),
                   [](char c){ return tolower(c); });
    return i < j;
}

int main() {
    int n = 0;
    std::cin >> n;

    std::vector<std::string> words(n);
    for (auto& i: words) {
        std::cin >> i;
    }

    sort(words.begin(), words.end(), str_comp);

    for (const auto& i: words) {
        std::cout << i << " ";
    }

    return 0;
}
