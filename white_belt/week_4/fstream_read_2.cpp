#include <fstream>
#include <iostream>
#include <string>

int main() {
    std::string line;
    std::ifstream input("input.txt");
    std::ofstream output("output.txt");

    if (input.is_open() && output.is_open()) {
        while(std::getline(input, line)) {
            output << line << std::endl;
        }
    }

    return 0;
}
