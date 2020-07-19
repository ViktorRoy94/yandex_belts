#include <iostream>
#include <fstream>
#include <vector>


int main() {
//#ifdef DEBUG
//    std::ifstream in("tests/average_temperature.txt");
//    std::cin.rdbuf(in.rdbuf());
//#endif

    int n, r;
    std::cin >> n >> r;

    uint64_t sum = 0;
    for (int i = 0; i < n; i++) {
        int16_t w, h, d;
        std::cin >> w >> h >> d;
        sum += static_cast<int64_t>(w) * h * d * r;
    }

    std::cout << sum << std::endl;
    return 0;
}
