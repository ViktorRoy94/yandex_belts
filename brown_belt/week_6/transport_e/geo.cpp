#include "geo.h"

#include <cmath>

using namespace std;

namespace Geo
{
    const int EARTH_RADIUS = 6371000;
    const double PI = 3.1415926535;

    double ConvertDegreesToRadians(double degrees) {
        return degrees * PI / 180.0;
    }

    Coordinates Coordinates::FromDegrees(double latitude, double longitude) {
        return {
            ConvertDegreesToRadians(latitude),
            ConvertDegreesToRadians(longitude)
        };
    }

    double Distance(Coordinates lhs, Coordinates rhs) {
        lhs = Coordinates::FromDegrees(lhs.latitude, lhs.longitude);
        rhs = Coordinates::FromDegrees(rhs.latitude, rhs.longitude);

        return acos(
                sin(lhs.latitude) * sin(rhs.latitude) +
                cos(lhs.latitude) * cos(rhs.latitude) *
                cos(abs(lhs.longitude - rhs.longitude))) * EARTH_RADIUS;
    }

    bool operator==(const Coordinates& lhs, const Coordinates& rhs) {
        return ((lhs.latitude - rhs.latitude) < 0.000001 &&
                (lhs.longitude - rhs.longitude) < 0.000001);
    }
    std::ostream& operator<<(std::ostream& out_stream, const Coordinates& coords) {
        return out_stream << coords.latitude << " " << coords.longitude;
    }

}