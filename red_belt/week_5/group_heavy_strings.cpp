#include "test_runner.h"

#include <algorithm>
#include <string>
#include <vector>
#include <set>
#include <map>

using namespace std;

// Объявляем Group<String> для произвольного типа String
// синонимом vector<String>.
// Благодаря этому в качестве возвращаемого значения
// функции можно указать не малопонятный вектор векторов,
// а вектор групп — vector<Group<String>>.
template <typename String>
using Group = vector<String>;

// Ещё один шаблонный синоним типа
// позволяет вместо громоздкого typename String::value_type
// использовать Char<String>
template <typename String>
using Char = typename String::value_type;

template <typename String>
vector<Group<String>> GroupHeavyStrings(vector<String> strings) {
    map<set<Char<String>>, Group<String>> groups_map;
    for (String& s : strings) {
        set<Char<String>> embedding(s.begin(), s.end());
        groups_map[move(embedding)].push_back(move(s));
    }

    vector<Group<String>> result;
    for (auto& it : groups_map) {
        result.push_back(move(it.second));
    }
    return result;
}


void TestGroupingABC() {
    vector<string> strings = {"caab", "abc", "cccc", "bacc", "c"};
    auto groups = GroupHeavyStrings(strings);
    ASSERT_EQUAL(groups.size(), 2u);
    sort(begin(groups), end(groups));  // Порядок групп не имеет значения
    ASSERT_EQUAL(groups[0], vector<string>({"caab", "abc", "bacc"}));
    ASSERT_EQUAL(groups[1], vector<string>({"cccc", "c"}));
}

void TestGroupingReal() {
    vector<string> strings = {"law", "port", "top", "laptop", "pot", "paloalto", "wall", "awl"};
    auto groups = GroupHeavyStrings(strings);
    ASSERT_EQUAL(groups.size(), 4u);
    sort(begin(groups), end(groups));  // Порядок групп не имеет значения
    ASSERT_EQUAL(groups[0], vector<string>({"laptop", "paloalto"}));
    ASSERT_EQUAL(groups[1], vector<string>({"law", "wall", "awl"}));
    ASSERT_EQUAL(groups[2], vector<string>({"port"}));
    ASSERT_EQUAL(groups[3], vector<string>({"top", "pot"}));
}

class MyChar {
public:
    MyChar(char c): c_(c) {}
    MyChar(const MyChar& other) {
        c_ = other.c_;
        copy_counter_++;
        if (copy_counter_ > 34) {
            //            cout << c_ << " " << copy_counter_ << endl;
        }
    }
    void operator=(const MyChar& other) {
        c_ = other.c_;
    }

    char Char() const {
        return c_;
    }

private:
    char c_;
    inline static int copy_counter_ = 0;
};

bool operator==(const MyChar& lhs, const MyChar& rhs) {
    return lhs.Char() == rhs.Char();
}

bool operator<(const MyChar& lhs, const MyChar& rhs) {
    return lhs.Char() < rhs.Char();
}

ostream& operator<<(ostream& os, const MyChar& c) {
    return os << c.Char();
}

void TestCopyCharOnce() {
    vector<vector<MyChar>> strings = {{'l', 'a', 'w'},
                                      {'p', 'o', 'r', 't'},
                                      {'t', 'o', 'p'},
                                      {'l', 'a', 'p', 't', 'o', 'p'},
                                      {'p', 'o', 't'},
                                      {'p', 'a', 'l', 'o', 'a', 'l', 't', 'o'},
                                      {'w', 'a', 'l', 'l'},
                                      {'a', 'w', 'l'}};
    auto groups = GroupHeavyStrings(strings);
    ASSERT_EQUAL(groups.size(), 4u);
    sort(begin(groups), end(groups));  // Порядок групп не имеет значения
    ASSERT_EQUAL(groups[0], vector<vector<MyChar>>({{'l', 'a', 'p', 't', 'o', 'p'},
                                                    {'p', 'a', 'l', 'o', 'a', 'l', 't', 'o'}}));
    ASSERT_EQUAL(groups[1], vector<vector<MyChar>>({{'l', 'a', 'w'},
                                                    {'w', 'a', 'l', 'l'},
                                                    {'a', 'w', 'l'}}));
    ASSERT_EQUAL(groups[2], vector<vector<MyChar>>({{'p', 'o', 'r', 't'}}));
    ASSERT_EQUAL(groups[3], vector<vector<MyChar>>({{'t', 'o', 'p'}, {'p', 'o', 't'}}));
}

int main() {
    TestRunner tr;
    RUN_TEST(tr, TestGroupingABC);
    RUN_TEST(tr, TestGroupingReal);
    RUN_TEST(tr, TestCopyCharOnce);
    return 0;
}
