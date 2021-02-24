#include <iostream>
#include <string>
#include <vector>
#include <deque>
#include <algorithm>
#include <numeric>
#include <iterator>
#include <sstream>

#ifdef LOCAL_BUILD
#include "test_runner.h"
#endif

using namespace std;

struct Operation {
    char type = ' ';
    int number = 0;
};

string BuildExpression(int start_num, vector<Operation> operations) {
    deque<string> deq = {to_string(start_num)};
    for (const auto& op : operations) {
        deq.push_front("(");
        deq.push_back(")");
        deq.push_back(" ");
        deq.push_back(string(1, op.type));
        deq.push_back(" ");
        deq.push_back(to_string(op.number));
    }
    std::ostringstream os;
    std::copy(deq.begin(), deq.end(), ostream_iterator<string>( os ) );
    return os.str();
}

#ifdef LOCAL_BUILD
void TestBuildExpression() {
    AssertEqual(BuildExpression(8, {}), "8");
    AssertEqual(BuildExpression(8, {{'*', 3}}), "(8) * 3");
    AssertEqual(BuildExpression(8, {{'*', 3}, {'-', 6}}), "((8) * 3) - 6");
    AssertEqual(BuildExpression(8, {{'*', 3}, {'-', 6}, {'/', 1}}), "(((8) * 3) - 6) / 1");
}
#endif

int main() {
#ifdef LOCAL_BUILD
    TestRunner runner;
    runner.RunTest(TestBuildExpression, "TestBuildExpression");
#else
    int start_num;
    int n_op;
    cin >> start_num;
    cin >> n_op;
    cin.ignore(256, '\n');

    vector<Operation> operations(n_op);
    for (auto& op : operations) {
        cin >> op.type;
        cin >> op.number;
    }
    cout << BuildExpression(start_num, operations) << endl;

#endif
    return 0;
}
