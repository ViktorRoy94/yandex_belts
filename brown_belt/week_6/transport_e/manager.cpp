#include "manager.h"
#include "profile.h"

#include <ostream>
#include <algorithm>
#include <set>

using namespace std;
using namespace Graph;
using namespace Geo;
using namespace Response;

void BusManager::AddBus(Bus bus)
{
    for (const auto& name : bus.stops) {
        stop_buses_[name].insert(bus.name);
    }
    buses_[bus.name] = move(bus);
}

const BusManager::Bus& BusManager::GetBus(const std::string& bus_name) const
{
    static Bus empty_bus;
    auto it = buses_.find(bus_name);
    if (it != buses_.end()) {
        return buses_.at(bus_name);
    } else {
        return empty_bus;
    }
}

const BusManager::BusDict &BusManager::GetBuses() const
{
    return buses_;
}

const SortedBusNames& BusManager::GetStopBuses(const std::string& stop_name) const
{
    static SortedBusNames empty;
    auto it = stop_buses_.find(stop_name);
    if (it != stop_buses_.end()) {
        return it->second;
    } else {
        return empty;
    }
}

void StopManager::AddStop(Stop stop)
{
    stops_[stop.name] = move(stop);
}

const StopManager::Stop& StopManager::GetStop(const std::string& stop_name) const
{
    static Stop empty;
    auto it = stops_.find(stop_name);
    if (it != stops_.end()) {
        return it->second;
    } else {
        return empty;
    }
}

const StopManager::StopDict& StopManager::GetStops() const
{
    return stops_;
}

size_t StopManager::GetDistance(const Path& path) const
{
    return Data::ComputeStopsDistance(stops_.at(path.from), stops_.at(path.to));
}

double StopManager::ComputeLength(const vector<string>& stop_names) const
{
    double total_length = 0.0;
    for (size_t i = 0; i < stop_names.size() - 1; ++i) {
        const Coordinates& coord1 = stops_.at(stop_names[i]).position;
        const Coordinates& coord2 = stops_.at(stop_names[i+1]).position;
        total_length += Distance(coord1, coord2);
    }
    return total_length;
}

size_t StopManager::ComputeRealLength(const vector<string>& stop_names) const
{
    double total_length = 0.0;
    for (size_t i = 0; i < stop_names.size() - 1; ++i) {
        Path path = {stop_names[i], stop_names[i+1]};
        total_length += GetDistance(path);
    }
    return total_length;
}

RouteManager::RouteManager(const BusManager &bus_manager,
                           const StopManager &stop_manager,
                           RoutingSettings settings) : settings_(settings)
{
    for (const auto& it : stop_manager.GetStops()) {
        const string& stop_name = it.first;
        vertex_id_to_name_.push_back(stop_name);
        vertex_name_to_id_[stop_name] = vertex_id_to_name_.size() - 1;
    }
    graph_ = make_unique<DirectedWeightedGraph>(vertex_name_to_id_.size());

    for (const auto& [bus_name, bus] : bus_manager.GetBuses()) {
        for (size_t i = 0; i < bus.stops.size() - 1; ++i) {
            EdgeInfo info;
            size_t total_distance = 0;
            for (size_t j = i; j < bus.stops.size() - 1; ++j) {
                total_distance += stop_manager.GetDistance({bus.stops[j], bus.stops[j+1]});
                info.span_count++;
                info.bus = bus_name;

                VertexId from = vertex_name_to_id_[bus.stops[i]];
                VertexId to   = vertex_name_to_id_[bus.stops[j+1]];
                double weight = total_distance * 1.0 / (settings_.bus_velocity * 1000.0 / 60) +
                                settings_.bus_wait_time;
                AddEdge({from, to, weight}, info);
            }
        }
    }
    router_ = make_unique<Router>(*graph_);
}

optional<Statistic::RouteItems> RouteManager::GetRoute(const Path& path) const
{
    if (cached_routes_.count(path) == 0) {
        bool result = BuildRoute(path);
        if (!result) return nullopt;
    }

    Statistic::RouteItems route_items;
    RouteInfo route_info = cached_routes_[path];
    for (size_t i = 0; i < route_info.edge_count; i++) {
        EdgeId edge_id = router_->GetRouteEdge(route_info.id, i);
        const Edge& e = edge_id_to_edge_[edge_id];
        const EdgeInfo& info = edge_info_[edge_id];
        string from = string(vertex_id_to_name_[e.from]);

        auto wait_item = make_unique<Statistic::WaitItem>(from, settings_.bus_wait_time);
        auto bus_item  = make_unique<Statistic::BusItem>(string(info.bus),
                                                         info.span_count,
                                                         e.weight - settings_.bus_wait_time);
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
    return cached_routes_.at(path).weight;
}

void RouteManager::AddEdge(Edge edge, const EdgeInfo& info) {
    auto p = edges_.insert(move(edge));
    if (p.second) {
        graph_->AddEdge(*p.first);
        edge_id_to_edge_.push_back(*p.first);
        edge_info_.push_back(info);
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

void TransportManager::AddBus(Bus bus)
{
    bus_manager_.AddBus(move(bus));
}
void TransportManager::AddStop(Stop stop)
{
    stop_manager_.AddStop(move(stop));
}
void TransportManager::AddRouteSettings(RoutingSettings settings)
{
    settings_= move(settings);
}
ResponsePtr TransportManager::BusInfo(size_t request_id, string bus_name) const
{
    Statistic::Bus stats;
    stats.bus_name = move(bus_name);
    const auto& bus = bus_manager_.GetBus(stats.bus_name);
    if (!bus.name.empty()) {
        double length_by_coords = stop_manager_.ComputeLength(bus.stops);
        size_t length_real = stop_manager_.ComputeRealLength(bus.stops);
        stats.path_length = length_real;
        stats.curvature = length_real / length_by_coords;
        stats.stops_count = bus.stops.size();
        stats.unique_stops_count = set<string>(bus.stops.begin(), bus.stops.end()).size();
    } else {
        stats.error_message = "not found";
    }
    return make_unique<BusInfoResponse>(request_id, move(stats));
}
ResponsePtr TransportManager::StopInfo(size_t request_id, string stop_name) const
{
    Statistic::Stop stats;
    stats.stop_name = move(stop_name);
    const auto& stop = stop_manager_.GetStop(stats.stop_name);
    if (!stop.name.empty()) {
        stats.bus_names = bus_manager_.GetStopBuses(stats.stop_name);
    } else {
        stats.error_message = "not found";
    }
    return make_unique<StopInfoResponse>(request_id, stats);
}
ResponsePtr TransportManager::RouteInfo(size_t request_id, Path path) const
{
    Statistic::Route stats;
    if (!route_manager_) {
        route_manager_ = make_unique<RouteManager>(bus_manager_, stop_manager_, move(settings_));
    }
    auto route_items = route_manager_->GetRoute(path);
    if (route_items.has_value()) {
        stats.items = move(route_items.value());
        stats.total_time = route_manager_->GetRouteTime(path).value();
    } else {
        stats.error_message = "not found";
    }
    return make_unique<RouteInfoResponse>(request_id, move(stats));
}
