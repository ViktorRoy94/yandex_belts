#include "bus_manager.h"
#include "query.h"
#include "responses.h"
#include <iostream>

using namespace std;

int main() {
//#ifdef LOCAL_BUILD
//    std::ifstream in("tests/map_bus_stops_1_test_1.txt");
//    std::cin.rdbuf(in.rdbuf());
//#endif

    int query_count;
    Query q;

    cin >> query_count;

    BusManager bm;
    for (int i = 0; i < query_count; ++i) {
        cin >> q;
        switch (q.type) {
        case QueryType::NewBus:
            bm.AddBus(q.bus, q.stops);
            break;
        case QueryType::BusesForStop:
            cout << bm.GetBusesForStop(q.stop) << endl;
            break;
        case QueryType::StopsForBus:
            cout << bm.GetStopsForBus(q.bus) << endl;
            break;
        case QueryType::AllBuses:
            cout << bm.GetAllBuses() << endl;
            break;
        }
    }

    return 0;
}