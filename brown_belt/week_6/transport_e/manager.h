#pragma once

#include <vector>
#include <list>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <optional>

#include "response.h"
#include "data_types.h"
#include "graph.h"
#include "router.h"

class BusManager
{
public:
    void AddBus(BusData bus);
    const std::unordered_map<std::string, StopNames>& GetBuses() const;
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
    const std::list<std::string>& GetStopNames() const;
    double GetDistance(const Path& path) const;
    bool StopExists(const std::string& stop_name) const;
    double ComputeLength(const StopNames& stop_names) const;
    double ComputeRealLength(const StopNames& stop_names) const;
private:
    std::list<std::string> stop_names_;
    std::unordered_map<std::string_view, Coordinates> stops_;
    std::unordered_map<Path, size_t, PathHasher> distances_;
};

class RouteManager
{
public:
    RouteManager(const BusManager& bus_manager,
                 const StopManager& stop_manager);

    bool IsInitialized() const;
    void Initialize();
    void UpdateRouteSettings(RoutingSettings settings);
    std::vector<RouteItemPtr> GetRoute(const Path& path) const;
    double GetRouteTime(const Path& path) const;

private:
    using VertexName = std::pair<std::string, std::string>;

    bool initialized_ = false;
    const BusManager& bus_manager_;
    const StopManager& stop_manager_;

    RoutingSettings settings_;
    std::unordered_map<VertexName, size_t, StringPairHasher> vertex_name_to_id_;
    std::unordered_map<size_t, VertexName> vertex_id_to_name_;

    std::unordered_set<Graph::Edge<double>, Graph::EdgeHasher<double>> edges_;
    std::unordered_map<size_t, Graph::Edge<double>> edge_id_to_edge_;

    std::unique_ptr<Graph::DirectedWeightedGraph<double>> graph_;
    std::unique_ptr<Graph::Router<double>> router_;

    mutable std::unordered_map<Path, Graph::Router<double>::RouteInfo, PathHasher> cached_routes_;

    void AddEdge(Graph::Edge<double> edge);
    void BuildRoute(const Path& path) const;
};

class Server
{
public:
    Server();
    void AddBus(BusData bus);
    void AddStop(StopData stop);
    void AddRouteSettings(RoutingSettings settings);
    ResponsePtr BusInfo(size_t request_id, std::string bus_name) const;
    ResponsePtr StopInfo(size_t request_id, std::string stop_name) const;
    ResponsePtr RouteInfo(size_t request_id, Path path) const;

private:
    RoutingSettings settings_;
    BusManager bus_manager_;
    StopManager stop_manager_;
    mutable RouteManager route_manager_;
};
