#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <tuple>

#ifdef DEBUG
enum class Lang {
    DE, FR, IT
};

struct Region {
    std::string std_name;
    std::string parent_std_name;
    std::map<Lang, std::string> names;
    int64_t population;
};
#endif

bool operator<(const Region& lhs, const Region& rhs) {
    auto left_side = std::tie(lhs.std_name, lhs.parent_std_name, lhs.names, lhs.population);
    auto right_side = std::tie(rhs.std_name, rhs.parent_std_name, rhs.names, rhs.population);
    return left_side < right_side;
}

int FindMaxRepetitionCount(const std::vector<Region>& regions) {
    int result = 0;
    std::map<Region, int> region_map;
    for (const auto& reg : regions) {
        result = std::max(result, ++region_map[reg]);
    }
    return result;
}

#ifdef DEBUG
int main() {
    std::cout << FindMaxRepetitionCount({
                {
                    "Moscow",
                    "Russia",
                    {{Lang::DE, "Moskau"}, {Lang::FR, "Moscou"}, {Lang::IT, "Mosca"}},
                    89
                }, {
                    "Russia",
                    "Eurasia",
                    {{Lang::DE, "Russland"}, {Lang::FR, "Russie"}, {Lang::IT, "Russia"}},
                    89
                }, {
                    "Moscow",
                    "Russia",
                    {{Lang::DE, "Moskau"}, {Lang::FR, "Moscou"}, {Lang::IT, "Mosca"}},
                    89
                }, {
                    "Moscow",
                    "Russia",
                    {{Lang::DE, "Moskau"}, {Lang::FR, "Moscou"}, {Lang::IT, "Mosca"}},
                    89
                }, {
                    "Russia",
                    "Eurasia",
                    {{Lang::DE, "Russland"}, {Lang::FR, "Russie"}, {Lang::IT, "Russia"}},
                    89
                },
            }) << std::endl;

    std::cout << FindMaxRepetitionCount({
                {
                    "Moscow",
                    "Russia",
                    {{Lang::DE, "Moskau"}, {Lang::FR, "Moscou"}, {Lang::IT, "Mosca"}},
                    89
                }, {
                    "Russia",
                    "Eurasia",
                    {{Lang::DE, "Russland"}, {Lang::FR, "Russie"}, {Lang::IT, "Russia"}},
                    89
                }, {
                    "Moscow",
                    "Russia",
                    {{Lang::DE, "Moskau"}, {Lang::FR, "Moscou deux"}, {Lang::IT, "Mosca"}},
                    89
                }, {
                    "Moscow",
                    "Toulouse",
                    {{Lang::DE, "Moskau"}, {Lang::FR, "Moscou"}, {Lang::IT, "Mosca"}},
                    89
                }, {
                    "Moscow",
                    "Russia",
                    {{Lang::DE, "Moskau"}, {Lang::FR, "Moscou"}, {Lang::IT, "Mosca"}},
                    31
                },
            }) << std::endl;

    return 0;
}
#endif
