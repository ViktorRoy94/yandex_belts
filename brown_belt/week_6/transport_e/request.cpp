#include "request.h"
#include "read_utils.h"

using namespace std;
using namespace Json;

RequestPtr Request::Create(Request::Type type)
{
    switch (type) {
    case Request::Type::ADD_BUS:
        return make_unique<AddBusRequest>();
    case Request::Type::ADD_STOP:
        return make_unique<AddStopRequest>();
    case Request::Type::BUS_INFO:
        return make_unique<BusInfoRequest>();
    case Request::Type::STOP_INFO:
        return make_unique<StopInfoRequest>();
    case Request::Type::ROUTE_INFO:
        return make_unique<RouteInfoRequest>();
    default:
        return nullptr;
    }
}

void AddBusRequest::ParseFrom(string_view input)
{
    ReadToken(input);
    bus.name = ReadToken(input, ":");
    string_view delimiter = ">";
    if (input.find(delimiter) == input.npos) {
        delimiter = "-";
        bus.is_circle_path = false;
    }

    while (input != "") {
        string_view stop = ReadToken(input, delimiter);
        bus.stops.emplace_back(string(stop));
    }
}
void AddBusRequest::ParseFromJson(const Node& node)
{
    auto& dict = node.AsMap();
    bus.name = dict.at("name").AsString();
    bus.is_circle_path = dict.at("is_roundtrip").AsBool();
    for (const auto& stop_name_node : dict.at("stops").AsArray()) {
        bus.stops.push_back(stop_name_node.AsString());
    }
}
void AddBusRequest::Process(TransportManager& manager) const
{
    manager.AddBus(move(bus));
}

void AddStopRequest::ParseFrom(string_view input)
{
    ReadToken(input);
    stop.name = ReadToken(input, ":");
    stop.coords.latitude = ConvertToDouble(ReadToken(input, ","));
    stop.coords.longitude = ConvertToDouble(ReadToken(input, ","));
    while (input.size() > 0) {
        string_view distance_view = ReadToken(input, "m ");
        size_t distance = ConvertToInt(distance_view);
        ValidateBounds(distance, size_t(0), size_t(1000000));
        ReadToken(input, "to ");
        string_view another_stop = ReadToken(input, ",");
        stop.distances.push_back({string(another_stop), distance});
    }
}
void AddStopRequest::ParseFromJson(const Node& node)
{
    auto& dict = node.AsMap();
    stop.name = dict.at("name").AsString();
    stop.coords.latitude = dict.at("latitude").AsDouble();
    stop.coords.longitude = dict.at("longitude").AsDouble();
    for (const auto& [stop_name_node, distance] : dict.at("road_distances").AsMap()) {
        stop.distances.push_back({stop_name_node, distance.AsInt()});
    }
}
void AddStopRequest::Process(TransportManager& manager) const
{
    manager.AddStop(move(stop));
}

void BusInfoRequest::ParseFrom(string_view input)
{
    ReadToken(input);
    bus_name = Rstrip(input);
}
void BusInfoRequest::ParseFromJson(const Node& node)
{
    auto& dict = node.AsMap();
    request_id = dict.at("id").AsInt();
    bus_name = dict.at("name").AsString();
}
ResponsePtr BusInfoRequest::Process(const TransportManager& manager) const
{
    return manager.BusInfo(request_id, move(bus_name));
}

void StopInfoRequest::ParseFrom(string_view input)
{
    ReadToken(input);
    stop_name = Rstrip(input);
}
void StopInfoRequest::ParseFromJson(const Node& node)
{
    auto& dict = node.AsMap();
    request_id = dict.at("id").AsInt();
    stop_name = dict.at("name").AsString();
}
ResponsePtr StopInfoRequest::Process(const TransportManager& manager) const
{
    return manager.StopInfo(request_id, move(stop_name));
}

void RouteInfoRequest::ParseFrom(string_view input)
{
//    ReadToken(input);
//    stop_name = Rstrip(input);
}
void RouteInfoRequest::ParseFromJson(const Node& node)
{
    auto& dict = node.AsMap();
    request_id = dict.at("id").AsInt();
    path.from = dict.at("from").AsString();
    path.to = dict.at("to").AsString();
}
ResponsePtr RouteInfoRequest::Process(const TransportManager& manager) const
{
    return manager.StopInfo(request_id, "");
}

optional<Request::Type> ConvertRequestTypeFromString(string_view request_str)
{
    string_view first_word = ReadToken(request_str);
    if (first_word == "Bus") {
        if (request_str.find(':') == request_str.npos) {
            return Request::Type::BUS_INFO;
        } else {
            return Request::Type::ADD_BUS;
        }
    } else if (first_word == "Stop") {
        if (request_str.find(':') == request_str.npos) {
            return Request::Type::STOP_INFO;
        } else {
            return Request::Type::ADD_STOP;
        }
    }

    return nullopt;
}

RequestPtr ParseRequest(string_view request_str)
{
    const auto request_type = ConvertRequestTypeFromString(request_str);
    if (!request_type) {
        return nullptr;
    }
    RequestPtr request = Request::Create(*request_type);
    if (request) {
        request->ParseFrom(request_str);
    };
    return request;
}

vector<RequestPtr> ReadRequests(istream& in_stream)
{
    vector<RequestPtr> requests;

    const size_t update_request_count = ReadNumberOnLine<size_t>(in_stream);
    requests.reserve(update_request_count);

    for (size_t i = 0; i < update_request_count; ++i) {
        string request_str;
        getline(in_stream, request_str);
        if (auto request = ParseRequest(request_str)) {
            requests.push_back(move(request));
        }
    }

    return requests;
}

optional<Request::Type> ConvertRequestTypeFromJson(const Json::Node& node, bool is_update_request)
{
    string_view type = node.AsMap().at("type").AsString();
    if (type == "Bus") {
        if (is_update_request) {
            return Request::Type::ADD_BUS;
        } else {
            return Request::Type::BUS_INFO;
        }
    } else if (type == "Stop") {
        if (is_update_request) {
            return Request::Type::ADD_STOP;
        } else {
            return Request::Type::STOP_INFO;
        }
    } else if (type == "Route") {
        return Request::Type::ROUTE_INFO;
    }

    return nullopt;
}

RequestPtr ParseJsonRequest(const Json::Node& node, bool is_update_request)
{
    const auto request_type = ConvertRequestTypeFromJson(node, is_update_request);
    if (!request_type) {
        return nullptr;
    }
    RequestPtr request = Request::Create(*request_type);
    if (request) {
        request->ParseFromJson(node);
    };
    return request;
}

RoutingSettings ReadJsonRoutingSettings(const Json::Node &node)
{
    const auto settings_map = node.AsMap().at("routing_settings").AsMap();
    return {settings_map.at("bus_wait_time").AsInt(),
            settings_map.at("bus_velocity").AsDouble()};
}

std::vector<RequestPtr> ReadJsonRequests(const Node &node)
{
    vector<RequestPtr> requests;
    vector<Node> json_update_requests = node.AsMap().at("base_requests").AsArray();
    requests.reserve(json_update_requests.size());
    for (const auto& json_node : json_update_requests) {
        if (auto request = ParseJsonRequest(json_node, true)) {
            requests.push_back(move(request));
        }
    }

    vector<Node> json_read_requests = node.AsMap().at("stat_requests").AsArray();
    requests.reserve(json_update_requests.size() + json_read_requests.size());
    for (const auto& json_node : json_read_requests) {
        if (auto request = ParseJsonRequest(json_node, false)) {
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

ostream& operator<<(ostream& out_stream, const Request::Type& type)
{
    return out_stream << static_cast<int>(type);
}

