#pragma once

#include <memory>
#include <iostream>

#include "data_types.h"

struct Response;
using ResponsePtr = std::unique_ptr<Response>;

struct Response {
    enum class Type {
        BUS_INFO,
        STOP_INFO
    };

    Response(Type type) : type(type) {}
    virtual void Print(std::ostream& out = std::cout) = 0;
    virtual ~Response() = default;

    const Type type;
};

struct BusInfoResponse : Response {
    BusInfoResponse(BusStat statistic)
        : Response(Type::BUS_INFO)
        , statistic(statistic)
    {}
    void Print(std::ostream& out = std::cout) override;

    BusStat statistic;
};

struct StopInfoResponse : Response {
    StopInfoResponse(StopStat statistic)
        : Response(Type::STOP_INFO)
        , statistic(statistic)
    {}
    void Print(std::ostream& out = std::cout) override;

    StopStat statistic;
};