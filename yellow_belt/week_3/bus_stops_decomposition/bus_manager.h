#pragma once
#include <string>
#include <map>
#include <vector>
#include "responses.h"

using namespace std;

class BusManager {
public:
    void AddBus(const string& bus, const vector<string>& stops);
    BusesForStopResponse GetBusesForStop(const string& stop) const;
    StopsForBusResponse GetStopsForBus(const string& bus) const;
    AllBusesResponse GetAllBuses() const;

private:
    map<string, vector<string>> buses_to_stop_;
    map<string, vector<string>> stops_to_buses_;
};