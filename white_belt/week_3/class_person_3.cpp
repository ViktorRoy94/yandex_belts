#include <iostream>
#include <string>
#include <map>
#include <vector>

class Person {
public:
    Person(const std::string& first_name, const std::string& last_name, int year) {
        birth_year = year;
        first_names[year] = first_name;
        last_names[year] = last_name;
    }
    void ChangeFirstName(int year, const std::string& first_name) {
        if (year >= birth_year) {
            first_names[year] = first_name;
        }
    }
    void ChangeLastName(int year, const std::string& last_name) {
        if (year >= birth_year) {
            last_names[year] = last_name;
        }
    }
    std::string GetFullName(int year) const{
        if (year < birth_year) {
            return "No person";
        }

        std::vector<std::string> first_names_hist = FindHistoryNameByYear(year, first_names);
        std::vector<std::string> last_names_hist = FindHistoryNameByYear(year, last_names);

        std::string first_name;
        std::string last_name;

        if (!first_names_hist.empty())
            first_name = first_names_hist.back();
        if (!last_names_hist.empty())
            last_name = last_names_hist.back();

        return BuildFullName(first_name, last_name);
    }
    std::string GetFullNameWithHistory(int year) const{
        if (year < birth_year) {
            return "No person";
        }

        std::vector<std::string> first_names_hist = FindHistoryNameByYear(year, first_names);
        std::vector<std::string> last_names_hist = FindHistoryNameByYear(year, last_names);

        std::string first_name_result = BuildJoinedString(first_names_hist);
        std::string last_name_result = BuildJoinedString(last_names_hist);

        return BuildFullName(first_name_result, last_name_result);
    }

private:
    int birth_year;
    std::map<int, std::string> first_names;
    std::map<int, std::string> last_names;

    std::vector<std::string> FindHistoryNameByYear(int year, std::map<int, std::string> names) const {
        std::vector<std::string> name_history;
        auto it = names.begin();
        while (it != names.end() && it->first <= year) {
            if (it->second != "" && (name_history.empty() || name_history.back() != it->second)) {
                name_history.push_back(it->second);
            }
            it++;
        }
        return name_history;
    }
    std::string BuildFullName(const std::string& first_name, const std::string& last_name) const{
        if (first_name == "" && last_name == "")
            return "Incognito";
        else if (first_name.empty())
            return last_name + " with unknown first name";
        else if (last_name.empty())
            return first_name + " with unknown last name";
        else
            return first_name + " " + last_name;
    }
    std::string BuildJoinedString(std::vector<std::string> name_history) const {
        std::string result;

        if (name_history.size() > 1) {
            result += name_history.back() + " (";
            for (int i = name_history.size()-2; i >= 0; i--) {
                result += name_history[i];
                if (i > 0)
                    result += ", ";
            }
            result += ")";
        }
        else {
            if (name_history.size() == 1)
                result = name_history.front();
        }
        return result;
    }
};

int main() {
    Person person("Polina", "Sergeeva", 1960);
    for (int year : {1959, 1960}) {
        std::cout << person.GetFullNameWithHistory(year) << std::endl;
    }

    person.ChangeFirstName(1965, "Appolinaria");
    person.ChangeLastName(1967, "Ivanova");
    for (int year : {1965, 1967}) {
        std::cout << person.GetFullNameWithHistory(year) << std::endl;
    }

    return 0;
}

