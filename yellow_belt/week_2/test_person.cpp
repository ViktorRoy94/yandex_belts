#include <iostream>
#include <map>
#include <set>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

using namespace std;

template <class T>
ostream& operator << (ostream& os, const vector<T>& s) {
    os << "{";
    bool first = true;
    for (const auto& x : s) {
        if (!first) {
            os << ", ";
        }
        first = false;
        os << x;
    }
    return os << "}";
}

template <class T>
ostream& operator << (ostream& os, const set<T>& s) {
    os << "{";
    bool first = true;
    for (const auto& x : s) {
        if (!first) {
            os << ", ";
        }
        first = false;
        os << x;
    }
    return os << "}";
}

template <class K, class V>
ostream& operator << (ostream& os, const map<K, V>& m) {
    os << "{";
    bool first = true;
    for (const auto& kv : m) {
        if (!first) {
            os << ", ";
        }
        first = false;
        os << kv.first << ": " << kv.second;
    }
    return os << "}";
}

template<class T, class U>
void AssertEqual(const T& t, const U& u, const string& hint = {}) {
    if (t != u) {
        ostringstream os;
        os << "Assertion failed: " << t << " != " << u;
        if (!hint.empty()) {
            os << " hint: " << hint;
        }
        throw runtime_error(os.str());
    }
}

template<class T, class U>
void AssertNotEqual(const T& t, const U& u, const string& hint = {}) {
    if (t == u) {
        ostringstream os;
        os << "Assertion failed: " << t << " != " << u;
        if (!hint.empty()) {
            os << " hint: " << hint;
        }
        throw runtime_error(os.str());
    }
}

void Assert(bool b, const string& hint) {
    AssertEqual(b, true, hint);
}

class TestRunner {
public:
    template <class TestFunc>
    void RunTest(TestFunc func, const string& test_name) {
        try {
            func();
            cerr << test_name << " OK" << endl;
        } catch (exception& e) {
            ++fail_count;
            cerr << test_name << " fail: " << e.what() << endl;
        } catch (...) {
            ++fail_count;
            cerr << "Unknown exception caught" << endl;
        }
    }

    ~TestRunner() {
        if (fail_count > 0) {
            cerr << fail_count << " unit tests failed. Terminate" << endl;
            exit(1);
        }
    }

private:
    int fail_count = 0;
};

struct FullName
{
    std::string first_name;
    std::string last_name;
};

#ifdef LOCAL_BUILD
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
#endif

void TestIncognito() {
    Person person;
    AssertEqual(person.GetFullName(1900), "Incognito", "1900 No history, should return Incognito");
    AssertEqual(person.GetFullName(1950), "Incognito", "1950 No history, should return Incognito");
    AssertEqual(person.GetFullName(2000), "Incognito", "2000 No history, should return Incognito");
    person.ChangeFirstName(1910, "A");
    person.ChangeLastName(1910, "B");
    AssertEqual(person.GetFullName(1900), "Incognito", "1900 Names were changed in 1910, should return Incognito");
    AssertNotEqual(person.GetFullName(1910), "Incognito", "1910 Names were changed in 1910, shouldn't return Incognito");
    AssertNotEqual(person.GetFullName(2000), "Incognito", "2000 Names were changed in 1910, shouldn't return Incognito");
}

void TestChangeFirstName() {
    Person person;
    AssertNotEqual(person.GetFullName(1910), "A with unknown last name", "1910 No history, should return Incognito");
    person.ChangeFirstName(1900, "A");
    AssertNotEqual(person.GetFullName(1890), "A with unknown last name", "1890 No history, should return Incognito");
    AssertEqual(person.GetFullName(1910), "A with unknown last name", "1910 Should return first name");
    AssertEqual(person.GetFullName(2000), "A with unknown last name", "2000 Should return first name");
    person.ChangeFirstName(1950, "B");
    AssertEqual(person.GetFullName(2000), "B with unknown last name", "2000 Should return first name");
}

void TestChangeLastName() {
    Person person;
    AssertNotEqual(person.GetFullName(1910), "A with unknown first name", "1910 No history, should return Incognito");
    person.ChangeLastName(1900, "A");
    AssertNotEqual(person.GetFullName(1890), "A with unknown first name", "1890 No history, should return Incognito");
    AssertEqual(person.GetFullName(1910), "A with unknown first name", "1910 Should return last name");
    AssertEqual(person.GetFullName(2000), "A with unknown first name", "2000 Should return last name");
    person.ChangeLastName(1950, "B");
    AssertEqual(person.GetFullName(2000), "B with unknown first name", "2000 Should return last name");
}

void TestChangeBothNames() {
    Person person;
    AssertNotEqual(person.GetFullName(1910), "A B", "1910 No history, should return Incognito");
    person.ChangeFirstName(1900, "A");
    AssertNotEqual(person.GetFullName(1905), "A B", "1905 Changed only first name");
    person.ChangeLastName(1910, "B");
    AssertNotEqual(person.GetFullName(1890), "A B", "1890 No history, should return Incognito");
    AssertEqual(person.GetFullName(1910), "A B", "1910 Should return both names");
    AssertEqual(person.GetFullName(2000), "A B", "2000 Should return both names");
    person.ChangeLastName(1950, "C");
    AssertEqual(person.GetFullName(2000), "A C", "2000 Should return both names");
}

int main() {
    TestRunner runner;
    runner.RunTest(TestIncognito, "TestIncognito");
    runner.RunTest(TestChangeFirstName, "TestChangeFirstName");
    runner.RunTest(TestChangeLastName, "TestChangeLastName");
    runner.RunTest(TestChangeBothNames, "TestChangeBothNames");
    return 0;
}
