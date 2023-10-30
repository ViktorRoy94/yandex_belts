#include "request.h"

using namespace std;
using namespace Json;
using namespace Response;

namespace Request {

    RequestPtr Request::Create(Type type)
    {
        switch (type) {
        case Type::ADD_ROUTE_SETTINGS:
            return make_unique<AddRouteSettingsRequest>();
        case Type::ADD_BUS:
            return make_unique<AddBusRequest>();
        case Type::ADD_STOP:
            return make_unique<AddStopRequest>();
        case Type::BUS_INFO:
            return make_unique<BusInfoRequest>();
        case Type::STOP_INFO:
            return make_unique<StopInfoRequest>();
        case Type::ROUTE_INFO:
            return make_unique<RouteInfoRequest>();
        default:
            return nullptr;
        }
    }

    void AddBusRequest::ParseFrom(const Node& node)
    {
        auto& dict = node.AsMap();
        bus.name = dict.at("name").AsString();
        for (const auto& stop_name_node : dict.at("stops").AsArray()) {
            bus.stops.push_back(stop_name_node.AsString());
        }
        if (!dict.at("is_roundtrip").AsBool()) {
            for (int i = static_cast<int>(bus.stops.size()) - 2; i >= 0; --i) {
                bus.stops.push_back(bus.stops[i]);
            }
        }
    }
    void AddBusRequest::Process(TransportManager& manager) const
    {
        manager.AddBus(move(bus));
    }

    void AddStopRequest::ParseFrom(const Node& node)
    {
        auto& dict = node.AsMap();
        stop.name = dict.at("name").AsString();
        stop.position.latitude = dict.at("latitude").AsDouble();
        stop.position.longitude = dict.at("longitude").AsDouble();
        for (const auto& [stop_name_node, distance] : dict.at("road_distances").AsMap()) {
            stop.distances[stop_name_node] = distance.AsInt();
        }
    }
    void AddStopRequest::Process(TransportManager& manager) const
    {
        manager.AddStop(move(stop));
    }

    void AddRouteSettingsRequest::ParseFrom(const Node& node)
    {
        const auto settings_map = node.AsMap();
        settings = {settings_map.at("bus_wait_time").AsInt(),
                    settings_map.at("bus_velocity").AsDouble()};
    }
    void AddRouteSettingsRequest::Process(TransportManager& manager) const
    {
        manager.AddRouteSettings(move(settings));
    }

    void BusInfoRequest::ParseFrom(const Node& node)
    {
        auto& dict = node.AsMap();
        request_id = dict.at("id").AsInt();
        bus_name = dict.at("name").AsString();
    }
    ResponsePtr BusInfoRequest::Process(const TransportManager& manager) const
    {
        return manager.BusInfo(request_id, move(bus_name));
    }

    void StopInfoRequest::ParseFrom(const Node& node)
    {
        auto& dict = node.AsMap();
        request_id = dict.at("id").AsInt();
        stop_name = dict.at("name").AsString();
    }
    ResponsePtr StopInfoRequest::Process(const TransportManager& manager) const
    {
        return manager.StopInfo(request_id, move(stop_name));
    }

    void RouteInfoRequest::ParseFrom(const Node& node)
    {
        auto& dict = node.AsMap();
        request_id = dict.at("id").AsInt();
        path.from = dict.at("from").AsString();
        path.to = dict.at("to").AsString();
    }
    ResponsePtr RouteInfoRequest::Process(const TransportManager& manager) const
    {
        return manager.RouteInfo(request_id, path);
    }

    optional<Type> ConvertRequestTypeFromJson(const Json::Node& node, bool is_update_request)
    {
        string_view type = node.AsMap().at("type").AsString();
        if (type == "Bus") {
            if (is_update_request) {
                return Type::ADD_BUS;
            } else {
                return Type::BUS_INFO;
            }
        } else if (type == "Stop") {
            if (is_update_request) {
                return Type::ADD_STOP;
            } else {
                return Type::STOP_INFO;
            }
        } else if (type == "Route") {
            return Type::ROUTE_INFO;
        }

        return nullopt;
    }

    RequestPtr ParseRequest(const Json::Node& node, bool is_update_request)
    {
        const auto request_type = ConvertRequestTypeFromJson(node, is_update_request);
        if (!request_type) {
            return nullptr;
        }
        RequestPtr request = Request::Create(*request_type);
        if (request) {
            request->ParseFrom(node);
        };
        return request;
    }

    RoutingSettings ReadRoutingSettings(const Json::Node &node)
    {
        const auto settings_map = node.AsMap().at("routing_settings").AsMap();
        return {settings_map.at("bus_wait_time").AsInt(),
                settings_map.at("bus_velocity").AsDouble()};
    }

    std::vector<RequestPtr> ReadRequests(const Node &node)
    {
        vector<RequestPtr> requests;
        if (node.AsMap().count("routing_settings") > 0) {
            RequestPtr request = Request::Create(Type::ADD_ROUTE_SETTINGS);
            if (request) {
                request->ParseFrom(node.AsMap().at("routing_settings"));
            };
            requests.push_back(move(request));
        }

        vector<Node> json_update_requests = node.AsMap().at("base_requests").AsArray();
        requests.reserve(requests.size());
        for (const auto& json_node : json_update_requests) {
            if (auto request = ParseRequest(json_node, true)) {
                requests.push_back(move(request));
            }
        }

        vector<Node> json_read_requests = node.AsMap().at("stat_requests").AsArray();
        requests.reserve(requests.size() + json_read_requests.size());
        for (const auto& json_node : json_read_requests) {
            if (auto request = ParseRequest(json_node, false)) {
                requests.push_back(move(request));
            }
        }

        return requests;
    }

    vector<ResponsePtr> ProcessRequests(TransportManager& manager, const vector<RequestPtr>& requests)
    {
        vector<ResponsePtr> responses;
        for (const auto& request_holder : requests) {
            try {
                const auto& request = dynamic_cast<const ReadRequest&>(*request_holder);
                responses.push_back(request.Process(manager));
            } catch (const bad_cast& e) {
                const auto& request = dynamic_cast<const UpdateRequest&>(*request_holder);
                request.Process(manager);
            }
        }
        return responses;
    }

    ostream& operator<<(ostream& out_stream, const Type& type)
    {
        return out_stream << static_cast<int>(type);
    }

}