#include <iostream>
#include <fstream>
#include <map>
#include <string>
#include <vector>


int main() {
#ifdef DEBUG
    std::ifstream in("tests/map_bus_stops_2_test_1.txt");
    std::cin.rdbuf(in.rdbuf());
#endif

    std::map<std::vector<std::string>, size_t> buses_to_stop;

    int n;
    std::cin >> n;
    for (int i=0; i < n; i++ ) {
        int stops_count;
        std::cin >> stops_count;
        std::vector<std::string> stops(stops_count);
        for (std::string& stop : stops) {
            std::cin >> stop;
        }

        if (buses_to_stop.count(stops) > 0) {
            std::cout << "Already exists for " << buses_to_stop[stops] << std::endl;
        }
        else {
            size_t size = buses_to_stop.size() + 1;
            buses_to_stop[stops] = size;
            std::cout << "New bus " << size << std::endl;
        }
    }
    return 0;
}
