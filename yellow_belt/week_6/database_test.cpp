#include "database.h"
#include "condition_parser.h"
#include "test_runner.h"

#include <sstream>

void PrintFindIfEntries(ostream& os, const vector<pair<Date, string>>& entries);
string FindIfShortcut(const Database& db, istream& is);
int RemoveIfShortcut(Database& db, istream& is);

void TestAddPrint();
void TestLast();
void TestFindIf();
void TestRemoveIf();
void TestWholeClass();

void TestDatabase() {
    TestAddPrint();
    TestLast();
    TestFindIf();
    TestRemoveIf();
    TestWholeClass();
}

void TestAddPrint() {
    {
        Database db;
        db.Add(Date(2017, 1, 1), "Holiday");

        stringstream ss;
        db.Print(ss);

        string answer = "2017-01-01 Holiday\n";
        AssertEqual(ss.str(), answer, "Database Add Print test 1");
    }

    {
        Database db;
        db.Add(Date(2017, 1, 1), "Holiday");
        db.Add(Date(2017, 3, 8), "Holiday");

        stringstream ss;
        db.Print(ss);

        string answer = "2017-01-01 Holiday\n2017-03-08 Holiday\n";
        AssertEqual(ss.str(), answer, "Database Add Print test 2");
    }

    {
        Database db;
        db.Add(Date(2017, 1, 1), "Holiday");
        db.Add(Date(2017, 1, 1), "Holiday");

        stringstream ss;
        db.Print(ss);

        string answer = "2017-01-01 Holiday\n";
        AssertEqual(ss.str(), answer, "Database Add Print test 3");
    }

    {
        Database db;
        db.Add(Date(2017, 1, 1), "Holiday1");
        db.Add(Date(2017, 1, 1), "Holiday2");

        stringstream ss;
        db.Print(ss);

        string answer = "2017-01-01 Holiday1\n2017-01-01 Holiday2\n";
        AssertEqual(ss.str(), answer, "Database Add Print test 3");
    }

    {
        Database db;
        db.Add(Date(2017, 1, 1), "Holiday");
        db.Add(Date(2017, 3, 8), "Holiday");
        db.Add(Date(2017, 1, 1), "New Year");
        db.Add(Date(2017, 1, 1), "New Year");

        stringstream ss;
        db.Print(ss);

        string answer = "2017-01-01 Holiday\n2017-01-01 New Year\n2017-03-08 Holiday\n";
        AssertEqual(ss.str(), answer, "Database Add Print test 4");
    }
}

void TestLast() {
    {
        Database db;
        db.Add(Date(2017, 1, 1), "New Year");
        db.Add(Date(2017, 3, 8), "Holiday");
        db.Add(Date(2017, 1, 1), "Holiday");

        AssertEqual(db.Last(Date(2016, 12, 31)), "No entries", "Database Last test 1");
        AssertEqual(db.Last(Date(2017,  1,  1)), "2017-01-01 Holiday", "Database Last test 2");
        AssertEqual(db.Last(Date(2017,  6,  1)), "2017-03-08 Holiday", "Database Last test 3");
    }

    {
        Database db;
        db.Add(Date(2017, 11, 21), "Tuesday");
        db.Add(Date(2017, 11, 20), "Monday");
        db.Add(Date(2017, 11, 21), "Weekly meeting");

        AssertEqual(db.Last(Date(2017, 11, 30)), "2017-11-21 Weekly meeting", "Database Last test 4");
    }
}



void TestFindIf() {

    {
        Database db;
        db.Add(Date(2017, 1, 1), "Holiday");
        db.Add(Date(2017, 3, 8), "Holiday");
        db.Add(Date(2017, 1, 1), "New Year");

        istringstream is(R"(event != "working day")");
        string result = FindIfShortcut(db, is);
        string answer = "2017-01-01 Holiday\n"
                        "2017-01-01 New Year\n"
                        "2017-03-08 Holiday\n"
                        "Found 3 entries\n";
        AssertEqual(result, answer, "Database FindIf test 1");
    }

    {
        Database db;
        db.Add(Date(2017, 1, 1), "Holiday1");
        db.Add(Date(2017, 3, 1), "Holiday2");
        db.Add(Date(2017, 6, 1), "New Year1");
        db.Add(Date(2018, 1, 1), "New Year2");

        istringstream is(R"(date > 2017-01-01 AND date < 2017-06-01)");
        string result = FindIfShortcut(db, is);
        string answer = "2017-03-01 Holiday2\n"
                        "Found 1 entries\n";
        AssertEqual(result, answer, "Database FindIf test 2");
    }

    {
        Database db;
        db.Add(Date(2017, 1, 1), "Holiday1");
        db.Add(Date(2017, 3, 1), "Holiday2");
        db.Add(Date(2017, 6, 1), "New Year1");
        db.Add(Date(2018, 1, 1), "New Year2");

        istringstream is(R"(date >= 2017-01-01 AND date <= 2018-06-01 AND event == "New Year1")");
        string result = FindIfShortcut(db, is);
        string answer = "2017-06-01 New Year1\n"
                        "Found 1 entries\n";
        AssertEqual(result, answer, "Database FindIf test 6");
    }

    {
        Database db;
        db.Add(Date(2017, 1, 1), "3");
        db.Add(Date(2017, 1, 1), "2");
        db.Add(Date(2017, 1, 1), "1");

        istringstream is(R"(date == 2017-01-01)");
        string result = FindIfShortcut(db, is);
        string answer = "2017-01-01 3\n"
                        "2017-01-01 2\n"
                        "2017-01-01 1\n"
                        "Found 3 entries\n";
        AssertEqual(result, answer, "Database FindIf test 6");
    }
}

void TestRemoveIf() {

    {
        Database db;
        db.Add(Date(2017, 1, 1), "Holiday1");
        db.Add(Date(2017, 3, 1), "Holiday2");
        db.Add(Date(2017, 6, 1), "New Year1");

        istringstream is(R"(date > 2017-01-01 AND date < 2017-06-01)");
        int count = RemoveIfShortcut(db, is);
        AssertEqual(count, 1, "Database RemoveIf test 1");

        stringstream ss;
        db.Print(ss);

        string answer = "2017-01-01 Holiday1\n"
                        "2017-06-01 New Year1\n";
        AssertEqual(ss.str(), answer, "Database RemoveIf test 2");
    }

    {
        Database db;
        db.Add(Date(2017, 11, 21), "Tuesday");
        db.Add(Date(2017, 11, 20), "Monday");
        db.Add(Date(2017, 11, 21), "Weekly meeting");

        istringstream is(R"(date > 2017-11-20)");
        int count = RemoveIfShortcut(db, is);
        AssertEqual(count, 2, "Database RemoveIf test 3");
    }

    {
        Database db;
        db.Add(Date(2017, 1, 2), "1");
        db.Add(Date(2017, 1, 2), "2");
        db.Add(Date(2017, 1, 2), "3");

        istringstream is("");
        int count = RemoveIfShortcut(db, is);
        AssertEqual(count, 3, "Database RemoveIf test 4");

        db.Add(Date(2017, 1, 2), "1");
        db.Add(Date(2017, 1, 2), "2");
        db.Add(Date(2017, 1, 2), "3");

        count = RemoveIfShortcut(db, is);
        AssertEqual(count, 3, "Database RemoveIf test 5");
    }
}

void TestWholeClass() {
    Database db;
    db.Add(Date(2017, 11, 21), "Tuesday");
    db.Add(Date(2017, 11, 20), "Monday");
    db.Add(Date(2017, 11, 21), "Weekly meeting");

    istringstream is_find_if(R"(event != "Weekly meeting")");
    istringstream is_remove_if(R"(date > 2017-11-20)");

    stringstream ss;
    db.Print(ss);
    ss << FindIfShortcut(db, is_find_if);
    ss << db.Last(Date(2017, 11, 30)) << endl;
    ss << "Removed " << RemoveIfShortcut(db, is_remove_if) << " entries" << endl;
    ss << db.Last(Date(2017, 11, 30)) << endl;
    ss << db.Last(Date(2017, 11, 01)) << endl;

    string answer = "2017-11-20 Monday\n"
                    "2017-11-21 Tuesday\n"
                    "2017-11-21 Weekly meeting\n"
                    "2017-11-20 Monday\n"
                    "2017-11-21 Tuesday\n"
                    "Found 2 entries\n"
                    "2017-11-21 Weekly meeting\n"
                    "Removed 2 entries\n"
                    "2017-11-20 Monday\n"
                    "No entries\n"
                    ;
    AssertEqual(ss.str(), answer, "Database WholeTest");
}

void PrintFindIfEntries(ostream& os, const vector<pair<Date, string>>& entries) {
    for (const auto& entry : entries) {
        os << entry << endl;
    }
    os << "Found " << entries.size() << " entries" << endl;
}

string FindIfShortcut(const Database& db, istream& is) {
    auto condition = ParseCondition(is);
    auto predicate = [condition](const Date& date, const string& event) {
        return condition->Evaluate(date, event);
    };
    const auto entries = db.FindIf(predicate);
    stringstream ss;
    PrintFindIfEntries(ss, entries);
    return ss.str();
}

int RemoveIfShortcut(Database& db, istream& is) {
    auto condition = ParseCondition(is);
    auto predicate = [condition](const Date& date, const string& event) {
        return condition->Evaluate(date, event);
    };
    return db.RemoveIf(predicate);
}