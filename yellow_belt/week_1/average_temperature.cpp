#include <iostream>
#include <fstream>
#include <vector>


int main() {
#ifdef DEBUG
    std::ifstream in("tests/average_temperature.txt");
    std::cin.rdbuf(in.rdbuf());
#endif

    int n;
    std::cin >> n;

    int64_t sum = 0;
    std::vector<int> temperatures(n);
    for (auto& elem : temperatures) {
        std::cin >> elem;
        sum += elem;
    }

    int average_temp = sum / n;
    std::vector<int> greater_than_average_idx;
    for (int i = 0; i < n; i++) {
        if (temperatures[i] > average_temp) {
            greater_than_average_idx.push_back(i);
        }
    }

    std::cout << greater_than_average_idx.size() << std::endl;
    for (const auto& elem : greater_than_average_idx) {
        std::cout << elem << " ";
    }
    std::cout << std::endl;

    return 0;
}
