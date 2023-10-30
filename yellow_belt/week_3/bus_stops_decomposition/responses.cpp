#include "responses.h"

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
