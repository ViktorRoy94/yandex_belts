#include <iostream>
#include <string>
#include <map>

struct FullName
{
    std::string first_name;
    std::string last_name;
};

class Person {
public:
    void ChangeFirstName(int year, const std::string& first_name) {
        history[year].first_name = first_name;
    }
    void ChangeLastName(int year, const std::string& last_name) {
        history[year].last_name = last_name;
    }
    std::string GetFullName(int year) {
        FullName full_name = FindFullNameByYear(year);

        std::string result = full_name.first_name + " " + full_name.last_name;
        if (full_name.first_name == "" && full_name.last_name == "")
            result = "Incognito";
        else if (full_name.first_name == "")
            result = full_name.last_name + " with unknown first name";
        else if (full_name.last_name == "")
            result = full_name.first_name + " with unknown last name";
        return result;
    }

private:
    std::map<int, FullName> history;

    FullName FindFullNameByYear(int year) {
        FullName full_name;
        auto it = history.begin();
        while (it != history.end() && it->first <= year) {
            if (it->second.first_name != "")
                full_name.first_name = it->second.first_name;
            if (it->second.last_name != "")
                full_name.last_name = it->second.last_name;
            it++;
        }
        return full_name;
    }

};

int main() {
    Person person;

    person.ChangeFirstName(1965, "Polina");
    person.ChangeLastName(1967, "Sergeeva");
    for (int year : {1900, 1965, 1990}) {
        std::cout << person.GetFullName(year) << std::endl;
    }

    person.ChangeFirstName(1970, "Appolinaria");
    for (int year : {1969, 1970}) {
        std::cout << person.GetFullName(year) << std::endl;
    }

    person.ChangeLastName(1968, "Volkova");
    for (int year : {1969, 1970}) {
        std::cout << person.GetFullName(year) << std::endl;
    }

    return 0;
}
