#include <iostream>
#include <map>
#include <set>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>
#include <algorithm>

using namespace std;

struct FullName
{
    std::string first_name;
    std::string last_name;
};

ostream& operator << (ostream& os, const FullName& full_name) {
    return os << full_name.first_name << ", " << full_name.last_name;
}

template <class K, class V>
ostream& operator << (ostream& os, const pair<K, V>& kv) {
    return os << "{" << kv.first << ": " << kv.second << "}";
}

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
//        auto it = history.find([](const FullName* full_name) { return full_name->first_name != "";});
        auto it = history.upper_bound(year);
        if (it == history.begin() && it->first > year)
            return full_name;

        do {
            it--;
            if (!it->second.first_name.empty() && full_name.first_name.empty())
                full_name.first_name = it->second.first_name;
            if (!it->second.last_name.empty() && full_name.last_name.empty())
                full_name.last_name = it->second.last_name;
            if (!full_name.first_name.empty() && !full_name.last_name.empty())
                break;
        } while(it != history.begin());
        return full_name;
    }

};

#ifdef LOCAL_BUILD
int main() {
    Person person;

    person.ChangeFirstName(1965, "Polina");
    person.ChangeLastName(1967, "Sergeeva");
    for (int year : {1960, 1965, 1990}) {
        cout << person.GetFullName(year) << endl;
    }

    person.ChangeFirstName(1970, "Appolinaria");
    for (int year : {1969, 1970}) {
        cout << person.GetFullName(year) << endl;
    }

    person.ChangeLastName(1968, "Volkova");
    for (int year : {1969, 1970}) {
        cout << person.GetFullName(year) << endl;
    }

    return 0;
}

#endif
