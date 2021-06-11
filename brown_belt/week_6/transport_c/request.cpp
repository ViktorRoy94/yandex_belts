#include "request.h"
#include "read_utils.h"

using namespace std;

RequestPtr Request::Create(Request::Type type) {
    switch (type) {
    case Request::Type::ADD_BUS:
        return make_unique<AddBusRequest>();
    case Request::Type::ADD_STOP:
        return make_unique<AddStopRequest>();
    case Request::Type::BUS_INFO:
        return make_unique<BusInfoRequest>();
    case Request::Type::STOP_INFO:
        return make_unique<StopInfoRequest>();
    default:
        return nullptr;
    }
}

void AddBusRequest::ParseFrom(string_view input) {
    ReadToken(input);
    bus_name = ReadToken(input, ":");
    string_view delimiter = ">";
    if (input.find(delimiter) == input.npos) {
        delimiter = "-";
        is_circle_path = false;
    }

    while (input != "") {
        string_view stop = ReadToken(input, delimiter);
        stops.emplace_back(string(stop));
    }
}
void AddBusRequest::Process(TransportManager& manager) const {
    manager.AddBus(move(bus_name), move(stops), is_circle_path);
}

void AddStopRequest::ParseFrom(string_view input) {
    ReadToken(input);
    stop.stop_name = ReadToken(input, ":");
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
void AddStopRequest::Process(TransportManager& manager) const {
    manager.AddStop(move(stop));
}

void BusInfoRequest::ParseFrom(string_view input) {
    ReadToken(input);
    bus_name = Rstrip(input);
}
ResponsePtr BusInfoRequest::Process(const TransportManager& manager) const {
    return manager.BusInfo(move(bus_name));
}

void StopInfoRequest::ParseFrom(string_view input) {
    ReadToken(input);
    stop_name = Rstrip(input);
}
ResponsePtr StopInfoRequest::Process(const TransportManager& manager) const {
    return manager.StopInfo(move(stop_name));
}

optional<Request::Type> ConvertRequestTypeFromString(string_view request_str) {
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

RequestPtr ParseRequest(string_view request_str) {
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

vector<RequestPtr> ReadRequests(istream& in_stream) {
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

vector<ResponsePtr> ProcessRequests(TransportManager& manager, const vector<RequestPtr>& requests) {
    vector<ResponsePtr> responses;
    for (const auto& request_holder : requests) {
        try {
            const auto& request = dynamic_cast<const ReadRequest&>(*request_holder);
            responses.push_back(request.Process(manager));
        } catch (const bad_cast& e) {
            const auto& request = dynamic_cast<const UpdateRequest&>(*request_holder);
            request.Process(manager);
        }


//        if (request_holder->type == Request::Type::BUS_INFO) {
//            const auto& request = static_cast<const BusInfoRequest&>(*request_holder);
//            responses.push_back(request.Process(manager));
//        } else {
//            const auto& request = static_cast<const UpdateRequest&>(*request_holder);
//            request.Process(manager);
//        }
    }
    return responses;
}

ostream& operator<<(ostream& out_stream, const Request::Type& type) {
    return out_stream << static_cast<int>(type);
}

