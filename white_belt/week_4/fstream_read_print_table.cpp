#include <fstream>
#include <iostream>
#include <string>
#include <iomanip>

int main() {
    std::ifstream input("input.txt");

    if (input.is_open()) {
        int n, m, value;
        input >> n >> m;

        std::cout << std::fixed << std::setprecision(3) << std::right;
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < m; j++) {
                input >> value;
                input.ignore(1);
                std::cout << std::setw(10) << value;
                if (j != m-1)
                    std::cout << " ";
            }
            if (i != n-1)
                std::cout << std::endl;
        }
    }

    return 0;
}
