#include "bus_manager.h"

void BusManager::AddBus(const string& bus, const vector<string>& stops) {
    buses_to_stop_.insert(make_pair(bus, stops));
    for (const std::string& stop: stops) {
        stops_to_buses_[stop].push_back(bus);
    }
}

BusesForStopResponse BusManager::GetBusesForStop(const string& stop) const {
    if (stops_to_buses_.count(stop) == 0) {
        return BusesForStopResponse{vector<string>()};
    } else {
        return BusesForStopResponse{stops_to_buses_.at(stop)};
    }
}

StopsForBusResponse BusManager::GetStopsForBus(const string& bus) const {
    vector<pair<string, vector<string>>> result;
    if (buses_to_stop_.count(bus) > 0) {
        for (const auto& stop : buses_to_stop_.at(bus)) {
            result.push_back(make_pair(stop, stops_to_buses_.at(stop)));
        }
    }
    return StopsForBusResponse{bus, result};
}

AllBusesResponse BusManager::GetAllBuses() const {
    return AllBusesResponse({buses_to_stop_});
}
