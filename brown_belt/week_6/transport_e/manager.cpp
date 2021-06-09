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
    VertexId id = 0;
    for (const auto& [bus_name, stop_names] : bus_manager_.GetBuses()) {
        for (const auto& stop_name : stop_names) {
            VertexName vertex_name = {stop_name, bus_name};
            if (vertex_name_to_id_.count(vertex_name) == 0) {
                vertex_name_to_id_[vertex_name] = id;
                vertex_id_to_name_[id] = move(vertex_name);
                id++;
            }

            vertex_name = {stop_name, ""};
            if (vertex_name_to_id_.count(vertex_name) == 0) {
                vertex_name_to_id_[vertex_name] = id;
                vertex_id_to_name_[id] = move(vertex_name);
                id++;
            }
        }
    }
    {
        LOG_DURATION("Graph creation");
        graph_ = make_unique<DirectedWeightedGraph<double>>(vertex_name_to_id_.size());
    }
    for (const auto& [bus_name, stop_names] : bus_manager_.GetBuses()) {
        for (size_t i = 0; i < stop_names.size() - 1; ++i) {
            double d = stop_manager_.GetDistance({stop_names[i], stop_names[i+1]});
            double weight = d / settings_.bus_velocity;
            VertexId from = vertex_name_to_id_[{stop_names[i],   bus_name}];
            VertexId to   = vertex_name_to_id_[{stop_names[i+1], bus_name}];
            if (stop_names[0] == stop_names[i+1]) {
                to = vertex_name_to_id_[{stop_names[i+1], ""}];
            }

            AddEdge({from, to, weight});
        }
    }
    for (const auto& stop_name : stop_manager_.GetStopNames()) {
        const auto& buses_on_this_stop = bus_manager_.GetBusesForStop(stop_name);
        for (auto it = buses_on_this_stop.begin(); it != buses_on_this_stop.end(); it++) {
            VertexId from = vertex_name_to_id_[{stop_name, *it}];
            VertexId to   = vertex_name_to_id_[{stop_name, ""}];
            AddEdge({from, to,   static_cast<double>(settings_.bus_wait_time)});
            AddEdge({to,   from, static_cast<double>(settings_.bus_wait_time)});
            for (auto it2 = buses_on_this_stop.begin(); it2 != buses_on_this_stop.end(); it2++) {
                if (*it == *it2) {
                    continue;
                }
                VertexId from = vertex_name_to_id_[{stop_name, *it}];
                VertexId to   = vertex_name_to_id_[{stop_name, *it2}];
                AddEdge({from, to,   static_cast<double>(settings_.bus_wait_time)});
                AddEdge({to,   from, static_cast<double>(settings_.bus_wait_time)});
            }
        }
    }
    {
        LOG_DURATION("Router creation");
        router_ = make_unique<Router<double>>(*graph_);
    }
    initialized_ = true;
}

void RouteManager::UpdateRouteSettings(RoutingSettings settings)
{
    settings_ = move(settings);
}

std::vector<RouteItemPtr> RouteManager::GetRoute(const Path& path) const
{
    if (cached_routes_.count(path) == 0) {
        BuildRoute(path);
    }

    string last_bus_name;
    size_t span_count = 0;
    double travel_time = 0.0;
    vector<RouteItemPtr> route_items;
    Router<double>::RouteInfo route_info = cached_routes_[path];
    for (size_t i = 0; i < route_info.edge_count; i++) {
        EdgeId edge_id = router_->GetRouteEdge(route_info.id, i);
        const Edge<double>& e = edge_id_to_edge_.at(edge_id);
        VertexName from = vertex_id_to_name_.at(e.from);
        VertexName to   = vertex_id_to_name_.at(e.to);

        if (from.second != to.second && to.second != "") {
            if (span_count > 0) {
                RouteItemPtr item = make_unique<BusItem>(from.second, span_count, travel_time);
                route_items.push_back(move(item));
            }

            RouteItemPtr item = make_unique<WaitItem>(from.first, e.weight);
            route_items.push_back(move(item));
            span_count = 0;
            travel_time = 0;
        } else {
            last_bus_name = from.second;
            span_count++;
            travel_time += e.weight;
        }
    }
    if (span_count > 0) {
        RouteItemPtr item = make_unique<BusItem>(last_bus_name, span_count, travel_time);
        route_items.push_back(move(item));
    }

    return route_items;
}

double RouteManager::GetRouteTime(const Path& path) const
{
    if (cached_routes_.count(path) == 0) {
        BuildRoute(path);
    }
    return cached_routes_.at(path).weight;
}

void RouteManager::AddEdge(Edge<double> edge) {
    auto p = edges_.insert(move(edge));
    if (p.second) {
        EdgeId edge_id = graph_->AddEdge(*p.first);
        edge_id_to_edge_[edge_id] = *p.first;
    }
}

void RouteManager::BuildRoute(const Path& path) const
{
    VertexId from = vertex_name_to_id_.at({path.from, ""});
    VertexId to   = vertex_name_to_id_.at({path.to,   ""});

    auto route = router_->BuildRoute(from, to);
    if (route.has_value()) {
        Router<double>::RouteInfo route_info = *route;
        if (route_info.edge_count > 0) {
            EdgeId id = router_->GetRouteEdge(route_info.id, route_info.edge_count - 1);
            const Edge<double>& e  = edge_id_to_edge_.at(id);
            VertexName from = vertex_id_to_name_.at(e.from);
            VertexName to   = vertex_id_to_name_.at(e.to);
            if (from.first == to.first) {
                route_info.edge_count--;
                route_info.weight -= settings_.bus_wait_time;
            }
        }
        cached_routes_[path] = route_info;
    }
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
    if (route_items.size() != 0) {
        stats.items = move(route_items);
    } else {
        stats.error_message = "not found";
    }
    stats.total_time = route_manager_.GetRouteTime(path);
    return make_unique<RouteInfoResponse>(request_id, move(stats));
}
