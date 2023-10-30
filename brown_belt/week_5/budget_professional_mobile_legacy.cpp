#include <array>
#include <cassert>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <optional>
#include <sstream>
#include <stdexcept>
#include <string_view>
#include <utility>

#include "test_runner.h"

using namespace std;

pair<string_view, optional<string_view>> SplitTwoStrict(string_view s, string_view delimiter = " ") {
    const size_t pos = s.find(delimiter);
    if (pos == s.npos) {
        return {s, nullopt};
    } else {
        return {s.substr(0, pos), s.substr(pos + delimiter.length())};
    }
}

pair<string_view, string_view> SplitTwo(string_view s, string_view delimiter = " ") {
    const auto [lhs, rhs_opt] = SplitTwoStrict(s, delimiter);
            return {lhs, rhs_opt.value_or("")};
}

string_view ReadToken(string_view& s, string_view delimiter = " ") {
    const auto [lhs, rhs] = SplitTwo(s, delimiter);
            s = rhs;
            return lhs;
}

int ConvertToInt(string_view str) {
    // use std::from_chars when available to git rid of string copy
    size_t pos;
    const int result = stoi(string(str), &pos);
    if (pos != str.length()) {
        std::stringstream error;
        error << "string " << str << " contains " << (str.length() - pos) << " trailing chars";
        throw invalid_argument(error.str());
    }
    return result;
}

template <typename Number>
void ValidateBounds(Number number_to_check, Number min_value, Number max_value) {
    if (number_to_check < min_value || number_to_check > max_value) {
        std::stringstream error;
        error << number_to_check << " is out of [" << min_value << ", " << max_value << "]";
        throw out_of_range(error.str());
    }
}

class Date {
public:
    static Date FromString(string_view str) {
        const int year = ConvertToInt(ReadToken(str, "-"));
        const int month = ConvertToInt(ReadToken(str, "-"));
        ValidateBounds(month, 1, 12);
        const int day = ConvertToInt(str);
        ValidateBounds(day, 1, 31);
        return {year, month, day};
    }

    // Weird legacy, can't wait for std::chrono::year_month_day
    time_t AsTimestamp() const {
        std::tm t;
        t.tm_sec  = 0;
        t.tm_min  = 0;
        t.tm_hour = 0;
        t.tm_mday = day_;
        t.tm_mon  = month_ - 1;
        t.tm_year = year_ - 1900;
        t.tm_isdst = 0;
        return mktime(&t);
    }

private:
    int year_;
    int month_;
    int day_;

    Date(int year, int month, int day)
        : year_(year), month_(month), day_(day)
    {}
};

int ComputeDaysDiff(const Date& date_to, const Date& date_from) {
    const time_t timestamp_to = date_to.AsTimestamp();
    const time_t timestamp_from = date_from.AsTimestamp();
    static constexpr int SECONDS_IN_DAY = 60 * 60 * 24;
    return (timestamp_to - timestamp_from) / SECONDS_IN_DAY;
}

static const Date START_DATE = Date::FromString("2000-01-01");
static const Date END_DATE = Date::FromString("2100-01-01");
static const size_t DAY_COUNT = ComputeDaysDiff(END_DATE, START_DATE);
static const size_t DAY_COUNT_P2 = 1 << 16;
static const size_t VERTEX_COUNT = DAY_COUNT_P2 * 2;

size_t ComputeDayIndex(const Date& date) {
    return ComputeDaysDiff(date, START_DATE);
}

struct MoneyState {
    double earned = 0.0;
    double spent = 0.0;

    double ComputeIncome() const {
        return earned - spent;
    }

    MoneyState& operator+=(const MoneyState& other) {
        earned += other.earned;
        spent += other.spent;
        return *this;
    }

    MoneyState operator+(const MoneyState& other) const {
        return MoneyState(*this) += other;
    }

    MoneyState operator*(double factor) const {
        return {earned * factor, spent * factor};
    }

    MoneyState& operator*=(double factor) {
        earned *= factor;
        return *this;
    }
};


array<MoneyState, VERTEX_COUNT> tree_values, tree_add;
array<double, VERTEX_COUNT> tree_factor;

void Init() {
    tree_values.fill(MoneyState());
    tree_add.fill(MoneyState());
    tree_factor.fill(1);
}

void Push(size_t v, size_t l, size_t r) {
    for (size_t w = v * 2; w <= v * 2 + 1; ++w) {
        if (w < VERTEX_COUNT) {
            tree_factor[w] *= tree_factor[v];
            (tree_add[w] *= tree_factor[v]) += tree_add[v];
            (tree_values[w] *= tree_factor[v]) += tree_add[v] * ((r - l) / 2);
        }
    }
    tree_factor[v] = 1;
    tree_add[v] = MoneyState();
}

double ComputeSum(size_t v, size_t l, size_t r, size_t ql, size_t qr) {
    if (v >= VERTEX_COUNT || qr <= l || r <= ql) {
        return 0;
    }
    Push(v, l, r);
    if (ql <= l && r <= qr) {
        return tree_values[v].ComputeIncome();
    }
    return ComputeSum(v * 2, l, (l + r) / 2, ql, qr)
            + ComputeSum(v * 2 + 1, (l + r) / 2, r, ql, qr);
}

void Add(size_t v, size_t l, size_t r, size_t ql, size_t qr, const MoneyState& value) {
    if (v >= VERTEX_COUNT || qr <= l || r <= ql) {
        return;
    }
    Push(v, l, r);
    if (ql <= l && r <= qr) {
        tree_add[v]+= value;
        tree_values[v] += value * (r - l);
        return;
    }
    Add(v * 2, l, (l + r) / 2, ql, qr, value);
    Add(v * 2 + 1, (l + r) / 2, r, ql, qr, value);
    tree_values[v] =
            (v * 2 < VERTEX_COUNT ? tree_values[v * 2] : MoneyState())
            + (v * 2 + 1 < VERTEX_COUNT ? tree_values[v * 2 + 1] : MoneyState());
}

void Multiply(size_t v, size_t l, size_t r, size_t ql, size_t qr, double f) {
    if (v >= VERTEX_COUNT || qr <= l || r <= ql) {
        return;
    }
    Push(v, l, r);
    if (ql <= l && r <= qr) {
        tree_factor[v] *= f;
        tree_add[v] *= f;
        tree_values[v] *= f;
        return;
    }
    Multiply(v * 2, l, (l + r) / 2, ql, qr, f);
    Multiply(v * 2 + 1, (l + r) / 2, r, ql, qr, f);
    tree_values[v] =
            (v * 2 < VERTEX_COUNT ? tree_values[v * 2] : MoneyState())
            + (v * 2 + 1 < VERTEX_COUNT ? tree_values[v * 2 + 1] : MoneyState());
}


void Process(istream& in_stream = cin, ostream& out_stream = cout) {
    cout.precision(25);
    std::ios::sync_with_stdio(false);
    cin.tie(NULL);
    assert(DAY_COUNT <= DAY_COUNT_P2 && DAY_COUNT_P2 < DAY_COUNT * 2);

    Init();

    int q;
    in_stream >> q;

    for (int i = 0; i < q; ++i) {
        string query_type;
        in_stream >> query_type;

        string date_from_str, date_to_str;
        in_stream >> date_from_str >> date_to_str;

        auto idx_from = ComputeDayIndex(Date::FromString(date_from_str));
        auto idx_to = ComputeDayIndex(Date::FromString(date_to_str)) + 1;

        if (query_type == "ComputeIncome") {
            out_stream << ComputeSum(1, 0, DAY_COUNT_P2, idx_from, idx_to) << endl;
        } else if (query_type == "PayTax") {
            double value;
            in_stream >> value;
            Multiply(1, 0, DAY_COUNT_P2, idx_from, idx_to, 1 - value / 100.0);
        } else if (query_type == "Earn") {
            double value;
            in_stream >> value;
            Add(1, 0, DAY_COUNT_P2, idx_from, idx_to, MoneyState({value / (idx_to - idx_from), 0}));
        } else if (query_type == "Spend") {
            double value;
            in_stream >> value;
            Add(1, 0, DAY_COUNT_P2, idx_from, idx_to, MoneyState({0, value / (idx_to - idx_from)}));
        }
    }
}

void TestOldPipeline() {
    istringstream input("8\n"
                        "Earn 2000-01-02 2000-01-06 20\n"
                        "ComputeIncome 2000-01-01 2001-01-01\n"
                        "PayTax 2000-01-02 2000-01-03 13\n"
                        "ComputeIncome 2000-01-01 2001-01-01\n"
                        "Earn 2000-01-03 2000-01-03 10\n"
                        "ComputeIncome 2000-01-01 2001-01-01\n"
                        "PayTax 2000-01-03 2000-01-03 13\n"
                        "ComputeIncome 2000-01-01 2001-01-01\n");
    ostringstream output;

    Process(input, output);
    ASSERT_EQUAL(output.str(), "20\n"
                               "18.96\n"
                               "28.96\n"
                               "27.2076\n");
}


void TestPipeline() {
    istringstream input("8\n"
                        "Earn 2000-01-02 2000-01-06 20\n"
                        "ComputeIncome 2000-01-01 2001-01-01\n"
                        "PayTax 2000-01-02 2000-01-03 13\n"
                        "ComputeIncome 2000-01-01 2001-01-01\n"
                        "Spend 2000-12-30 2001-01-02 14\n"
                        "ComputeIncome 2000-01-01 2001-01-01\n"
                        "PayTax 2000-12-30 2000-12-30 13\n"
                        "ComputeIncome 2000-01-01 2001-01-01\n");
    ostringstream output;
    Process(input, output);
    ASSERT_EQUAL(output.str(), "20\n"
                               "18.96\n"
                               "8.46\n"
                               "8.46\n");
}

void Test8() {
    {
        istringstream input("8\n"
                            "Earn 2000-01-1 2000-01-5 100\n"
                            "ComputeIncome 2000-01-01 2001-01-01\n"
                            "PayTax 2000-01-1 2000-01-5 50\n"
                            "ComputeIncome 2000-01-01 2001-01-01\n"
                            "Spend 2000-12-30 2001-01-02 5\n"
                            "ComputeIncome 2000-01-01 2001-01-01\n"
                            "PayTax 2000-01-1 2000-01-5 50\n"
                            "ComputeIncome 2000-01-01 2001-01-01\n"
                            );
        ostringstream output;
        Process(input, output);
        ASSERT_EQUAL(output.str(), "100\n"
                                   "50\n"
                                   "46.25\n"
                                   "21.25\n");
    }
}


void Test14() {
    {
        istringstream input("4\n"
                            "Earn 2000-01-1 2000-01-5 100\n"
                            "ComputeIncome 2000-01-01 2001-01-01\n"
                            "PayTax 2000-01-1 2000-01-5 100\n"
                            "ComputeIncome 2000-01-01 2001-01-01\n");
        ostringstream output;
        Process(input, output);
        ASSERT_EQUAL(output.str(), "100\n"
                                   "0\n");
    }
    {
        istringstream input("12\n"
                            "Earn 2000-02-11 2000-02-20 100\n"
                            "ComputeIncome 2000-01-01 2001-01-01\n"
                            "PayTax 2000-02-16 2000-02-20 20\n"
                            "ComputeIncome 2000-01-01 2001-01-01\n"
                            "PayTax 2000-02-14 2000-02-17 100\n"
                            "ComputeIncome 2000-01-01 2001-01-01\n"
                            "Earn 2000-02-11 2000-02-20 100\n"
                            "ComputeIncome 2000-01-01 2001-01-01\n"
                            "PayTax 2000-02-15 2000-02-16 50\n"
                            "ComputeIncome 2000-01-01 2001-01-01\n"
                            "PayTax 2000-02-11 2000-02-20 0\n"
                            "ComputeIncome 2000-01-01 2001-01-01\n");
        ostringstream output;
        Process(input, output);
        ASSERT_EQUAL(output.str(), "100\n"
                                   "90\n"
                                   "54\n"
                                   "154\n"
                                   "144\n"
                                   "144\n");
    }
    {
        istringstream input("8\n"
                            "Earn 2000-01-1 2000-01-5 100\n"
                            "ComputeIncome 2000-01-01 2001-01-01\n"
                            "PayTax 2000-01-1 2000-01-5 50\n"
                            "ComputeIncome 2000-01-01 2001-01-01\n"
                            "PayTax 2000-01-1 2000-01-5 20\n"
                            "ComputeIncome 2000-01-01 2001-01-01\n"
                            "PayTax 2000-01-1 2000-01-5 10\n"
                            "ComputeIncome 2000-01-01 2001-01-01\n");
        ostringstream output;
        Process(input, output);
        ASSERT_EQUAL(output.str(), "100\n"
                                   "50\n"
                                   "40\n"
                                   "36\n");
    }
}

int main() {
#ifdef LOCAL_BUILD
    TestRunner tr;
    RUN_TEST(tr, TestOldPipeline);
    RUN_TEST(tr, TestPipeline);
    RUN_TEST(tr, Test8);
    RUN_TEST(tr, Test14);
#else
    Process();
#endif
    return 0;
}
