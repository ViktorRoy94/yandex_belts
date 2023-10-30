#include <algorithm>
#include <numeric>
#include <iostream>
#include <string>
#include <vector>
#include <iomanip>
#include <ctime>
#include "test_runner.h"

using namespace std;

struct Date
{
    int year;
    int month;
    int day;

    time_t AsTimestamp() const {
        std::tm t;
        t.tm_sec   = 0;
        t.tm_min   = 0;
        t.tm_hour  = 0;
        t.tm_mday  = day;
        t.tm_mon   = month - 1;
        t.tm_year  = year - 1900;
        t.tm_isdst = 0;
        return mktime(&t);
    }
};

int ComputeDaysDiff(const Date& date_to, const Date& date_from) {
    const time_t timestamp_to = date_to.AsTimestamp();
    const time_t timestamp_from = date_from.AsTimestamp();
    static const int SECONDS_IN_DAY = 60 * 60 * 24;
    return (timestamp_to - timestamp_from) / SECONDS_IN_DAY;
}

std::istream& operator>>(std::istream& in_stream, Date& d) {
    in_stream >> d.year;
    in_stream.ignore(1);
    in_stream >> d.month;
    in_stream.ignore(1);
    in_stream >> d.day;

    return in_stream;
}
ostream& operator<<(ostream& out_stream, const Date& d) {
    out_stream << setw(4) << setfill('0') << d.year << "-";
    out_stream << setw(2) << setfill('0') << d.month << "-";
    out_stream << setw(2) << setfill('0') << d.day;
    return out_stream;
}
bool operator==(const Date& lhs, const Date& rhs) {
    return tie(lhs.year, lhs.month, lhs.day) ==
           tie(rhs.year, rhs.month, rhs.day);
}

int DayIndex(const Date& d, const Date& start_point) {
    return ComputeDaysDiff(d, start_point);
}

class PrivateBudget
{
public:
    PrivateBudget() {
        data_.fill(0);
    }

    double ComputeIncome(const Date& from, const Date& to) const {
        int begin_idx = DayIndex(from, ZERO_DATE);
        int end_idx = DayIndex(to, ZERO_DATE) + 1;
        double sum = accumulate(data_.begin() + begin_idx,
                                data_.begin() + end_idx,
                                0.0);
        return sum ;
    }

    void Earn(const Date& from, const Date& to, int value) {
        int begin_idx = DayIndex(from, ZERO_DATE);
        int end_idx = DayIndex(to, ZERO_DATE) + 1;
        double value_chunk = value * 1.0 / (end_idx - begin_idx);
        for (auto i = begin_idx; i < end_idx; i++) {
            data_[i] += value_chunk;
        }
    }
    void PayTax(const Date& from, const Date& to, int percantage = 13) {
        int begin_idx = DayIndex(from, ZERO_DATE);
        int end_idx = DayIndex(to, ZERO_DATE) + 1;
        for (auto i = begin_idx; i < end_idx; i++) {
            data_[i] *= (100 - percantage) / 100.0;
        }
    }

private:
    inline static constexpr int MAX_DAYS_COUNT = 36525;
    inline static constexpr Date ZERO_DATE = Date({2000, 1, 1});
    array<double, MAX_DAYS_COUNT> data_;
};

enum class CommandType {
    ComputeIncome,
    Earn,
    PayTax
};
ostream& operator<<(ostream& out_stream, const CommandType& type) {
    out_stream << static_cast<int>(type);
    return out_stream;
}

struct Query
{
    CommandType command;
    Date from;
    Date to;
    optional<double> value = nullopt;
};

Query ParseQuery(const string& str) {
    stringstream query_stream(str);

    Query q;
    string command;
    query_stream >> command;

    if (command == "ComputeIncome") {
        q.command = CommandType::ComputeIncome;
    } else if (command == "Earn") {
        q.command = CommandType::Earn;
    } else if (command == "PayTax") {
        q.command = CommandType::PayTax;
    }

    query_stream >> q.from >> q.to;

    if (command == "Earn") {
        double v;
        query_stream >> v;
        q.value = optional<double>(v);
    }

    return q;
}
ostream& operator<<(ostream& out_stream, const Query& q) {
    out_stream << q.command << " " << q.from << " " << q.to << " ";
    if (q.value.has_value()) {
        out_stream << q.value.value() << " ";
    }
    return out_stream;
}
bool operator==(const Query& lhs, const Query& rhs) {
    return tie(lhs.command, lhs.from, lhs.to) ==
           tie(rhs.command, rhs.from, rhs.to);
}

vector<Query> ReadQueries(istream& in_stream = cin) {
    size_t count;
    in_stream >> count;
    in_stream.ignore();

    vector<Query> queries(count);
    for (auto& q : queries) {
        string query_str;
        getline(in_stream, query_str);
        q = ParseQuery(query_str);
    }

    return queries;
}

vector<double> ProcessQueries(const vector<Query>& queries) {
    vector<double> total_income;

    PrivateBudget budget;
    for (const auto& q : queries) {
        if (q.command == CommandType::ComputeIncome) {
            double income = budget.ComputeIncome(q.from, q.to);
            total_income.push_back(income);
        } else if (q.command == CommandType::Earn) {
            budget.Earn(q.from, q.to, q.value.value());
        } else if (q.command == CommandType::PayTax) {
            budget.PayTax(q.from, q.to);
        }
    }
    return total_income;
}

void WriteTotalIncome(const vector<double>& total_income, ostream& out_stream = cout) {
    for (double income : total_income) {
            out_stream << income << endl;
    }
}

void TestParseDate() {
    istringstream input("2000-01-02");
    Date d;
    input >> d;
    ASSERT_EQUAL(d, Date({2000, 1, 2}));
}

void TestParseQuery() {
    {
        Query q = ParseQuery("Earn 2000-01-02 2000-01-06 20");
        ASSERT_EQUAL(q.command, CommandType::Earn);
        ASSERT_EQUAL(q.from, Date({2000, 1, 2}));
        ASSERT_EQUAL(q.to, Date({2000, 1, 6}));
        ASSERT_EQUAL(q.value.has_value(), true);
        ASSERT_EQUAL(q.value.value(), 20);
    }
    {
        Query q = ParseQuery("ComputeIncome 2000-01-01 2001-01-01");
        ASSERT_EQUAL(q.command, CommandType::ComputeIncome);
        ASSERT_EQUAL(q.from, Date({2000, 1, 1}));
        ASSERT_EQUAL(q.to, Date({2001, 1, 1}));
        ASSERT_EQUAL(q.value.has_value(), false);
    }
    {
        Query q = ParseQuery("PayTax 2000-01-02 2000-01-03");
        ASSERT_EQUAL(q.command, CommandType::PayTax);
        ASSERT_EQUAL(q.from, Date({2000, 1, 2}));
        ASSERT_EQUAL(q.to, Date({2000, 1, 3}));
        ASSERT_EQUAL(q.value.has_value(), false);
    }
}

void TestReadQueries() {
    istringstream input("8\n"
                        "Earn 2000-01-02 2000-01-06 20\n"
                        "ComputeIncome 2000-01-01 2001-01-01\n"
                        "PayTax 2000-01-02 2000-01-03\n"
                        "ComputeIncome 2000-01-01 2001-01-01\n"
                        "Earn 2000-01-03 2000-01-03 10\n"
                        "ComputeIncome 2000-01-01 2001-01-01\n"
                        "PayTax 2000-01-03 2000-01-03\n"
                        "ComputeIncome 2000-01-01 2001-01-01\n"
                        );
    const vector<Query> queries = ReadQueries(input);
    const vector<Query> result = {
        Query({CommandType::Earn, Date({2000, 1, 2}), Date({2000, 1, 6}), 20}),
        Query({CommandType::ComputeIncome, Date({2000, 1, 1}), Date({2001, 1, 1})}),
        Query({CommandType::PayTax, Date({2000, 1, 2}), Date({2000, 1, 3})}),
        Query({CommandType::ComputeIncome, Date({2000, 1, 1}), Date({2001, 1, 1})}),
        Query({CommandType::Earn, Date({2000, 1, 3}), Date({2000, 1, 3}), 10}),
        Query({CommandType::ComputeIncome, Date({2000, 1, 1}), Date({2001, 1, 1})}),
        Query({CommandType::PayTax, Date({2000, 1, 3}), Date({2000, 1, 3})}),
        Query({CommandType::ComputeIncome, Date({2000, 1, 1}), Date({2001, 1, 1})}),
        };

    ASSERT_EQUAL(queries, result);
}

void TestProcessQuerys() {
    const vector<Query> queries = {
        Query({CommandType::Earn, Date({2000, 1, 2}), Date({2000, 1, 6}), 20}),
        Query({CommandType::ComputeIncome, Date({2000, 1, 1}), Date({2001, 1, 1})}),
        Query({CommandType::PayTax, Date({2000, 1, 2}), Date({2000, 1, 3})}),
        Query({CommandType::ComputeIncome, Date({2000, 1, 1}), Date({2001, 1, 1})}),
        Query({CommandType::Earn, Date({2000, 1, 3}), Date({2000, 1, 3}), 10}),
        Query({CommandType::ComputeIncome, Date({2000, 1, 1}), Date({2001, 1, 1})}),
        Query({CommandType::PayTax, Date({2000, 1, 3}), Date({2000, 1, 3})}),
        Query({CommandType::ComputeIncome, Date({2000, 1, 1}), Date({2001, 1, 1})}),
        };

    const vector<double> result = ProcessQueries(queries);

    ASSERT_EQUAL(result, vector<double>({20, 18.96, 28.96, 27.2076}));
}

void TestWriteTotalIncome() {
    const vector<double> total_income = {20, 18.96, 28.96, 27.2076};

    ostringstream output;
    WriteTotalIncome(total_income, output);

    ASSERT_EQUAL(output.str(), "20\n"
                               "18.96\n"
                               "28.96\n"
                               "27.2076\n"
    );
}

void TestPipeline() {
    istringstream input("8\n"
                        "Earn 2000-01-02 2000-01-06 20\n"
                        "ComputeIncome 2000-01-01 2001-01-01\n"
                        "PayTax 2000-01-02 2000-01-03\n"
                        "ComputeIncome 2000-01-01 2001-01-01\n"
                        "Earn 2000-01-03 2000-01-03 10\n"
                        "ComputeIncome 2000-01-01 2001-01-01\n"
                        "PayTax 2000-01-03 2000-01-03\n"
                        "ComputeIncome 2000-01-01 2001-01-01\n");
    ostringstream output;

    const vector<Query> queries = ReadQueries(input);
    const vector<double> total_income = ProcessQueries(queries);
    WriteTotalIncome(total_income, output);
    ASSERT_EQUAL(output.str(), "20\n"
                               "18.96\n"
                               "28.96\n"
                               "27.2076\n");
}

void TestDayIndex() {
    ASSERT_EQUAL(DayIndex(Date({2000, 1, 1}), Date({2000, 1, 1})), 0);
    ASSERT_EQUAL(DayIndex(Date({2099, 12, 31}), Date({2000, 1, 1})), 36524);
}

int main() {
#ifdef LOCAL_BUILD
    TestRunner tr;
    RUN_TEST(tr, TestParseDate);
    RUN_TEST(tr, TestParseQuery);
    RUN_TEST(tr, TestReadQueries);
    RUN_TEST(tr, TestProcessQuerys);
    RUN_TEST(tr, TestWriteTotalIncome);
    RUN_TEST(tr, TestPipeline);
    RUN_TEST(tr, TestDayIndex);
#else
    cout.precision(25);
    const vector<Query> queries = ReadQueries();
    const vector<double> result = ProcessQueries(queries);
    WriteTotalIncome(result);
#endif
    return 0;
}
