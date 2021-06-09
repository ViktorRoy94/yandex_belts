#include "data_types.h"

#include <cmath>
#include <tuple>

using namespace std;

double LengthBetwenCoords(const Coordinates& coord1, const Coordinates& coord2) {
    double lat1  = coord1.latitude  * PI / 180.;
    double lat2  = coord2.latitude  * PI / 180.;
    double long1 = coord1.longitude * PI / 180.;
    double long2 = coord2.longitude * PI / 180.;

    return acos(
            sin(lat1) * sin(lat2) +
            cos(lat1) * cos(lat2) *
            cos(abs(long1 - long2))) * EARTH_R;
}

bool operator==(const Coordinates& lhs, const Coordinates& rhs) {
    return ((lhs.latitude - rhs.latitude) < 0.000001 &&
            (lhs.longitude - rhs.longitude) < 0.000001);
}
std::ostream& operator<<(std::ostream& out_stream, const Coordinates& coords) {
    return out_stream << coords.latitude << " " << coords.longitude;
}

bool Path::operator==(const Path& other) const {
    return tie(from, to) == tie(other.from, other.to);
}

size_t PathHasher::operator() (const Path& a) const {
    size_t coef = 320412427;
    hash<string> shash;
    return coef * shash(a.from) + shash(a.to);
}

size_t StringPairHasher::operator() (const pair<string, string>& a) const {
    size_t coef = 280561877;
    hash<string> shash;
    return coef * shash(a.first) + shash(a.second);
}

void WaitItem::PrintJson(std::ostream& out) {
    out << "{";
    out << "\"type\": \"Wait\", ";
    out << "\"stop_name\": " << "\"" << stop_name << "\", ";
    out << "\"time\": " << time;
    out << "}";
}

bool operator==(const WaitItem& lhs, const WaitItem& rhs) {
    return lhs.stop_name == rhs.stop_name &&
           abs(lhs.time - rhs.time) < 0.0001;
}
std::ostream& operator<<(std::ostream& out_stream, const WaitItem& item) {
    return out_stream << "stop_name: " << item.stop_name << " "
                      << "time: "     << item.time;
}

void BusItem::PrintJson(std::ostream& out) {
    out << "{";
    out << "\"type\": \"Bus\", ";
    out << "\"bus\": " << "\"" << bus_name << "\", ";
    out << "\"span_count\": " << span_count << ", ";
    out << "\"time\": " << time;
    out << "}";
}

bool operator==(const BusItem& lhs, const BusItem& rhs) {
    return tie(lhs.bus_name, lhs.span_count) ==
           tie(rhs.bus_name, rhs.span_count) &&
           abs(lhs.time - rhs.time) < 0.0001;
}
std::ostream& operator<<(std::ostream& out_stream, const BusItem& item) {
    return out_stream << "bus_name: " << item.bus_name << " "
                      << "span_count: " << item.span_count << " "
                      << "time: " << item.time;
}
