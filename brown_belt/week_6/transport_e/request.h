#pragma once

#include "manager.h"
#include "data_types.h"
#include "json.h"

#include <exception>
#include <iostream>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace Request {
    struct Request;
    using RequestPtr = std::unique_ptr<Request>;

    enum class Type {
        ADD_ROUTE_SETTINGS,
        ADD_STOP,
        ADD_BUS,
        BUS_INFO,
        STOP_INFO,
        ROUTE_INFO
    };

    struct Request {
        Request(Type type) : type(type) {}
        static RequestPtr Create(Type type);
        virtual void ParseFrom(const Json::Node& node) = 0;
        virtual ~Request() = default;

        const Type type;
    };

    struct ReadRequest : Request {
        using Request::Request;
        virtual Response::ResponsePtr Process(const TransportManager& manager) const = 0;

        size_t request_id;
    };

    struct UpdateRequest : Request {
        using Request::Request;

        virtual void Process(TransportManager& manager) const = 0;
    };

    struct AddBusRequest : UpdateRequest {
        AddBusRequest() : UpdateRequest(Type::ADD_BUS) {}
        void ParseFrom(const Json::Node& node) override;

        void Process(TransportManager& manager) const override;

        Data::Bus bus;
    };

    struct AddStopRequest : UpdateRequest {
        AddStopRequest() : UpdateRequest(Type::ADD_STOP) {}
        void ParseFrom(const Json::Node& node) override;

        void Process(TransportManager& manager) const override;

        Data::Stop stop;
    };

    struct AddRouteSettingsRequest : UpdateRequest {
        AddRouteSettingsRequest() : UpdateRequest(Type::ADD_ROUTE_SETTINGS) {}
        void ParseFrom(const Json::Node& node) override;

        void Process(TransportManager& manager) const override;

        RoutingSettings settings;
    };

    struct BusInfoRequest : ReadRequest {
        BusInfoRequest() : ReadRequest(Type::BUS_INFO) {}
        void ParseFrom(const Json::Node& node) override;

        Response::ResponsePtr Process(const TransportManager& manager) const override;

        std::string bus_name;
    };

    struct StopInfoRequest : ReadRequest {
        StopInfoRequest() : ReadRequest(Type::STOP_INFO) {}
        void ParseFrom(const Json::Node& node) override;

        Response::ResponsePtr Process(const TransportManager& manager) const override;

        std::string stop_name;
    };

    struct RouteInfoRequest : ReadRequest {
        RouteInfoRequest() : ReadRequest(Type::ROUTE_INFO) {}
        void ParseFrom(const Json::Node& node) override;

        Response::ResponsePtr Process(const TransportManager& manager) const override;

        Path path;
    };

    RequestPtr ParseRequest(const Json::Node &node, bool is_update_request);

    RoutingSettings ReadRoutingSettings(const Json::Node &node);

    std::vector<RequestPtr> ReadRequests(const Json::Node &node);

    std::vector<Response::ResponsePtr> ProcessRequests(TransportManager& manager,
                                                       const std::vector<RequestPtr>& requests);

    std::ostream& operator<<(std::ostream& out_stream, const Type& type);
}
