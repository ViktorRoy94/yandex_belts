#include <iostream>
#include <set>
#include <map>
#include <string>


std::set<std::string> BuildMapValuesSet(const std::map<int, std::string>& m) {
    std::set<std::string> map_value_set;
    for (const auto& item : m) {
        map_value_set.insert(item.second);
    }
    return map_value_set;
}


int main() {
    std::set<std::string> values = BuildMapValuesSet({
        {1, "odd"},
        {2, "even"},
        {3, "odd"},
        {4, "even"},
        {5, "odd"}
    });

    for (const std::string& value : values) {
      std::cout << value << std::endl;
    }

    return 0;
}
