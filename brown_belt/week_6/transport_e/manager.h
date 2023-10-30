#pragma once

#include "response.h"
#include "data_types.h"
#include "graph.h"
#include "router.h"
#include "geo.h"

#include <vector>
#include <list>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <optional>

class BusManager
{
private:
    using Bus = Data::Bus;
    using BusDict = Data::BusDict;
public:
    void AddBus(Bus bus);
    const Bus& GetBus(const std::string& bus_name) const;
    const BusDict& GetBuses() const;
    const SortedBusNames& GetStopBuses(const std::string& stop_name) const;

private:
    BusDict buses_;
    std::unordered_map<std::string, SortedBusNames> stop_buses_;
};

class StopManager
{
private:
    using Stop = Data::Stop;
    using StopDict = Data::StopDict;
public:
    void AddStop(Stop stop);
    const Stop& GetStop(const std::string& stop_name) const;
    const StopDict& GetStops() const;
    size_t GetDistance(const Path& path) const;
    double ComputeLength(const std::vector<std::string>& stop_names) const;
    size_t ComputeRealLength(const std::vector<std::string>& stop_names) const;
private:
    StopDict stops_;
};

class RouteManager
{
public:
    RouteManager(const BusManager& bus_manager,
                 const StopManager& stop_manager,
                 RoutingSettings settings);

    void UpdateRouteSettings(RoutingSettings settings);
    std::optional<Statistic::RouteItems> GetRoute(const Path& path) const;
    std::optional<double> GetRouteTime(const Path& path) const;

private:
    struct EdgeInfo {
        int span_count = 0;
        std::string_view bus;
    };

    using Router = Graph::Router<double>;
    using RouteInfo = Router::RouteInfo;
    using Edge = Graph::Edge<double>;
    using EdgeHasher = Graph::EdgeHasher<double>;
    using DirectedWeightedGraph = Graph::DirectedWeightedGraph<double>;

    RoutingSettings settings_;
    std::unordered_map<std::string_view, size_t> vertex_name_to_id_;
    std::vector<std::string_view> vertex_id_to_name_;
    std::unordered_set<Edge, EdgeHasher> edges_;
    std::vector<Edge> edge_id_to_edge_;
    std::vector<EdgeInfo> edge_info_;

    std::unique_ptr<DirectedWeightedGraph> graph_;
    std::unique_ptr<Router> router_;

    mutable std::unordered_map<Path, RouteInfo, PathHasher> cached_routes_;

    void AddEdge(Edge edge, const EdgeInfo& info);
    bool BuildRoute(const Path& path) const;
};

class TransportManager
{
private:
    using Bus = Data::Bus;
    using Stop = Data::Stop;
public:
    void AddBus(Bus bus);
    void AddStop(Stop stop);
    void AddRouteSettings(RoutingSettings settings);
    Response::ResponsePtr BusInfo(size_t request_id, std::string bus_name) const;
    Response::ResponsePtr StopInfo(size_t request_id, std::string stop_name) const;
    Response::ResponsePtr RouteInfo(size_t request_id, Path path) const;

private:
    RoutingSettings settings_;
    BusManager bus_manager_;
    StopManager stop_manager_;
    mutable std::unique_ptr<RouteManager> route_manager_;
};
