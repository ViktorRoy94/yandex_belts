#include <string>
#include <iostream>
#include <fstream>
#include <cassert>
#include <vector>
#include <map>

using namespace std;

enum class QueryType {
    NewBus,
    BusesForStop,
    StopsForBus,
    AllBuses
};

struct Query {
    QueryType type;
    string bus;
    string stop;
    vector<string> stops;
};

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

struct BusesForStopResponse {
    vector<string> buses;
};

ostream& operator << (ostream& os, const BusesForStopResponse& r) {
    if (r.buses.size() == 0) {
        os << "No stop";
    } else {
        for (const std::string& bus : r.buses) {
            os << bus << " ";
        }
    }
    return os;
}

struct StopsForBusResponse {
    string bus;
    vector<pair<string, vector<string>>> stops_for_buses;
};

ostream& operator << (ostream& os, const StopsForBusResponse& r) {
    if (r.stops_for_buses.size() == 0) {
        os << "No bus";
    }
    bool first = true;
    for (const auto& [stop, buses] : r.stops_for_buses) {
        if (!first) {
            os << endl;
        }
        first = false;

        os << "Stop " << stop << ": ";
        if (buses.size() == 1) {
            os << "no interchange";
        } else {
            for (const std::string& bus : buses) {
                if (bus != r.bus)
                    os << bus << " ";
            }
        }
    }
    return os;
}

struct AllBusesResponse {
    map<string, vector<string>> buses_to_stop;
};

ostream& operator << (ostream& os, const AllBusesResponse& r) {
    if (r.buses_to_stop.size() == 0) {
        os << "No buses";
    }
    bool first = true;
    for (const auto& [bus, stops] : r.buses_to_stop) {
        if (!first) {
            os << endl;
        }
        first = false;

        os << "Bus " << bus << ": ";
        for (const std::string& stop_name : stops) {
            os << stop_name << " ";
        }
    }
    return os;
}

class BusManager {
public:
    void AddBus(const string& bus, const vector<string>& stops) {
        buses_to_stop_.insert(make_pair(bus, stops));
        for (const std::string& stop: stops) {
            stops_to_buses_[stop].push_back(bus);
        }
    }

    BusesForStopResponse GetBusesForStop(const string& stop) const {
        if (stops_to_buses_.count(stop) == 0) {
            return BusesForStopResponse{vector<string>()};
        } else {
            return BusesForStopResponse{stops_to_buses_.at(stop)};
        }
    }

    StopsForBusResponse GetStopsForBus(const string& bus) const {
        vector<pair<string, vector<string>>> result;
        if (buses_to_stop_.count(bus) > 0) {
            for (const auto& stop : buses_to_stop_.at(bus)) {
                result.push_back(make_pair(stop, stops_to_buses_.at(stop)));
            }
        }
        return StopsForBusResponse{bus, result};
    }

    AllBusesResponse GetAllBuses() const {
        return AllBusesResponse({buses_to_stop_});
    }

private:
    map<string, vector<string>> buses_to_stop_;
    map<string, vector<string>> stops_to_buses_;
};

int main() {
#ifdef LOCAL_BUILD
    std::ifstream in("tests/map_bus_stops_1_test_1.txt");
    std::cin.rdbuf(in.rdbuf());
#endif

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
