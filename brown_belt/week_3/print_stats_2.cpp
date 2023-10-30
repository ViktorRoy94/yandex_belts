#include <algorithm>
#include <iostream>
#include <vector>
#include <string>
#include <numeric>
#include <sstream>
#include <optional>
#include <tuple>
#include <unordered_map>

using namespace std;

struct Person {
    string name;
    int age, income;
    bool is_male;
};

vector<Person> ReadPeople(istream& input) {
    int count;
    input >> count;

    vector<Person> result(count);
    for (Person& p : result) {
        char gender;
        input >> p.name >> p.age >> p.income >> gender;
        p.is_male = gender == 'M';
    }

    return result;
}

string FindMostPopularName(const vector<Person>& people, char gender) {
    string most_popular_name;
    int count = 0;
    unordered_map<string, int> frequency;
    for (const auto& p : people) {
        if (p.is_male != (gender == 'M')) {
            continue;
        }

        frequency[p.name]++;
        if (frequency[p.name] > count) {
            count = frequency[p.name];
            most_popular_name = p.name;
        } else if (frequency[p.name] == count) {
            most_popular_name = min(p.name, most_popular_name);
        }
    }
    return most_popular_name;
}

struct DataStats {
    vector<Person> sorted_by_age;
    vector<int> adult_count;
    vector<int> cumulative_wealth;
    string popular_male_name;
    string popular_female_name;
};

DataStats BuildDataStats(vector<Person> people) {
    DataStats result;

    result.popular_male_name = FindMostPopularName(people, 'M');
    result.popular_female_name = FindMostPopularName(people, 'W');

    sort(people.begin(), people.end(), [](const Person& lhs, const Person& rhs) {
        return lhs.income > rhs.income;
    });

    auto& wealth = result.cumulative_wealth;
    wealth.resize(people.size());
    if (!people.empty()) {
        wealth[0] = people[0].income;
        for (size_t i = 1; i < people.size(); ++i) {
            wealth[i] = wealth[i - 1] + people[i].income;
        }
    }

    sort(begin(people), end(people), [](const Person& lhs, const Person& rhs) {
        return lhs.age < rhs.age;
    });

    auto& adult_count = result.adult_count;
    adult_count.resize(prev(people.end())->age + 1);
    for (size_t i = 0; i < adult_count.size(); i++) {
        auto it = lower_bound(people.begin(), people.end(), i,
                         [](const Person& lhs, int age) {
                             return lhs.age < age;
                         });
        adult_count[i] = distance(it, people.end());
    }

    result.sorted_by_age = std::move(people);

    return result;
}

int main() {
#ifdef LOCAL_BUILD
    stringstream in("11\n"
                    "Ivan 25 1000 M\n"
                    "Olga 30 623 W\n"
                    "Sergey 24 825 M\n"
                    "Maria 42 1254 W\n"
                    "Mikhail 15 215 M\n"
                    "Oleg 18 230 M\n"
                    "Denis 53 8965 M\n"
                    "Maxim 37 9050 M\n"
                    "Ivan 47 19050 M\n"
                    "Ivan 17 50 M\n"
                    "Olga 23 550 W\n"
                    "AGE 18\n"
                    "AGE 25\n"
                    "WEALTHY 1\n"
                    "WEALTHY 2\n"
                    "WEALTHY 3\n"
                    "WEALTHY 4\n"
                    "WEALTHY 5\n"
                    "WEALTHY 6\n"
                    "WEALTHY 7\n"
                    "WEALTHY 8\n"
                    "WEALTHY 9\n"
                    "WEALTHY 10\n"
                    "WEALTHY 11\n"
                    "POPULAR_NAME M\n"
                    "POPULAR_NAME W");
    cin.rdbuf(in.rdbuf());
#endif

    const DataStats data = BuildDataStats(ReadPeople(cin));

    for (string command; cin >> command; ) {
        if (command == "AGE") {
            int adult_age;
            cin >> adult_age;

            int adult_count = 0;
            if (adult_age < static_cast<int>(data.adult_count.size())) {
                adult_count = data.adult_count[adult_age];
            }

            cout << "There are " << adult_count
                 << " adult people for maturity age " << adult_age << '\n';
        } else if (command == "WEALTHY") {
            int count;
            cin >> count;
            cout << "Top-" << count
                 << " people have total income " << data.cumulative_wealth[count-1] << '\n';
        } else if (command == "POPULAR_NAME") {
            char gender;
            cin >> gender;

            string popular_name = data.popular_male_name;
            if (gender == 'W') {
                popular_name = data.popular_female_name;
            }

            if (popular_name.empty()) {
                cout << "No people of gender " << gender << '\n';
            } else {
                cout << "Most popular name among people of gender "
                     << gender << " is "
                     << popular_name << '\n';
            }
        }
    }
}


//size_t age_command(const vector<Person>& people, int adult_age) {
//    size_t count = 0;
//    for (const auto& p : people) {
//        if (p.age >= adult_age) {
//            count++;
//        }
//    }
//    return count;
//}

//size_t wealthy_command(const vector<Person>& people, int count) {
//    return accumulate(people.begin(), people.begin() + count, 0,
//                      [](int cur, const Person& p) {
//                          return cur += p.income;
//                      });
//}

//string gender_command(const vector<Person>& people, char gender) {
//    string most_popular_name;
//    int count = 0;
//    unordered_map<string, int> frequency;
//    for (const auto& p : people) {
//        if (p.is_male != (gender == 'M')) {
//            continue;
//        }

//        frequency[p.name]++;
//        if (frequency[p.name] > count) {
//            count = frequency[p.name];
//            most_popular_name = p.name;
//        } else if (frequency[p.name] == count) {
//            most_popular_name = min(p.name, most_popular_name);
//        }
//    }
//    return most_popular_name;
//}

//struct AgeCache {
//    size_t people_count;
//};
//struct WealthyCache {
//    size_t total_income;
//};
//struct PopularNameCache {
//    string name;
//};

//int main() {
//#ifdef LOCAL_BUILD
//    stringstream in("11\n"
//                    "Ivan 25 1000 M\n"
//                    "Olga 30 623 W\n"
//                    "Sergey 24 825 M\n"
//                    "Maria 42 1254 W\n"
//                    "Mikhail 15 215 M\n"
//                    "Oleg 18 230 M\n"
//                    "Denis 53 8965 M\n"
//                    "Maxim 37 9050 M\n"
//                    "Ivan 47 19050 M\n"
//                    "Ivan 17 50 M\n"
//                    "Olga 23 550 W\n"
//                    "AGE 18\n"
//                    "AGE 25\n"
//                    "WEALTHY 1\n"
//                    "WEALTHY 2\n"
//                    "WEALTHY 3\n"
//                    "WEALTHY 4\n"
//                    "WEALTHY 5\n"
//                    "WEALTHY 6\n"
//                    "WEALTHY 7\n"
//                    "WEALTHY 8\n"
//                    "WEALTHY 9\n"
//                    "WEALTHY 10\n"
//                    "WEALTHY 11\n"
//                    "POPULAR_NAME M\n"
//                    "POPULAR_NAME W");
//    cin.rdbuf(in.rdbuf());
//#endif

//    const vector<Person> people = []() {
//        vector<Person> data = ReadPeople(cin);
//        sort(begin(data), end(data), [](const Person& lhs, const Person& rhs) {
//            return lhs.income > rhs.income;
//        });
//        return data;
//    }();

//    array<optional<AgeCache>, 200> age_cache;
//    vector<optional<WealthyCache>> wealthy_cache(people.size()+1);
//    array<optional<PopularNameCache>, 2> popular_name_cache;
//    for (string command; cin >> command; ) {
//        if (command == "AGE") {
//            int adult_age;
//            cin >> adult_age;

//            if (!age_cache[adult_age]) {
//                age_cache[adult_age] = AgeCache{age_command(people, adult_age)};
//            }

//            cout << "There are " << age_cache[adult_age]->people_count
//                 << " adult people for maturity age " << adult_age << '\n';
//        } else if (command == "WEALTHY") {
//            int count;
//            cin >> count;

//            if (!wealthy_cache[count]) {
//                wealthy_cache[count] = WealthyCache{wealthy_command(people, count)};
//            }

//            cout << "Top-" << count
//                 << " people have total income " << wealthy_cache[count]->total_income << '\n';
//        } else if (command == "POPULAR_NAME") {
//            char gender;
//            cin >> gender;

//            size_t index = static_cast<size_t>(gender == 'M');
//            if (!popular_name_cache[index]) {
//                popular_name_cache[index] = PopularNameCache{gender_command(people, gender)};
//            }

//            string popular_name = popular_name_cache[gender == 'M']->name;
//            if (popular_name.empty()) {
//                cout << "No people of gender " << gender << '\n';
//            } else {
//                cout << "Most popular name among people of gender "
//                     << gender << " is "
//                     << popular_name << '\n';
//            }
//        }
//    }
//}
