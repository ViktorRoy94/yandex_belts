#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

#include "test_runner.h"

using namespace std;

#ifdef LOCAL_BUILD
#include "print_stats_B.cpp"
#endif

void TestBug1() {
    vector<Person> persons;
    int result = ComputeMedianAge(persons.begin(), persons.end());
    ASSERT_EQUAL(result, 0);
}

void TestBug2() {
    vector<Person> persons = {{10, Gender::FEMALE, true},
                              {11, Gender::FEMALE, true},
                              {12, Gender::FEMALE, true}};
    int result = ComputeMedianAge(persons.begin(), persons.end());
    ASSERT_EQUAL(result, 11);
}

void TestBug3() {
    istringstream input("1\n10 1 0");
    vector<Person> result = ReadPersons(input);
    ASSERT_EQUAL(result.size(), 1u);
    ASSERT_EQUAL(result[0].age, 10);
    ASSERT_EQUAL(static_cast<int>(result[0].gender), static_cast<int>(Gender::MALE));
    ASSERT_EQUAL(result[0].is_employed, false);
}

void TestBug4() {
    istringstream input("1\n10 1 0");
    vector<Person> result = ReadPersons(input);
    ASSERT_EQUAL(result.size(), 1u);
    ASSERT_EQUAL(result[0].age, 10);
    ASSERT_EQUAL(static_cast<int>(result[0].gender), static_cast<int>(Gender::MALE));
    ASSERT_EQUAL(result[0].is_employed, false);
}
    //                 persons
    //                |       |
    //          females        males
    //         |       |      |     |
    //      empl.  unempl. empl.   unempl.
void TestBug5() {
    vector<Person> persons = {{10, Gender::FEMALE, true},
                              {11, Gender::FEMALE, true},
                              {12, Gender::FEMALE, false},
                              {15, Gender::MALE, true},
                              {16, Gender::MALE, true},
                              {17, Gender::MALE, false}};
    AgeStats result = ComputeStats(persons);
    ASSERT_EQUAL(result.total, 15);
    ASSERT_EQUAL(result.females, 11);
    ASSERT_EQUAL(result.males, 16);
    ASSERT_EQUAL(result.employed_females, 11);
    ASSERT_EQUAL(result.unemployed_females, 12);
    ASSERT_EQUAL(result.employed_males, 16);
    ASSERT_EQUAL(result.unemployed_males, 17);
}

void TestBug6() {
    ostringstream output;
    AgeStats stats = {15, 11, 16, 11, 12, 16, 17};
    PrintStats(stats, output);
    ASSERT_EQUAL(output.str(), "Median age = 15\n"
                               "Median age for females = 11\n"
                               "Median age for males = 16\n"
                               "Median age for employed females = 11\n"
                               "Median age for unemployed females = 12\n"
                               "Median age for employed males = 16\n"
                               "Median age for unemployed males = 17\n")
}

int main() {
    TestRunner tr;
    RUN_TEST(tr, TestBug1);
    RUN_TEST(tr, TestBug2);
    RUN_TEST(tr, TestBug3);
    RUN_TEST(tr, TestBug4);
    RUN_TEST(tr, TestBug5);
    RUN_TEST(tr, TestBug6);
    return 0;
}