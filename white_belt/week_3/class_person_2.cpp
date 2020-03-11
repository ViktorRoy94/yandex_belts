#include <iostream>
#include <string>
#include <map>
#include <vector>

class Person {
public:
    void ChangeFirstName(int year, const std::string& first_name) {
        first_names[year] = first_name;
    }
    void ChangeLastName(int year, const std::string& last_name) {
        last_names[year] = last_name;
    }
    std::string GetFullName(int year) {
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

    std::string GetFullNameWithHistory(int year) {
        std::vector<std::string> first_names_hist = FindHistoryNameByYear(year, first_names);
        std::vector<std::string> last_names_hist = FindHistoryNameByYear(year, last_names);

        std::string first_name_result = BuildJoinedString(first_names_hist);
        std::string last_name_result = BuildJoinedString(last_names_hist);

        return BuildFullName(first_name_result, last_name_result);
    }

private:
    std::map<int, std::string> first_names;
    std::map<int, std::string> last_names;

    std::vector<std::string> FindHistoryNameByYear(int year, std::map<int, std::string> names) {
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
    std::string BuildFullName(const std::string& first_name, const std::string& last_name) {
        if (first_name == "" && last_name == "")
            return "Incognito";
        else if (first_name.empty())
            return last_name + " with unknown first name";
        else if (last_name.empty())
            return first_name + " with unknown last name";
        else
            return first_name + " " + last_name;
    }
    std::string BuildJoinedString(std::vector<std::string> name_history) {
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
    Person person;

    person.ChangeFirstName(1965, "Polina");
    person.ChangeLastName(1967, "Sergeeva");
    for (int year : {1900, 1965, 1990}) {
        std::cout << person.GetFullNameWithHistory(year) << std::endl;
    }

    person.ChangeFirstName(1970, "Appolinaria");
    for (int year : {1969, 1970}) {
        std::cout << person.GetFullNameWithHistory(year) << std::endl;
    }

    person.ChangeLastName(1968, "Volkova");
    for (int year : {1969, 1970}) {
        std::cout << person.GetFullNameWithHistory(year) << std::endl;
    }

    person.ChangeFirstName(1990, "Polina");
    person.ChangeLastName(1990, "Volkova-Sergeeva");
    std::cout << person.GetFullNameWithHistory(1990) << std::endl;

    person.ChangeFirstName(1966, "Pauline");
    std::cout << person.GetFullNameWithHistory(1966) << std::endl;

    person.ChangeLastName(1960, "Sergeeva");
    for (int year : {1960, 1967}) {
        std::cout << person.GetFullNameWithHistory(year) << std::endl;
    }

    person.ChangeLastName(1961, "Ivanova");
    std::cout << person.GetFullNameWithHistory(1967) << std::endl;

    return 0;
}
