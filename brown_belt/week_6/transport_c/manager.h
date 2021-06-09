#pragma once

#include <vector>
#include <string>
#include <unordered_map>
#include <optional>

#include "response.h"
#include "data_types.h"

class BusManager
{
public:
    void AddBus(std::string bus_name, StopNames stops, bool is_circle_path);
    const StopNames& GetBusStops(const std::string& bus_name) const;
    const SortedBusNames& GetBusesForStop(const std::string& stop_name) const;

private:
    std::unordered_map<std::string, StopNames> buses_;
    std::unordered_map<std::string, SortedBusNames> buses_for_stops_;
};

class StopManager
{
public:
    void AddStop(StopData stop);
    bool StopExists(const std::string& stop_name) const;
    double ComputeLength(const StopNames& stop_names) const;
    double ComputeRealLength(const StopNames& stop_names) const;
private:
    std::unordered_map<std::string, Coordinates> stops_;
    std::unordered_map<Path, size_t, PathHasher> distances_;
};

class Server
{
public:
    void AddBus(std::string bus_name, std::vector<std::string> stop_names, bool is_circle_path);
    void AddStop(StopData stop);
    ResponsePtr BusInfo(std::string bus_name) const;
    ResponsePtr StopInfo(std::string stop_name) const;

private:
    BusManager bus_manager_;
    StopManager stop_manager_;
};
