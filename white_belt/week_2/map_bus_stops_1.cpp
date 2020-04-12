#include <iostream>
#include <fstream>
#include <map>
#include <string>
#include <vector>


int main() {
#ifdef DEBUG
    std::ifstream in("tests/map_bus_stops_1_test_1.txt");
    std::cin.rdbuf(in.rdbuf());
#endif

    std::map<std::string, std::vector<std::string>> buses_to_stop;
    std::map<std::string, std::vector<std::string>> stops_to_buses;

    int n;
    std::cin >> n;
    for (int i=0; i < n; i++ ) {
        std::string op;
        std::cin >> op;
        if (op == "NEW_BUS") {
            std::string bus_name;
            std::cin >> bus_name;

            int stop_count;
            std::cin >> stop_count;
            std::vector<std::string> stops(stop_count);
            for (std::string& stop: stops) {
                std::cin >> stop;
                stops_to_buses[stop].push_back(bus_name);
            }
            buses_to_stop[bus_name] = stops;
        }
        if (op == "BUSES_FOR_STOP") {
            std::string stop;
            std::cin >> stop;
            if (stops_to_buses.count(stop) == 0) {
                std::cout << "No stop" << std::endl;
                continue;
            }

            for (const std::string& bus : stops_to_buses[stop]) {
                std::cout << bus << " ";
            }
            std::cout << std::endl;
        }
        if (op == "STOPS_FOR_BUS") {
            std::string bus_name;
            std::cin >> bus_name;
            if (buses_to_stop.count(bus_name) == 0) {
                std::cout << "No bus" << std::endl;
                continue;
            }

            for (const std::string& bus_stop : buses_to_stop[bus_name]) {
                std::cout << "Stop " << bus_stop << ": ";
                if (stops_to_buses[bus_stop].size() == 1) {
                    std::cout << "no interchange";
                }
                else {
                    for (const std::string& bus : stops_to_buses[bus_stop]) {
                        if (bus != bus_name)
                            std::cout << bus << " ";
                    }
                }
                std::cout << std::endl;
            }
        }
        if (op == "ALL_BUSES") {
            if (buses_to_stop.size() == 0) {
                std::cout << "No buses" << std::endl;
                continue;
            }
            for (const auto& item : buses_to_stop) {
                std::cout << "Bus " << item.first << ": ";
                for (const std::string& stop_name : item.second) {
                    std::cout << stop_name << " ";
                }
                std::cout << std::endl;
            }
        }
    }
    return 0;
}
