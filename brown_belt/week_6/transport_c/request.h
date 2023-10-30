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

struct Request;
using RequestPtr = std::unique_ptr<Request>;

struct Request {
    enum class Type {
        ADD_STOP,
        ADD_BUS,
        BUS_INFO,
        STOP_INFO
    };

    Request(Type type) : type(type) {}
    static RequestPtr Create(Type type);
    virtual void ParseFrom(std::string_view input) = 0;
    virtual ~Request() = default;

    const Type type;
};

struct ReadRequest : Request {
    using Request::Request;
    virtual ResponsePtr Process(const TransportManager& manager) const = 0;
};

struct UpdateRequest : Request {
    using Request::Request;
    virtual void Process(TransportManager& manager) const = 0;
};

struct AddBusRequest : UpdateRequest {
    AddBusRequest() : UpdateRequest(Type::ADD_BUS) {}
    void ParseFrom(std::string_view input) override;

    void Process(TransportManager& manager) const override;

    std::string bus_name;
    StopNames stops;
    bool is_circle_path = true;
};

struct AddStopRequest : UpdateRequest {
    AddStopRequest() : UpdateRequest(Type::ADD_STOP) {}
    void ParseFrom(std::string_view input) override;

    void Process(TransportManager& manager) const override;

    StopData stop;
};

struct BusInfoRequest : ReadRequest {
    BusInfoRequest() : ReadRequest(Type::BUS_INFO) {}
    void ParseFrom(std::string_view input) override;

    ResponsePtr Process(const TransportManager& manager) const override;

    std::string bus_name;
};

struct StopInfoRequest : ReadRequest {
    StopInfoRequest() : ReadRequest(Type::STOP_INFO) {}
    void ParseFrom(std::string_view input) override;

    ResponsePtr Process(const TransportManager& manager) const override;

    std::string stop_name;
};

RequestPtr ParseRequest(std::string_view request_str);

std::vector<RequestPtr> ReadRequests(std::istream& in_stream = std::cin);

std::vector<ResponsePtr> ProcessRequests(TransportManager& manager,
                                         const std::vector<RequestPtr>& requests);

std::ostream& operator<<(std::ostream& out_stream, const Request::Type& type);