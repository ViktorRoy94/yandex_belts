#include "response.h"

#include <iomanip>

using namespace std;

namespace Response {

    void BusInfoResponse::PrintJson(std::ostream& out) {
        out << "{";
        if (statistic.error_message.empty()) {
            out << "\"route_length\": " << statistic.path_length << ", ";
            out << "\"request_id\": " << request_id << ", ";
            out << "\"curvature\": " << statistic.curvature << ", ";
            out << "\"stop_count\": " << statistic.stops_count << ", ";
            out << "\"unique_stop_count\": " << statistic.unique_stops_count;
        } else {
            out << "\"request_id\": " << request_id << ", ";
            out << "\"error_message\": " << "\"" << statistic.error_message << "\"";
        }
        out << "}";
    }

    void StopInfoResponse::PrintJson(std::ostream& out) {
        out << "{";
        if (statistic.error_message.empty()) {
            out << "\"buses\": " << "[";
            bool first = true;
            for (const auto& bus_name : statistic.bus_names) {
                if (!first) {
                    out << ", ";
                }
                out << "\"" << bus_name << "\"";
                first = false;
            }
            out << "], ";
            out << "\"request_id\": " << request_id;
        } else {
            out << "\"request_id\": " << request_id << ", ";
            out << "\"error_message\": " << "\"" << statistic.error_message << "\"";
        }
        out << "}";
    }

    void RouteInfoResponse::PrintJson(std::ostream& out) {
        out << "{";
        if (statistic.error_message.empty()) {
            out << "\"request_id\": " << request_id << ", ";
            out << "\"total_time\": " << statistic.total_time << ", ";
            out << "\"items\": " << "[";
            bool first = true;
            for (const auto& item : statistic.items) {
                if (!first) {
                    out << ", ";
                }
                item->PrintJson(out);
                first = false;
            }
            out << "]";
        } else {
            out << "\"request_id\": " << request_id << ", ";
            out << "\"error_message\": " << "\"" << statistic.error_message << "\"";
        }
        out << "}";
    }

    void PrintResponcesInJsonFormat(const std::vector<ResponsePtr>& responses, std::ostream& out) {
        out << setprecision(6);
        out << "[";
        bool first = true;
        for (const auto& response : responses) {
            if (!first) {
                out << ", ";
            }
            response->PrintJson(out);
            first = false;
        }
        out << "]";
    }

}