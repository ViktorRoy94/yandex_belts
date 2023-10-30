#include "date.h"

#include <iomanip>
#include <tuple>

Date::Date():day_(0), month_(0), year_(0) {}
Date::Date(int year, int month, int day) {
    if (year < 0 || year > 9999) {
        throw invalid_argument("Year value is invalid: " + to_string(year));
    }

    if (month < 1 || month > 12) {
        throw invalid_argument("Month value is invalid: " + to_string(month));
    }

    if (day < 1 || day > 31) {
        throw invalid_argument("Day value is invalid: " + to_string(day));
    }

    this->day_ = day;
    this->month_ = month;
    this->year_ = year;
}

int Date::GetYear() const {
    return year_;
}
int Date::GetMonth() const {
    return month_;
}
int Date::GetDay() const {
    return day_;
}

bool operator<(const Date& lhs, const Date& rhs) {
    auto lhs_key = make_tuple<int, int, int>(lhs.GetYear() , lhs.GetMonth() , lhs.GetDay());
    auto rhs_key = make_tuple<int, int, int>(rhs.GetYear() , rhs.GetMonth() , rhs.GetDay());
    return lhs_key < rhs_key;
}

bool operator>(const Date& lhs, const Date& rhs) {
    return rhs < lhs;
}

bool operator<=(const Date& lhs, const Date& rhs) {
    return !(lhs > rhs);
}

bool operator>=(const Date& lhs, const Date& rhs) {
    return !(lhs < rhs);
}

bool operator==(const Date& lhs, const Date& rhs) {
    auto lhs_key = make_tuple<int, int, int>(lhs.GetYear() , lhs.GetMonth() , lhs.GetDay());
    auto rhs_key = make_tuple<int, int, int>(rhs.GetYear() , rhs.GetMonth() , rhs.GetDay());
    return lhs_key == rhs_key;
}

bool operator!=(const Date& lhs, const Date& rhs) {
    return !(lhs == rhs);
}

ostream& operator<<(ostream& stream, const Date& d) {
    stream << setw(4) << setfill('0') << d.GetYear() << "-";
    stream << setw(2) << setfill('0') << d.GetMonth() << "-";
    stream << setw(2) << setfill('0') << d.GetDay();
    return stream;
}

Date ParseDate(istream& is) {
    string date_str;
    is >> date_str;
    stringstream date_stream(date_str);

    bool ok = true;

    int year = 0;
    ok = ok && (date_stream >> year);
    ok = ok && (date_stream.peek() == '-');
    date_stream.ignore(1);

    int month = 0;
    ok = ok && (date_stream >> month);
    ok = ok && (date_stream.peek() == '-');
    date_stream.ignore(1);

    int day = 0;
    ok = ok && (date_stream >> day);
    ok = ok && date_stream.eof();

    if (!ok) {
        throw invalid_argument("Wrong date format: " + date_str);
    }

    return Date(year, month, day);
}
