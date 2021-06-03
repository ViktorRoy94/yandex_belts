#pragma once

#include <memory>
#include <iostream>

#include "data_types.h"

struct Response;
using ResponsePtr = std::unique_ptr<Response>;

struct Response {
    enum class Type {
        BUS_INFO,
        STOP_INFO,
        ROUTE_INFO
    };

    Response(Type type, size_t id) : type(type), request_id(id) {}
    virtual void Print(std::ostream& out = std::cout) = 0;
    virtual void PrintJson(std::ostream& out = std::cout) = 0;
    virtual ~Response() = default;

    const Type type;
    size_t request_id;
};

struct BusInfoResponse : Response {
    BusInfoResponse(size_t request_id, BusStat statistic)
        : Response(Type::BUS_INFO, request_id)
        , statistic(statistic)
    {}
    void Print(std::ostream& out = std::cout) override;
    void PrintJson(std::ostream& out = std::cout) override;

    BusStat statistic;
};

struct StopInfoResponse : Response {
    StopInfoResponse(size_t request_id, StopStat statistic)
        : Response(Type::STOP_INFO, request_id)
        , statistic(statistic)
    {}
    void Print(std::ostream& out = std::cout) override;
    void PrintJson(std::ostream& out = std::cout) override;

    StopStat statistic;
};

struct RouteInfoResponse : Response {
    RouteInfoResponse(size_t request_id, RouteStat statistic)
        : Response(Type::ROUTE_INFO, request_id)
        , statistic(std::move(statistic))
    {}
    void Print(std::ostream& out = std::cout) override;
    void PrintJson(std::ostream& out = std::cout) override;

    RouteStat statistic;
};

void PrintResponces(const std::vector<ResponsePtr>& responses, std::ostream& out = std::cout);
void PrintResponcesInJsonFormat(const std::vector<ResponsePtr>& responses, std::ostream& out = std::cout);