#include "response.h"

#include <iomanip>

using namespace std;

void BusInfoResponse::Print(ostream& out) {
    out << "Bus " << statistic.bus_name << ": ";
    if (statistic.stops_count != 0) {
        out << statistic.stops_count << " stops on route, "
            << statistic.unique_stops_count << " unique stops, "
            << statistic.path_length << " route length, "
            << statistic.curvature << " curvature";
    } else {
        out << "not found";
    }
    out << endl;
}

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

void StopInfoResponse::Print(ostream& out) {
    out << "Stop " << statistic.stop_name << ": ";
    if (!statistic.error_message.empty()) {
        out << statistic.error_message;
    } else {
        if (statistic.bus_names.size() != 0) {
            out << "buses";
            for (const auto& bus : statistic.bus_names) {
                out << " " << bus;
            }
        } else {
            out << "no buses";
        }
    }
    out << endl;
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

void RouteInfoResponse::Print(ostream& out) {
    out << "Route : " << statistic.total_time << endl;
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

void PrintResponces(const std::vector<ResponsePtr>& responses, std::ostream& out) {
    out << setprecision(7);
    for (const auto& response : responses) {
        response->Print(out);
    }
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