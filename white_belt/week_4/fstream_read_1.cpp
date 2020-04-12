#include <fstream>
#include <iostream>
#include <string>

int main() {
    std::string line;
    std::ifstream input("input.txt");

    if (input.is_open()) {
        while(std::getline(input, line)) {
            std::cout << line << std::endl;
        }
    }

    return 0;
}
