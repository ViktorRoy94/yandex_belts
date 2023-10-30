#pragma once

#include "data_types.h"

#include <memory>
#include <iostream>

namespace Response {
    struct Response;
    using ResponsePtr = std::unique_ptr<Response>;

    struct Response {
        enum class Type {
            BUS_INFO,
            STOP_INFO,
            ROUTE_INFO
        };

        Response(Type type, size_t id) : type(type), request_id(id) {}
        virtual void PrintJson(std::ostream& out = std::cout) = 0;
        virtual ~Response() = default;

        const Type type;
        size_t request_id;
    };

    struct BusInfoResponse : Response {
        BusInfoResponse(size_t request_id, Statistic::Bus statistic)
            : Response(Type::BUS_INFO, request_id)
            , statistic(statistic)
        {}
        void PrintJson(std::ostream& out = std::cout) override;

        Statistic::Bus statistic;
    };

    struct StopInfoResponse : Response {
        StopInfoResponse(size_t request_id, Statistic::Stop statistic)
            : Response(Type::STOP_INFO, request_id)
            , statistic(statistic)
        {}
        void PrintJson(std::ostream& out = std::cout) override;

        Statistic::Stop statistic;
    };

    struct RouteInfoResponse : Response {
        RouteInfoResponse(size_t request_id, Statistic::Route statistic)
            : Response(Type::ROUTE_INFO, request_id)
            , statistic(std::move(statistic))
        {}
        void PrintJson(std::ostream& out = std::cout) override;

        Statistic::Route statistic;
    };

    void PrintResponcesInJsonFormat(const std::vector<ResponsePtr>& responses, std::ostream& out = std::cout);
}

