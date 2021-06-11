#pragma once

#include "geo.h"

#include <ostream>
#include <iostream>
#include <vector>
#include <unordered_map>
#include <set>
#include <memory>

using StopNames = std::vector<std::string>;
using SortedBusNames = std::set<std::string>;

namespace Data {
    struct Bus
    {
        std::string name;
        std::vector<std::string> stops;
    };
    using BusDict = std::unordered_map<std::string, Bus>;

    struct Stop {
        std::string name;
        Geo::Coordinates position;
        std::unordered_map<std::string, size_t> distances;
    };
    using StopDict = std::unordered_map<std::string, Stop>;

    size_t ComputeStopsDistance(const Stop& lhs, const Stop& rhs);
}

namespace Statistic {
    struct Bus
    {
        std::string bus_name;
        size_t stops_count = 0;
        size_t unique_stops_count = 0;
        size_t path_length = 0;
        double curvature = 0.0;
        std::string error_message;
    };

    struct Stop
    {
        std::string stop_name;
        SortedBusNames bus_names;
        std::string error_message;
    };

    struct RouteItem;
    using RouteItemPtr = std::unique_ptr<RouteItem>;
    using RouteItems = std::vector<RouteItemPtr>;

    struct Route
    {
        double total_time = 0.0;
        std::vector<RouteItemPtr> items;
        std::string error_message;
    };

    struct RouteItem {
        virtual void PrintJson(std::ostream& out = std::cout) = 0;
        virtual ~RouteItem() = default;
    };

    struct WaitItem : RouteItem {
        WaitItem(std::string stop_name, double time);
        virtual void PrintJson(std::ostream& out = std::cout) override;

        std::string stop_name;
        double time = 0.0;
    };
    bool operator==(const WaitItem& lhs, const WaitItem& rhs);
    std::ostream& operator<<(std::ostream& out_stream, const WaitItem& item);

    struct BusItem : RouteItem {
        BusItem(std::string bus_name, size_t span_count, double time);
        virtual void PrintJson(std::ostream& out = std::cout) override;

        std::string bus_name;
        size_t span_count = 0;
        double time = 0.0;
    };
    bool operator==(const BusItem& lhs, const BusItem& rhs);
    std::ostream& operator<<(std::ostream& out_stream, const BusItem& item);
}

struct Path
{
    std::string from;
    std::string to;
    bool operator==(const Path& other) const;
};

struct PathHasher {
    size_t operator() (const Path& a) const;
};

struct RoutingSettings {
//    RoutingSettings() {}
//    RoutingSettings(int wait_time, double velocity)
//        : bus_wait_time(wait_time)
//        , bus_velocity(velocity * 50.0 / 3)
//    {}
    int bus_wait_time = 0;
    double bus_velocity = 0;
};