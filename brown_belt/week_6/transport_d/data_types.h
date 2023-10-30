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

struct BusData
{
    std::string name;
    StopNames stops;
    bool is_circle_path = true;
};


struct StopData {
    using DistanceToStop = std::pair<std::string, size_t>;

    std::string name;
    Coordinates coords;
    std::vector<DistanceToStop> distances;
};

struct BusStat
{
    std::string bus_name;
    size_t stops_count = 0;
    size_t unique_stops_count = 0;
    size_t path_length = 0;
    double curvature = 0.0;
    std::string error_message;
};

struct StopStat
{
    std::string stop_name;
    SortedBusNames bus_names;
    std::string error_message;
};

struct Path
{
    std::string from;
    std::string to;
    bool operator==(const Path& other) const;
};

struct PathHasher {
    size_t operator() (const Path& a) const;
};