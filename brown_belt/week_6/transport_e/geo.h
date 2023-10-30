#pragma once

#include <ostream>
#include <iostream>
#include <vector>
#include <set>

namespace Geo
{
    struct Coordinates
    {
        double latitude;
        double longitude;

        static Coordinates FromDegrees(double latitude, double longitude);
    };

    bool operator==(const Coordinates& lhs, const Coordinates& rhs);
    std::ostream& operator<<(std::ostream& out_stream, const Coordinates& coords);
    double Distance(Coordinates lhs, Coordinates rhs);

    double ConvertDegreesToRadians(double degrees);
}