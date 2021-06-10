#pragma once

#include <exception>
#include <iostream>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include <vector>

#include "manager.h"
#include "data_types.h"
#include "json.h"

struct Request;
using RequestPtr = std::unique_ptr<Request>;

struct Request {
    enum class Type {
        ADD_ROUTE_SETTINGS,
        ADD_STOP,
        ADD_BUS,
        BUS_INFO,
        STOP_INFO,
        ROUTE_INFO
    };

    Request(Type type) : type(type) {}
    static RequestPtr Create(Type type);
    virtual void ParseFrom(std::string_view input) = 0;
    virtual void ParseFromJson(const Json::Node& node) = 0;
    virtual ~Request() = default;

    const Type type;
};

struct ReadRequest : Request {
    using Request::Request;
    virtual ResponsePtr Process(const Server& manager) const = 0;

    size_t request_id;
};

struct UpdateRequest : Request {
    using Request::Request;

    virtual void Process(Server& manager) const = 0;
};

struct AddBusRequest : UpdateRequest {
    AddBusRequest() : UpdateRequest(Type::ADD_BUS) {}
    void ParseFrom(std::string_view input) override;
    void ParseFromJson(const Json::Node& node) override;

    void Process(Server& manager) const override;

    BusData bus;
};

struct AddStopRequest : UpdateRequest {
    AddStopRequest() : UpdateRequest(Type::ADD_STOP) {}
    void ParseFrom(std::string_view input) override;
    void ParseFromJson(const Json::Node& node) override;

    void Process(Server& manager) const override;

    StopData stop;
};

struct AddRouteSettingsRequest : UpdateRequest {
    AddRouteSettingsRequest() : UpdateRequest(Type::ADD_ROUTE_SETTINGS) {}
    void ParseFrom(std::string_view input) override;
    void ParseFromJson(const Json::Node& node) override;

    void Process(Server& manager) const override;

    RoutingSettings settings;
};

struct BusInfoRequest : ReadRequest {
    BusInfoRequest() : ReadRequest(Type::BUS_INFO) {}
    void ParseFrom(std::string_view input) override;
    void ParseFromJson(const Json::Node& node) override;

    ResponsePtr Process(const Server& manager) const override;

    std::string bus_name;
};

struct StopInfoRequest : ReadRequest {
    StopInfoRequest() : ReadRequest(Type::STOP_INFO) {}
    void ParseFrom(std::string_view input) override;
    void ParseFromJson(const Json::Node& node) override;

    ResponsePtr Process(const Server& manager) const override;

    std::string stop_name;
};

struct RouteInfoRequest : ReadRequest {
    RouteInfoRequest() : ReadRequest(Type::ROUTE_INFO) {}
    void ParseFrom(std::string_view input) override;
    void ParseFromJson(const Json::Node& node) override;

    ResponsePtr Process(const Server& manager) const override;

    Path path;
};

RequestPtr ParseRequest(std::string_view request_str);

std::vector<RequestPtr> ReadRequests(std::istream& in_stream = std::cin);

RequestPtr ParseJsonRequest(const Json::Node &node, bool is_update_request);

RoutingSettings ReadJsonRoutingSettings(const Json::Node &node);

std::vector<RequestPtr> ReadJsonRequests(const Json::Node &node);

std::vector<ResponsePtr> ProcessRequests(Server& manager,
                                         const std::vector<RequestPtr>& requests);

std::ostream& operator<<(std::ostream& out_stream, const Request::Type& type);