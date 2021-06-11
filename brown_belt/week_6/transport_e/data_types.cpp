#include "data_types.h"

#include <tuple>

using namespace std;

namespace Data {
    size_t ComputeStopsDistance(const Stop& lhs, const Stop& rhs) {
        auto it = lhs.distances.find(rhs.name);
        if (it != lhs.distances.end()) {
            return it->second;
        } else {
            return rhs.distances.at(lhs.name);
        }
    }
}

bool Path::operator==(const Path& other) const {
    return tie(from, to) == tie(other.from, other.to);
}

size_t PathHasher::operator() (const Path& a) const {
    size_t coef = 320412427;
    hash<string> shash;
    return coef * shash(a.from) + shash(a.to);
}

namespace Statistic {
    WaitItem::WaitItem(std::string stop_name, double time)
        : stop_name(stop_name)
        , time(time) {}
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

    BusItem::BusItem(std::string bus_name, size_t span_count, double time)
        : bus_name(bus_name)
        , span_count(span_count)
        , time(time) {}
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
}