#include <ostream>
#include <algorithm>
#include <set>

#include "manager.h"
#include "profile.h"

using namespace std;
using namespace Graph;

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

const std::unordered_map<std::string, StopNames>& BusManager::GetBuses() const
{
    return buses_;
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
    stop_names_.push_back(move(stop.name));
    stops_[stop_names_.back()] = move(stop.coords);
}

const list<string>& StopManager::GetStopNames() const
{
    return stop_names_;
}

double StopManager::GetDistance(const Path& path) const
{
    return distances_.at(path);
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

RouteManager::RouteManager(const BusManager& bus_manager, const StopManager& stop_manager)
    : bus_manager_(bus_manager)
    , stop_manager_(stop_manager)
{
}

bool RouteManager::IsInitialized() const
{
    return initialized_;
}

void RouteManager::Initialize()
{
    for (const auto& stop_name : stop_manager_.GetStopNames()) {
        vertex_id_to_name_.push_back(stop_name);
        vertex_name_to_id_[stop_name] = vertex_id_to_name_.size() - 1;
    }
    graph_ = make_unique<DirectedWeightedGraph>(vertex_name_to_id_.size());

    for (const auto& [bus_name, stop_names] : bus_manager_.GetBuses()) {
        for (size_t i = 0; i < stop_names.size() - 1; ++i) {
            EdgeWeight weight = {static_cast<double>(settings_.bus_wait_time), 0};
            for (size_t j = i; j < stop_names.size() - 1; ++j) {
                double d = stop_manager_.GetDistance({stop_names[j], stop_names[j+1]});
                weight.weight += d / settings_.bus_velocity;
                weight.span_count++;
                weight.bus = bus_name;

                VertexId from = vertex_name_to_id_[stop_names[i]];
                VertexId to   = vertex_name_to_id_[stop_names[j+1]];
                AddEdge({from, to, weight});
            }
        }
    }
    router_ = make_unique<Router>(*graph_);
    initialized_ = true;
}

void RouteManager::UpdateRouteSettings(RoutingSettings settings)
{
    settings_ = move(settings);
}

optional<vector<RouteItemPtr> > RouteManager::GetRoute(const Path& path) const
{
    if (cached_routes_.count(path) == 0) {
        bool result = BuildRoute(path);
        if (!result) return nullopt;
    }

    vector<RouteItemPtr> route_items;
    RouteInfo route_info = cached_routes_[path];
    for (size_t i = 0; i < route_info.edge_count; i++) {
        EdgeId edge_id = router_->GetRouteEdge(route_info.id, i);
        const Edge& e = edge_id_to_edge_[edge_id];
        string from = string(vertex_id_to_name_[e.from]);

        RouteItemPtr wait_item = make_unique<WaitItem>(from, settings_.bus_wait_time);
        RouteItemPtr bus_item  = make_unique<BusItem>(string(e.weight.bus),
                                                      e.weight.span_count,
                                                      e.weight.weight - settings_.bus_wait_time);
        route_items.push_back(move(wait_item));
        route_items.push_back(move(bus_item));
    }
    return move(route_items);
}

optional<double> RouteManager::GetRouteTime(const Path& path) const
{
    if (cached_routes_.count(path) == 0) {
        return nullopt;
    }
    return cached_routes_.at(path).weight.weight;
}

void RouteManager::AddEdge(Edge edge) {
    auto p = edges_.insert(move(edge));
    if (p.second) {
        graph_->AddEdge(*p.first);
        edge_id_to_edge_.push_back(*p.first);
    }
}

bool RouteManager::BuildRoute(const Path& path) const
{
    VertexId from = vertex_name_to_id_.at(path.from);
    VertexId to   = vertex_name_to_id_.at(path.to);

    auto route = router_->BuildRoute(from, to);
    if (route.has_value()) {
        RouteInfo route_info = *route;
        cached_routes_[path] = route_info;
    }
    return route.has_value();
}

Server::Server() : route_manager_(bus_manager_, stop_manager_)
{}

void Server::AddBus(BusData bus)
{
    bus_manager_.AddBus(move(bus));
}
void Server::AddStop(StopData stop)
{
    stop_manager_.AddStop(move(stop));
}
void Server::AddRouteSettings(RoutingSettings settings)
{
    route_manager_.UpdateRouteSettings(move(settings));
}
ResponsePtr Server::BusInfo(size_t request_id, string bus_name) const
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

ResponsePtr Server::StopInfo(size_t request_id, string stop_name) const
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

ResponsePtr Server::RouteInfo(size_t request_id, Path path) const
{
    RouteStat stats;
    if (!route_manager_.IsInitialized()) {
        route_manager_.Initialize();
    }
    auto route_items = route_manager_.GetRoute(path);
    if (route_items.has_value()) {
        stats.items = move(route_items.value());
        stats.total_time = route_manager_.GetRouteTime(path).value();
    } else {
        stats.error_message = "not found";
    }
    return make_unique<RouteInfoResponse>(request_id, move(stats));
}
