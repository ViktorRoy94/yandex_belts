#include <iostream>
#include <fstream>
#include <map>
#include <string>

void PrintMap(const std::map<std::string, std::string>& m) {
    for (const auto& item: m) {
        std::cout << item.first << "/" << item.second << " ";
    }
    std::cout << std::endl;
}

int main() {
#ifdef LOCAL_BUILD
    std::ifstream in("tests/map_capitals_test_2.txt");
    std::cin.rdbuf(in.rdbuf());
#endif

    std::map<std::string, std::string> capital_map;

    int n;
    std::cin >> n;
    for (int i=0; i < n; i++ ) {
        std::string op;
        std::cin >> op;
        if (op == "CHANGE_CAPITAL") {
            std::string country, new_capital;
            std::cin >> country >> new_capital;
            if (capital_map.count(country) == 0) {
                capital_map[country] = new_capital;
                std::cout << "Introduce new country " << country
                          << " with capital " << new_capital << std::endl;
            } else {
                std::string old_capital = capital_map[country];
                if (new_capital == old_capital) {
                    std::cout << "Country " << country << " hasn't changed its capital" << std::endl;
                }
                else {
                    capital_map[country] = new_capital;
                    std::cout << "Country " << country << " has changed its capital from "
                              << old_capital << " to " << new_capital << std::endl;
                }
            }
        }
        if (op == "RENAME") {
            std::string old_name, new_name;
            std::cin >> old_name >> new_name;
            if (capital_map.count(old_name) == 0 ||
                capital_map.count(new_name) == 1 ||
                    old_name == new_name) {
                std::cout << "Incorrect rename, skip" << std::endl;
                continue;
            }
            capital_map[new_name] = capital_map[old_name];
            capital_map.erase(old_name);
            std::cout << "Country " << old_name << " with capital " << capital_map[new_name]
                         << " has been renamed to " << new_name << std::endl;
        }
        if (op == "ABOUT") {
            std::string country;
            std::cin >> country;
            if (capital_map.count(country) == 0) {
                std::cout << "Country " << country << " doesn't exist" << std::endl;
            } else {
                std::cout << "Country " << country << " has capital " << capital_map[country] << std::endl;
            }
        }
        if (op == "DUMP") {
            if (capital_map.size() == 0) {
                std::cout << "There are no countries in the world" << std::endl;
            }
            else {
                PrintMap(capital_map);
            }
        }

    }
    return 0;
}
