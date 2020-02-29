#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <set>


int main() {
//    std::ifstream in("tests/set_bus_stops_3_test_1.txt");
//    std::cin.rdbuf(in.rdbuf());

    std::map<std::set<std::string>, size_t> buses_to_stop;

    int n;
    std::cin >> n;
    for (int i=0; i < n; i++ ) {
        int stops_count;
        std::cin >> stops_count;
        std::set<std::string> stops;
        for (int i=0; i < stops_count; i++) {
            std::string stop;
            std::cin >> stop;

            stops.insert(stop);
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
