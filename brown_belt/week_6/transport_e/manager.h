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
    std::optional<std::vector<RouteItemPtr>> GetRoute(const Path& path) const;
    std::optional<double> GetRouteTime(const Path& path) const;

private:
    struct EdgeWeight {
        double weight = 0.0;
        int span_count = 0;
        std::string_view bus;
        bool operator>(const EdgeWeight& other) const {
            return weight > other.weight;
        }
        bool operator<(const EdgeWeight& other) const {
            return weight < other.weight;
        }
        bool operator==(const EdgeWeight& other) const {
            return weight == other.weight;
        }
        bool operator>=(const EdgeWeight& other) const {
            return weight >= other.weight;
        }
        bool operator>=(int value) const {
            return weight >= value;
        }
        EdgeWeight operator+(const EdgeWeight& other) const {
            return {weight + other.weight, 0, other.bus};
        }
    };

    using Router = Graph::Router<EdgeWeight>;
    using RouteInfo = Router::RouteInfo;
    using Edge = Graph::Edge<EdgeWeight>;
    using EdgeHasher = Graph::EdgeHasher<EdgeWeight>;
    using DirectedWeightedGraph = Graph::DirectedWeightedGraph<EdgeWeight>;

    bool initialized_ = false;
    const BusManager& bus_manager_;
    const StopManager& stop_manager_;

    RoutingSettings settings_;
    std::unordered_map<std::string_view, size_t> vertex_name_to_id_;
    std::vector<std::string_view> vertex_id_to_name_;
    std::unordered_set<Edge, EdgeHasher> edges_;
    std::vector<Edge> edge_id_to_edge_;

    std::unique_ptr<DirectedWeightedGraph> graph_;
    std::unique_ptr<Router> router_;

    mutable std::unordered_map<Path, Graph::Router<EdgeWeight>::RouteInfo, PathHasher> cached_routes_;

    void AddEdge(Edge edge);
    bool BuildRoute(const Path& path) const;
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
