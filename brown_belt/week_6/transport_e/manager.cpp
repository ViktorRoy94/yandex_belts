#include <ostream>
#include <algorithm>
#include <set>

#include "manager.h"

using namespace std;

void BusManager::AddBus(BusData bus)
{
    if (!bus.is_circle_path) {
        for (int i = static_cast<int>(bus.stops.size()) - 2; i >= 0; --i) {
            bus.stops.push_back(bus.stops[i]);
        }
    }
    for (const auto& name : bus.stops) {
        buses_for_stops_[name].insert(bus.name);
    }

    buses_[move(bus.name)] = move(bus.stops);
}

const StopNames& BusManager::GetBusStops(const string& bus_name) const
{
    static const StopNames empty;
    auto it = buses_.find(bus_name);
    if (it != buses_.end()) {
        return it->second;
    } else {
        return empty;
    }
}

const SortedBusNames& BusManager::GetBusesForStop(const std::string& stop_name) const
{
    static const SortedBusNames empty;
    auto it = buses_for_stops_.find(stop_name);
    if (it != buses_for_stops_.end()) {
        return it->second;
    } else {
        return empty;
    }
}

void StopManager::AddStop(StopData stop)
{
    for (auto& [another_stop, distance] : stop.distances) {
        Path path = {stop.name, another_stop};
        Path reverse_path = {another_stop, stop.name};
        distances_[path] = distance;
        if (distances_.count(reverse_path) == 0) {
            distances_[reverse_path] = distance;
        }
    }
    stops_[move(stop.name)] = move(stop.coords);
}

bool StopManager::StopExists(const std::string& stop_name) const
{
    return stops_.count(stop_name) > 0;
}

double StopManager::ComputeLength(const StopNames &stop_names) const
{
    double total_length = 0.0;
    for (size_t i = 0; i < stop_names.size() - 1; ++i) {
        const Coordinates& coord1 = stops_.at(stop_names[i]);
        const Coordinates& coord2 = stops_.at(stop_names[i+1]);
        total_length += LengthBetwenCoords(coord1, coord2);
    }
    return total_length;
}

double StopManager::ComputeRealLength(const StopNames& stop_names) const
{
    double total_length = 0.0;
    for (size_t i = 0; i < stop_names.size() - 1; ++i) {
        Path path = {stop_names[i], stop_names[i+1]};
        total_length += distances_.at(path);
    }
    return total_length;
}

TransportManager::TransportManager(RoutingSettings settings) : settings_(settings)
{
}

void TransportManager::AddBus(BusData bus)
{
    bus_manager_.AddBus(move(bus));
}
void TransportManager::AddStop(StopData stop)
{
    stop_manager_.AddStop(move(stop));
}
ResponsePtr TransportManager::BusInfo(size_t request_id, string bus_name) const
{
    BusStat stats;
    stats.bus_name = move(bus_name);
    const StopNames& names = bus_manager_.GetBusStops(stats.bus_name);
    if (names.size() != 0) {
        double length_by_coords = stop_manager_.ComputeLength(names);
        size_t length_real = stop_manager_.ComputeRealLength(names);
        stats.path_length = length_real;
        stats.curvature = length_real / length_by_coords;
        stats.stops_count = names.size();
        stats.unique_stops_count = set<string>(names.begin(), names.end()).size();
    } else {
        stats.error_message = "not found";
    }
    return make_unique<BusInfoResponse>(request_id, move(stats));
}

ResponsePtr TransportManager::StopInfo(size_t request_id, string stop_name) const
{
    StopStat stats;
    stats.stop_name = move(stop_name);
    if (!stop_manager_.StopExists(stats.stop_name)) {
        stats.error_message = "not found";
    }
    const SortedBusNames& buses = bus_manager_.GetBusesForStop(stats.stop_name);
    if (buses.size() != 0) {
        stats.bus_names = buses;
    }
    return make_unique<StopInfoResponse>(request_id, stats);
}

ResponsePtr TransportManager::RouteInfo(size_t request_id, Path path) const
{
    RouteStat stats;
    return make_unique<RouteInfoResponse>(request_id, move(stats));
}
