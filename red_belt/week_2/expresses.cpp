#include <algorithm>
#include <cmath>
#include <iostream>
#include <map>
#include <string>
#include <vector>

#include "test_runner.h"

using namespace std;

class RouteManager {
public:
    void AddRoute(int start, int finish) {
        reachable_lists_[start].insert(finish);
        reachable_lists_[finish].insert(start);
    }
    int FindNearestFinish(int start, int finish) const {
        int result = abs(start - finish);
        if (reachable_lists_.count(start) < 1) {
            return result;
        }
        const set<int>& reachable_stations = reachable_lists_.at(start);
        auto it = reachable_stations.lower_bound(finish);
        if (it != reachable_stations.end()) {
            result = min(result, abs(finish - *it));
        }
        if (it != reachable_stations.begin()) {
            result = min(result, abs(finish - *prev(it)));
        }
        return result;
    }
private:
    map<int, set<int>> reachable_lists_;
};

void TestRouteManager() {
    RouteManager routes;
    routes.AddRoute(-2, 5);
    routes.AddRoute(10, 4);
    routes.AddRoute(5, 8);

    ostringstream os;
    os << routes.FindNearestFinish(4, 10) << "\n";
    os << routes.FindNearestFinish(4, -2) << "\n";
    os << routes.FindNearestFinish(5, 0) << "\n";
    os << routes.FindNearestFinish(5, 100) << "\n";

    ASSERT_EQUAL(os.str(), "0\n6\n2\n92\n");
}


int main() {
#ifdef LOCAL_BUILD
    TestRunner tr;
    RUN_TEST(tr, TestRouteManager);
#else
    RouteManager routes;

    int query_count;
    cin >> query_count;

    for (int query_id = 0; query_id < query_count; ++query_id) {
        string query_type;
        cin >> query_type;
        int start, finish;
        cin >> start >> finish;
        if (query_type == "ADD") {
            routes.AddRoute(start, finish); // O(logQ)
        } else if (query_type == "GO") {
            cout << routes.FindNearestFinish(start, finish) << "\n"; // O(Q)
        }
    }

#endif
    return 0;
}
