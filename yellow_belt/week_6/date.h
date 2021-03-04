#pragma once

#include <string>
#include <istream>

using namespace std;

class Date {
public:
    Date();
    Date(int year_, int month_, int day_);

    int GetYear() const;
    int GetMonth() const;
    int GetDay() const;

private:
    int day_;
    int month_;
    int year_;
};

bool operator<(const Date& lhs, const Date& rhs);
bool operator>(const Date& lhs, const Date& rhs);
bool operator<=(const Date& lhs, const Date& rhs);
bool operator>=(const Date& lhs, const Date& rhs);
bool operator==(const Date& lhs, const Date& rhs);
bool operator!=(const Date& lhs, const Date& rhs);

std::istream& operator>>(std::istream& stream, Date& d);
std::ostream& operator<<(std::ostream& stream, const Date& d);

Date ParseDate(istream& is);

void TestDate();
