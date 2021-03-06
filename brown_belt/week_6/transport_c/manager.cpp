#include <ostream>
#include <algorithm>
#include <set>

#include "manager.h"

using namespace std;

void BusManager::AddBus(string bus_name, StopNames stops, bool is_circle_path)
{
    if (!is_circle_path) {
        for (int i = static_cast<int>(stops.size()) - 2; i >= 0; --i) {
            stops.push_back(stops[i]);
        }
    }
    for (const auto& name : stops) {
        buses_for_stops_[name].insert(bus_name);
    }

    buses_[move(bus_name)] = move(stops);
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
        Path path = {stop.stop_name, another_stop};
        Path reverse_path = {another_stop, stop.stop_name};
        distances_[path] = distance;
        if (distances_.count(reverse_path) == 0) {
            distances_[reverse_path] = distance;
        }
    }
    stops_[move(stop.stop_name)] = move(stop.coords);
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

void TransportManager::AddBus(string bus_name, vector<string> stop_names, bool is_circle_path)
{
    bus_manager_.AddBus(move(bus_name), move(stop_names), is_circle_path);
}
void TransportManager::AddStop(StopData stop)
{
    stop_manager_.AddStop(move(stop));
}
ResponsePtr TransportManager::BusInfo(string bus_name) const
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
    }
    return make_unique<BusInfoResponse>(move(stats));
}

ResponsePtr TransportManager::StopInfo(string stop_name) const
{
    StopStat stats;
    stats.stop_name = move(stop_name);
    stats.is_stop_found = stop_manager_.StopExists(stats.stop_name);
    const SortedBusNames& buses = bus_manager_.GetBusesForStop(stats.stop_name);
    if (buses.size() != 0) {
        stats.bus_names = buses;
    }
    return make_unique<StopInfoResponse>(stats);
}
