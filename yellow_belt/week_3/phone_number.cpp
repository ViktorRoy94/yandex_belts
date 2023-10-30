#include "phone_number.h"
#include <iostream>
#include <sstream>



PhoneNumber::PhoneNumber(const string &international_number) {
    istringstream is(international_number);

    char sign = is.get();
    getline(is, country_code_, '-');
    getline(is, city_code_, '-');
    getline(is, local_number_);

    if (sign != '+' || country_code_.empty() || city_code_.empty() || local_number_.empty()) {
        throw invalid_argument("Phone number must begin with '+' symbol and contain 3 parts separated by '-' symbol: " + international_number);
    }
}

string PhoneNumber::GetCountryCode() const {
    return country_code_;
}
string PhoneNumber::GetCityCode() const {
    return city_code_;
}
string PhoneNumber::GetLocalNumber() const {
    return local_number_;
}
string PhoneNumber::GetInternationalNumber() const {
    ostringstream os;
    os << "+" << country_code_ << "-" << city_code_ << "-" << local_number_;
    return os.str();
}

#ifdef LOCAL_BUILD

#include "test_runner.h"

void AssertPhoneNumber(const PhoneNumber& num, vector<string> correct, const string& message) {
    vector<string> result = {num.GetCountryCode(), num.GetCityCode(), num.GetLocalNumber()};
    AssertEqual(result, correct, message);
}

void TestPhoneNumber() {
    AssertPhoneNumber(PhoneNumber("+7-495-111-22-33"), {"7", "495", "111-22-33"}, "");
    AssertPhoneNumber(PhoneNumber("+7-495-1112233"), {"7", "495", "1112233"}, "");
    AssertPhoneNumber(PhoneNumber("+323-22-460002"), {"323", "22", "460002"}, "");
    AssertPhoneNumber(PhoneNumber("+1-2-coursera-cpp"), {"1", "2", "coursera-cpp"}, "");
}

void TestPhoneNumberWithInvalidArguments() {
    try {
        PhoneNumber("1-2-333");
        throw runtime_error("1-2-333");
    } catch(invalid_argument) {}
    try {
        PhoneNumber("+1");
        throw runtime_error("+1");
    } catch(invalid_argument) {}
    try {
        PhoneNumber("+1-");
        throw runtime_error("+1-");
    } catch(invalid_argument) {}
    try {
        PhoneNumber("+7-1233");
        throw runtime_error("+7-1233");
    } catch(invalid_argument) {}
    try {
        PhoneNumber("+--1233");
        throw runtime_error("+--1233");
    } catch(invalid_argument) {}
}

int main() {
    TestRunner tr;
    tr.RunTest(TestPhoneNumber, "TestPhoneNumber");
    tr.RunTest(TestPhoneNumberWithInvalidArguments, "TestPhoneNumberWithInvalidArguments");
}

#endif
