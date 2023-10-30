#include "query.h"

istream& operator >> (istream& is, Query& q) {
    std::string op;
    is >> op;
    if (op == "NEW_BUS") {
        q.type = QueryType::NewBus;
        is >> q.bus;
        int stop_count;
        is >> stop_count;
        q.stops.resize(stop_count);
        for (std::string& stop: q.stops) {
            is >> stop;
        }
    } else if (op == "BUSES_FOR_STOP") {
        q.type = QueryType::BusesForStop;
        is >> q.stop;
    } else if (op == "STOPS_FOR_BUS") {
        q.type = QueryType::StopsForBus;
        is >> q.bus;
    } else if (op == "ALL_BUSES") {
        q.type = QueryType::AllBuses;
    }
    return is;
}