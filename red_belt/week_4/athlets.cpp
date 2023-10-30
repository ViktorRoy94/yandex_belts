#include <iostream>
#include <list>
#include <set>
#include <algorithm>

#include "test_runner.h"

using namespace std;

const int MAX_ATHLETES = 100000;
using Position = list<int>::iterator;

void AddAthlet(list<int>& athlets_order,
               vector<Position>& athlets_on_field,
               int athlet,
               int next_athlet)
{
    athlets_on_field[athlet] = athlets_order.insert(athlets_on_field[next_athlet], athlet);
}

void TestAthlets() {
    list<int> athlets_order;
    vector<Position> athlets_on_field(MAX_ATHLETES + 1, athlets_order.end());

    AddAthlet(athlets_order, athlets_on_field, 42, 0);
    AddAthlet(athlets_order, athlets_on_field, 17, 42);
    AddAthlet(athlets_order, athlets_on_field, 13, 0);
    AddAthlet(athlets_order, athlets_on_field, 123, 42);
    AddAthlet(athlets_order, athlets_on_field, 5, 13);

    AssertEqual(athlets_order.front(), 17);
    athlets_order.pop_front();
    AssertEqual(athlets_order.front(), 123);
    athlets_order.pop_front();
    AssertEqual(athlets_order.front(), 42);
    athlets_order.pop_front();
    AssertEqual(athlets_order.front(), 5);
    athlets_order.pop_front();
    AssertEqual(athlets_order.front(), 13);
    athlets_order.pop_front();
}

int main() {
#ifdef LOCAL_BUILD
    TestRunner tr;
    RUN_TEST(tr, TestAthlets);
#else
    int n = 0;
    cin >> n;

    list<int> athlets_order;
    vector<Position> athlets_on_field(MAX_ATHLETES + 1, athlets_order.end());
    for (int i = 0; i < n; i++) {
        int athlet_num, position_num;
        cin >> athlet_num >> position_num;
        AddAthlet(athlets_order, athlets_on_field, athlet_num, position_num);
    }
    for (const auto& num : athlets_order) {
        cout << num << "\n";
    }

#endif
}