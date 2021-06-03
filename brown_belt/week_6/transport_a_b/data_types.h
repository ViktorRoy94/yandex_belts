#pragma once

#include <ostream>
#include <vector>
#include <set>

const int EARTH_R = 6371000;
const double PI = 3.1415926535;

struct Coordinates
{
    double latitude = 0.0;
    double longitude = 0.0;
};

bool operator==(const Coordinates& lhs, const Coordinates& rhs);
std::ostream& operator<<(std::ostream& out_stream, const Coordinates& coords);
double LengthBetwenCoords(const Coordinates& coord1, const Coordinates& coord2);

using StopNames = std::vector<std::string>;
using BusNames = std::vector<std::string>;
using SortedBusNames = std::set<std::string>;

struct BusStat
{
    std::string bus_name;
    size_t stops_count = 0;
    size_t unique_stops_count = 0;
    double path_length = 0.0;
};

struct StopStat
{
    std::string stop_name;
    bool is_stop_found;
    SortedBusNames bus_names;
};