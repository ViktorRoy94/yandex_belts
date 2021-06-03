#include "response.h"

#include <iomanip>

using namespace std;

void BusInfoResponse::Print(ostream& out) {
//    out.precision(6);
    out << setprecision(7) << "Bus " << statistic.bus_name << ": ";
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

void StopInfoResponse::Print(ostream& out) {
    out.precision(6);
    out << "Stop " << statistic.stop_name << ": ";
    if (!statistic.is_stop_found) {
        out << "not found";
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