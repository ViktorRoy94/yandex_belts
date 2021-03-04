#include "date.h"
#include "test_runner.h"

void AssertEqualDate(const Date& d, tuple<int, int, int> answer, const string& message) {
    AssertEqual(d.GetYear(), get<0>(answer), message + " year");
    AssertEqual(d.GetMonth(), get<1>(answer), message + " month");
    AssertEqual(d.GetDay(), get<2>(answer), message + " day");
}

void AssertDateThrowException(int year, int month, int day, const string e_message, const string& hint = {}) {
    try {
        Date(year, month, day);
    } catch(const invalid_argument& e) {
        AssertEqual(string(e.what()), e_message, hint);
    }
}

void AssertParseDateThrowException(istream& is, const string e_message, const string& hint = {}) {
    try {
        ParseDate(is);
    } catch(const exception& e) {
        AssertEqual(string(e.what()), e_message, hint);
    }
}

void TestDate() {
    {
        AssertEqualDate(Date(), {0, 0, 0}, "Date default constructor");
        AssertEqualDate(Date(2017, 1, 12), {2017, 1, 12}, "Date constructor");
        AssertDateThrowException(-1, 1, 1, "Year value is invalid: -1", "Date constructor wrong year value 1");
        AssertDateThrowException(10000, 1, 1, "Year value is invalid: 10000", "Date constructor wrong year value 2");
        AssertDateThrowException(10000000, 1, 1, "Year value is invalid: 10000000", "Date constructor wrong year value 3");
        AssertDateThrowException(-10000000, 1, 1, "Year value is invalid: -10000000", "Date constructor wrong year value 4");
        AssertDateThrowException(0, 0, 1, "Month value is invalid: 0", "Date constructor wrong month value 1");
        AssertDateThrowException(0, 13, 1, "Month value is invalid: 13", "Date constructor wrong month value 2");
        AssertDateThrowException(0, -10000, 1, "Month value is invalid: -10000", "Date constructor wrong month value 3");
        AssertDateThrowException(0, 10000, 1, "Month value is invalid: 10000", "Date constructor wrong month value 4");
    }

    {
        istringstream is("2017-11-18");
        AssertEqual(ParseDate(is), Date(2017, 11, 18), "ParseDate condition 1");
    }
    {
        istringstream is("1994-1-31");
        AssertEqual(ParseDate(is), Date(1994, 1, 31), "ParseDate condition 2");
    }
    {
        istringstream is("0-2-31");
        AssertEqual(ParseDate(is), Date(0, 2, 31), "ParseDate condition 3");
    }
    {
        istringstream is("-11-18");
        AssertParseDateThrowException(is, "Wrong date format: -11-18", "ParseDate wrong format 1");
    }
    {
        istringstream is("2017-11-");
        AssertParseDateThrowException(is, "Wrong date format: 2017-11-", "ParseDate wrong format 2");
    }
    {
        istringstream is("-2017-11-1");
        AssertParseDateThrowException(is, "Year value is invalid: -2017", "ParseDate wrong format 2");
    }

    {
        Assert(  Date(2000, 1, 1) < Date(2001, 1, 1),  "Date operator< 1");
        Assert(  Date(2000, 1, 1) < Date(2000, 2, 1),  "Date operator< 2");
        Assert(  Date(2000, 1, 1) < Date(2000, 1, 2),  "Date operator< 3");
        Assert(!(Date(2000, 1, 1) < Date(2000, 1, 1)), "Date operator< 4");

        Assert(  Date(2001, 1, 1) > Date(2000, 1, 1),  "Date operator> 1");
        Assert(  Date(2000, 2, 1) > Date(2000, 1, 1),  "Date operator> 2");
        Assert(  Date(2000, 1, 2) > Date(2000, 1, 1),  "Date operator> 3");
        Assert(!(Date(2000, 1, 1) > Date(2000, 1, 1)), "Date operator> 4");

        Assert(Date(2000, 1, 1) <= Date(2001, 1, 1), "Date operator<= 1");
        Assert(Date(2000, 1, 1) <= Date(2000, 2, 1), "Date operator<= 2");
        Assert(Date(2000, 1, 1) <= Date(2000, 1, 2), "Date operator<= 3");
        Assert(Date(2000, 1, 1) <= Date(2000, 1, 1), "Date operator<= 4");

        Assert(Date(2001, 1, 1) >= Date(2000, 1, 1), "Date operator>= 1");
        Assert(Date(2000, 2, 1) >= Date(2000, 1, 1), "Date operator>= 2");
        Assert(Date(2000, 1, 2) >= Date(2000, 1, 1), "Date operator>= 3");
        Assert(Date(2000, 1, 1) >= Date(2000, 1, 1), "Date operator>= 4");
    }
}
