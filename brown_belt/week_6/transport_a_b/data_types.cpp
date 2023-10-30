#include "data_types.h"

#include <cmath>

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