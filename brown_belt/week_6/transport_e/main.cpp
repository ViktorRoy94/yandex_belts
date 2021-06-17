#include "request.h"
#include "test_runner.h"
#include "profile.h"
#include "geo.h"

#include <fstream>

using namespace std;
using namespace Json;
using namespace Geo;
using namespace Request;
using namespace Response;


bool CompareResponseJsonStrings(std::string str1, std::string str2) {
    istringstream input1(str1);
    istringstream input2(str2);
    Document doc1 = Load(input1);
    Document doc2 = Load(input2);

    const auto& arr1 = doc1.GetRoot().AsArray();
    const auto& arr2 = doc2.GetRoot().AsArray();

    bool result = arr1.size() == arr2.size();
    if (!result) return result;
    for (size_t i = 0; i < arr1.size(); i++) {
        if (arr1[i].AsMap().count("items") == 0) {
            result = result && CompareNodes(arr1[i], arr2[i]);
        } else {
            const auto& map1 = arr1[i].AsMap();
            const auto& map2 = arr2[i].AsMap();
            result = result && map1.at("request_id").AsInt() == map2.at("request_id").AsInt();
            double time_diff = abs(map1.at("total_time").AsDouble() -
                                   map2.at("total_time").AsDouble());
            result = result && time_diff < 0.005;
            if (!result)
                return result;
        }
    }
    return result;
}

void TestJson() {
    {
        istringstream input(R"({
                                  "d1": 1823423.907907,
                                  "d2": -1823423.907907,
                                  "d3": 1823423,
                                  "1": false,
                                  "2": "828",
                                  "array": [],
                                  "last": true
                                })");

        Node node = Load(input).GetRoot();
        ASSERT_EQUAL(node.AsMap().at("d1").AsDouble(), 1823423.907907);
        ASSERT_EQUAL(node.AsMap().at("d2").AsDouble(), -1823423.907907);
        ASSERT_EQUAL(node.AsMap().at("d3").AsDouble(), 1823423.0);
        ASSERT_EQUAL(node.AsMap().at("1").AsBool(), false);
        ASSERT_EQUAL(node.AsMap().at("2").AsString(), "828");
        ASSERT_EQUAL(node.AsMap().at("array").AsArray().size(), 0u);
        ASSERT_EQUAL(node.AsMap().at("last").AsBool(), true);
    }
    {
        istringstream input(R"({"d1": 1823423.907907, "d2": -1823423.907907, "d3": 1823423, "1": false, "2": "828", "array": [], "last": true })");
        Node node = Load(input).GetRoot();
        ASSERT_EQUAL(node.AsMap().at("d1").AsDouble(), 1823423.907907);
        ASSERT_EQUAL(node.AsMap().at("d2").AsDouble(), -1823423.907907);
        ASSERT_EQUAL(node.AsMap().at("d3").AsDouble(), 1823423.0);
        ASSERT_EQUAL(node.AsMap().at("1").AsBool(), false);
        ASSERT_EQUAL(node.AsMap().at("2").AsString(), "828");
        ASSERT_EQUAL(node.AsMap().at("array").AsArray().size(), 0u);
        ASSERT_EQUAL(node.AsMap().at("last").AsBool(), true);
    }
}

void TestJsonCompare() {
    {
        string input(R"({"int": 10, "double": 1823423.1231, "bool": true, "array": [1, 2, 3]})");
        string input_changed_int(R"({"int": 9, "double": 1823423.1231, "bool": true, "array": [1, 2, 3]})");
        string input_changed_double(R"({"int": 10, "double": 1823423.14, "bool": true, "array": [1, 2, 3]})");
        string input_changed_bool(R"({"int": 10, "double": 1823423.1231, "bool": false, "array": [1, 2, 3]})");
        string input_changed_map(R"({"int": 10, "double": 1823423.1231, "bool": true)");
        string input_changed_array(R"({"int": 10, "double": 1823423.1231, "bool": true, "array": [10, 2, 3]})");
        ASSERT( CompareJsonStrings(input, input));
        ASSERT(!CompareJsonStrings(input, input_changed_int));
        ASSERT(!CompareJsonStrings(input, input_changed_double));
        ASSERT(!CompareJsonStrings(input, input_changed_bool));
        ASSERT(!CompareJsonStrings(input, input_changed_map));
        ASSERT(!CompareJsonStrings(input, input_changed_array));
    }
}

void TestParseJsonRoutingSettings() {
    istringstream input(R"({"routing_settings": {"bus_wait_time": 6, "bus_velocity": 40}})");
    Node node = Load(input).GetRoot();
    RoutingSettings settings = ReadRoutingSettings(node);
    ASSERT_EQUAL(settings.bus_wait_time, 6);
    ASSERT_EQUAL(settings.bus_velocity, 40);
}

void TestAddBusRequestParseFromJson() {
    {
        istringstream input(R"({
                                  "type": "Bus",
                                  "name": "828",
                                  "stops": [
                                    "Biryulyovo Zapadnoye",
                                    "Universam",
                                    "Rossoshanskaya ulitsa",
                                    "Biryulyovo Zapadnoye"
                                  ],
                                  "is_roundtrip": true
                                })");

        Node node = Load(input).GetRoot();
        AddBusRequest request;
        request.ParseFrom(node);
        ASSERT_EQUAL(request.bus.name, "828");
        ASSERT_EQUAL(request.bus.stops, vector<string>({"Biryulyovo Zapadnoye",
                                                        "Universam",
                                                        "Rossoshanskaya ulitsa",
                                                        "Biryulyovo Zapadnoye"}));
    }
    {
        istringstream input(R"({
                                  "type": "Bus",
                                  "name": "750",
                                  "stops": [
                                    "Tolstopaltsevo",
                                    "Marushkino",
                                    "Rasskazovka"
                                  ],
                                  "is_roundtrip": false
                               })");

        Node node = Load(input).GetRoot();
        AddBusRequest request;
        request.ParseFrom(node);
        ASSERT_EQUAL(request.bus.name, "750");
        ASSERT_EQUAL(request.bus.stops, vector<string>({"Tolstopaltsevo",
                                                        "Marushkino",
                                                        "Rasskazovka",
                                                        "Marushkino",
                                                        "Tolstopaltsevo"}));
    }
    {
        istringstream input(R"({ "type": "Bus", "name": "750", "stops": [ "Tolstopaltsevo", "Marushkino", "Rasskazovka" ], "is_roundtrip": false })");

        Node node = Load(input).GetRoot();
        AddBusRequest request;
        request.ParseFrom(node);
        ASSERT_EQUAL(request.bus.name, "750");
        ASSERT_EQUAL(request.bus.stops, vector<string>({"Tolstopaltsevo",
                                                        "Marushkino",
                                                        "Rasskazovka",
                                                        "Marushkino",
                                                        "Tolstopaltsevo"}));
    }
}

void TestAddStopRequestParseFromJson() {
    {
        istringstream input(R"({
                                  "type": "Stop",
                                  "road_distances": {
                                    "Marushkino": 3900
                                  },
                                  "longitude": 37.20829,
                                  "name": "Tolstopaltsevo",
                                  "latitude": 55.611087
                               })");

        Node node = Load(input).GetRoot();
        AddStopRequest request;
        request.ParseFrom(node);
        ASSERT_EQUAL(request.stop.name, "Tolstopaltsevo");
        ASSERT_EQUAL(request.stop.position, Coordinates({55.611087, 37.20829}));
        ASSERT_EQUAL(request.stop.distances["Marushkino"], 3900u);
    }
    {
        istringstream input(R"({
                                  "type": "Stop",
                                  "road_distances": {
                                    "Rasskazovka": 9900
                                  },
                                  "longitude": 37.209755,
                                  "name": "Marushkino",
                                  "latitude": 55.595884
                               })");
        Node node = Load(input).GetRoot();
        AddStopRequest request;
        request.ParseFrom(node);
        ASSERT_EQUAL(request.stop.name, "Marushkino");
        ASSERT_EQUAL(request.stop.position, Coordinates({55.595884, 37.209755}));
        ASSERT_EQUAL(request.stop.distances["Rasskazovka"], 9900u);
    }


    {
        istringstream input(R"({
                                  "type": "Stop",
                                  "road_distances": {
                                    "Rossoshanskaya ulitsa": 5600,
                                    "Biryulyovo Tovarnaya": 900
                                  },
                                  "longitude": 37.645687,
                                  "name": "Universam",
                                  "latitude": 55.587655
                               })");
        Node node = Load(input).GetRoot();
        AddStopRequest request;
        request.ParseFrom(node);
        ASSERT_EQUAL(request.stop.name, "Universam");
        ASSERT_EQUAL(request.stop.position, Coordinates({55.587655, 37.645687}));
        ASSERT_EQUAL(request.stop.distances["Biryulyovo Tovarnaya"], 900u);
        ASSERT_EQUAL(request.stop.distances["Rossoshanskaya ulitsa"], 5600u);
    }
    {
        istringstream input(R"({
                                  "type": "Stop",
                                  "road_distances": {},
                                  "longitude": 37.333324,
                                  "name": "Rasskazovka",
                                  "latitude": 55.632761
                               })");
        Node node = Load(input).GetRoot();
        AddStopRequest request;
        request.ParseFrom(node);
        ASSERT_EQUAL(request.stop.name, "Rasskazovka");
        ASSERT_EQUAL(request.stop.position, Coordinates({55.632761, 37.333324}));
        ASSERT_EQUAL(request.stop.distances.size(), 0u);
    }
}

void TestBusInfoRequestParseFromJson() {
    {
        istringstream input(R"({
                                  "type": "Bus",
                                  "name": "256",
                                  "id": 1965312327
                               })");

        Node node = Load(input).GetRoot();
        BusInfoRequest request;
        request.ParseFrom(node);
        ASSERT_EQUAL(request.request_id, 1965312327u);
        ASSERT_EQUAL(request.bus_name, "256");
    }
    {
        istringstream input(R"({
                                  "type": "Bus",
                                  "name": "750",
                                  "id": 519139350
                               })");

        Node node = Load(input).GetRoot();
        BusInfoRequest request;
        request.ParseFrom(node);
        ASSERT_EQUAL(request.request_id, 519139350u);
        ASSERT_EQUAL(request.bus_name, "750");
    }
    {
        istringstream input(R"({
                                  "type": "Bus",
                                  "name": "three word bus 750",
                                  "id": 2147483647
                               })");

        Node node = Load(input).GetRoot();
        BusInfoRequest request;
        request.ParseFrom(node);
        ASSERT_EQUAL(request.request_id, 2147483647u);
        ASSERT_EQUAL(request.bus_name, "three word bus 750");
    }
}

void TestStopInfoRequestParseFromJson() {
    {
        istringstream input(R"({
                                  "type": "Stop",
                                  "name": "Samara",
                                  "id": 746888088
                               })");

        Node node = Load(input).GetRoot();
        StopInfoRequest request;
        request.ParseFrom(node);
        ASSERT_EQUAL(request.request_id, 746888088u);
        ASSERT_EQUAL(request.stop_name, "Samara");

    }
    {
        istringstream input(R"({
                                  "type": "Stop",
                                  "name": "Prazhskaya",
                                  "id": 65100610
                               })");

        Node node = Load(input).GetRoot();
        StopInfoRequest request;
        request.ParseFrom(node);
        ASSERT_EQUAL(request.request_id, 65100610u);
        ASSERT_EQUAL(request.stop_name, "Prazhskaya");
    }
    {
        istringstream input(R"({
                                  "type": "Stop",
                                  "name": "three word stop",
                                  "id": 2147483647
                               })");

        Node node = Load(input).GetRoot();
        StopInfoRequest request;
        request.ParseFrom(node);
        ASSERT_EQUAL(request.request_id, 2147483647u);
        ASSERT_EQUAL(request.stop_name, "three word stop");
    }
}

void TestRouteInfoRequestParseFromJson() {
    {
        istringstream input(R"({
                                  "type": "Route",
                                  "from": "Biryulyovo Zapadnoye",
                                  "to": "Universam",
                                  "id": 4
                               })");

        Node node = Load(input).GetRoot();
        RouteInfoRequest request;
        request.ParseFrom(node);
        ASSERT_EQUAL(request.request_id, 4u);
        ASSERT_EQUAL(request.path.from, "Biryulyovo Zapadnoye");
        ASSERT_EQUAL(request.path.to, "Universam");
    }
}

void TestParseJsonRequest() {
    {
        istringstream input(R"({
                                  "type": "Bus",
                                  "name": "828",
                                  "stops": [
                                    "Biryulyovo Zapadnoye",
                                    "Universam",
                                    "Rossoshanskaya ulitsa",
                                    "Biryulyovo Zapadnoye"
                                  ],
                                  "is_roundtrip": true
                                })");

        Node node = Load(input).GetRoot();
        RequestPtr request = ParseRequest(node, true);
        ASSERT_EQUAL(request->type, Type::ADD_BUS);
    }
    {
        istringstream input(R"({
                                  "type": "Stop",
                                  "road_distances": {
                                    "Marushkino": 3900
                                  },
                                  "longitude": 37.20829,
                                  "name": "Tolstopaltsevo",
                                  "latitude": 55.611087
                               })");

        Node node = Load(input).GetRoot();
        RequestPtr request = ParseRequest(node, true);
        ASSERT_EQUAL(request->type, Type::ADD_STOP);
    }
    {
        istringstream input(R"({
                                  "type": "Bus",
                                  "name": "256",
                                  "id": 1965312327
                               })");
        Node node = Load(input).GetRoot();
        RequestPtr request = ParseRequest(node, false);
        ASSERT_EQUAL(request->type, Type::BUS_INFO);
    }
    {
        istringstream input(R"({
                                  "type": "Stop",
                                  "name": "Chernigovskay",
                                  "id": 1965312327
                               })");
        Node node = Load(input).GetRoot();
        RequestPtr request = ParseRequest(node, false);
        ASSERT_EQUAL(request->type, Type::STOP_INFO);
    }
}

void TestReadJsonRequests() {
    {
        istringstream input(R"({
                                 "routing_settings": {
                                      "bus_wait_time": 6,
                                      "bus_velocity": 40
                                   },
                                  "base_requests": [
                                    {
                                      "type": "Stop",
                                      "road_distances": {
                                        "Marushkino": 3900
                                      },
                                      "longitude": 37.20829,
                                      "name": "Tolstopaltsevo",
                                      "latitude": 55.611087
                                    },
                                    {
                                      "type": "Stop",
                                      "road_distances": {
                                        "Rasskazovka": 9900
                                      },
                                      "longitude": 37.209755,
                                      "name": "Marushkino",
                                      "latitude": 55.595884
                                    },
                                    {
                                      "type": "Bus",
                                      "name": "256",
                                      "stops": [
                                        "Biryulyovo Zapadnoye",
                                        "Biryusinka",
                                        "Universam",
                                        "Biryulyovo Tovarnaya",
                                        "Biryulyovo Passazhirskaya",
                                        "Biryulyovo Zapadnoye"
                                      ],
                                      "is_roundtrip": true
                                    }
                                  ],
                                  "stat_requests": [
                                    {
                                      "type": "Bus",
                                      "name": "256",
                                      "id": 1965312327
                                    },
                                    {
                                      "type": "Stop",
                                      "name": "Biryulyovo Zapadnoye",
                                      "id": 1042838872
                                    }
                                  ]
                               })");

        Document doc = Load(input);
        const auto requests = ReadRequests(doc.GetRoot());
        ASSERT_EQUAL(requests.size(), 6u);
        ASSERT_EQUAL(requests[0]->type, Type::ADD_ROUTE_SETTINGS);
        ASSERT_EQUAL(requests[1]->type, Type::ADD_STOP);
        ASSERT_EQUAL(requests[2]->type, Type::ADD_STOP);
        ASSERT_EQUAL(requests[3]->type, Type::ADD_BUS);
        ASSERT_EQUAL(requests[4]->type, Type::BUS_INFO);
        ASSERT_EQUAL(requests[5]->type, Type::STOP_INFO);
    }
}

void TestPrintJsonResponse() {
    {
        BusInfoResponse response(1965312327, {"256", 6, 5, 5950, 1.361239});
        ostringstream output;
        output.precision(6);
        response.PrintJson(output);
        ASSERT_EQUAL(output.str(), R"({"route_length": 5950, "request_id": 1965312327, "curvature": 1.36124, "stop_count": 6, "unique_stop_count": 5})");
    }
    {
        BusInfoResponse response(1965312327, {"750", 6, 5, 5950, 1.361239, "not found"});
        ostringstream output;
        output.precision(6);
        response.PrintJson(output);
        ASSERT_EQUAL(output.str(), R"({"request_id": 1965312327, "error_message": "not found"})");
    }
    {
        StopInfoResponse response(1042838872, {"256", set<string>({"256", "828"}), ""});
        ostringstream output;
        output.precision(6);
        response.PrintJson(output);
        ASSERT_EQUAL(output.str(), R"({"buses": ["256", "828"], "request_id": 1042838872})");
    }
    {
        StopInfoResponse response(1042838872, {"256", set<string>({}), ""});
        ostringstream output;
        output.precision(6);
        response.PrintJson(output);
        ASSERT_EQUAL(output.str(), R"({"buses": [], "request_id": 1042838872})");
    }
    {
        StopInfoResponse response(1042838872, {"256", set<string>({"256", "828"}), "not found"});
        ostringstream output;
        output.precision(6);
        response.PrintJson(output);
        ASSERT_EQUAL(output.str(), R"({"request_id": 1042838872, "error_message": "not found"})");
    }
    {
        using namespace Statistic;
        RouteItemPtr item1 = make_unique<WaitItem>("Biryulyovo", 6);
        RouteItemPtr item2 = make_unique<BusItem>("297", 2, 5.235);
        std::vector<RouteItemPtr> items(2);
        items[0] = move(item1);
        items[1] = move(item2);
        RouteInfoResponse response(1, {10.0, move(items), ""});
        ostringstream output;
        output.precision(6);
        response.PrintJson(output);
        ASSERT_EQUAL(output.str(), R"({"request_id": 1, "total_time": 10, "items": [{"type": "Wait", "stop_name": "Biryulyovo", "time": 6}, {"type": "Bus", "bus": "297", "span_count": 2, "time": 5.235}]})");
    }
    {
        RouteInfoResponse response(1, {0.0, {}, "not found"});
        ostringstream output;
        output.precision(6);
        response.PrintJson(output);
        ASSERT_EQUAL(output.str(), R"({"request_id": 1, "error_message": "not found"})");
    }
}

void TestPrintResponcesInJsonFormat() {
    {
        using namespace Statistic;
        Bus bus_stat = {"750", 6, 5, 5950, 1.361239, "not found"};
        Stop stop_stat = {"256", set<string>({"256", "828"}), ""};
        vector<ResponsePtr> responses(2);
        responses[0] = make_unique<BusInfoResponse>(1965312327, bus_stat);
        responses[1] = make_unique<StopInfoResponse>(1042838872, stop_stat);

        ostringstream output;
        PrintResponcesInJsonFormat(responses, output);
        ASSERT_EQUAL(output.str(), R"([{"request_id": 1965312327, "error_message": "not found"}, {"buses": ["256", "828"], "request_id": 1042838872}])");
    }
}

void TestPrintWaitItem() {
    {
        using namespace Statistic;
        WaitItem item = {"Biryulyovo", 6.0};
        ostringstream output;
        item.PrintJson(output);
        ASSERT_EQUAL(output.str(), R"({"type": "Wait", "stop_name": "Biryulyovo", "time": 6})");
    }
}

void TestPrintBusItem() {
    {
        using namespace Statistic;
        BusItem item = {"297", 2, 5.235};
        ostringstream output;
        item.PrintJson(output);
        ASSERT_EQUAL(output.str(), R"({"type": "Bus", "bus": "297", "span_count": 2, "time": 5.235})");
    }
}

void TestLengthBetweenCoords() {
    {
        Coordinates coord1 = {55.611087, 37.20829};
        Coordinates coord2 = {55.595884, 37.209755};
        double length = Distance(coord1, coord2);
        ASSERT(abs(length - 1692.99) < 0.01);
    }
    {
        Coordinates coord1 = {56.326797, 44.006516};
        Coordinates coord2 = {55.755819, 37.617644};
        double length = Distance(coord1, coord2);
        ASSERT(abs(length - 401726.39) < 0.01);
    }
}

void TestBusManager() {
    {
        BusManager manager;
        manager.AddBus({"256", {"Biryulyovo Zapadnoye",
                               "Biryusinka",
                               "Universam",
                               "Biryulyovo Zapadnoye"}});
        manager.AddBus({"750", {"Tolstopaltsevo",
                               "Marushkino",
                               "Rasskazovka",
                               "Marushkino",
                               "Tolstopaltsevo"}});
        ASSERT_EQUAL(manager.GetBus("256").stops, vector<string>({"Biryulyovo Zapadnoye",
                                                                  "Biryusinka",
                                                                  "Universam",
                                                                  "Biryulyovo Zapadnoye"}));
        ASSERT_EQUAL(manager.GetBus("750").stops, vector<string>({"Tolstopaltsevo",
                                                                  "Marushkino",
                                                                  "Rasskazovka",
                                                                  "Marushkino",
                                                                  "Tolstopaltsevo"}));
    }
    {
        BusManager manager;
        manager.AddBus({"256", {"1", "2", "3", "4", "3", "2", "1"}});
        manager.AddBus({"750", {"1", "2", "3", "4", "3", "2", "1"}});
        manager.AddBus({"111", {"4", "5", "6", "7", "6", "5", "4"}});
        ASSERT_EQUAL(manager.GetStopBuses("1"), set<string>({"256", "750"}));
        ASSERT_EQUAL(manager.GetStopBuses("4"), set<string>({"256", "750", "111"}));
    }
}

void TestStopManager() {
    {
        Coordinates coord1 = {55.611087, 37.20829};
        Coordinates coord2 = {55.595884, 37.209755};
        Coordinates coord3 = {55.632761, 37.333324};
        Coordinates coord4 = {55.574371, 37.6517};

        StopManager manager;
        manager.AddStop({"Tolstopaltsevo",       coord1, {}});
        manager.AddStop({"Marushkino",           coord2, {}});
        manager.AddStop({"Rasskazovka",          coord3, {}});
        manager.AddStop({"Biryulyovo Zapadnoye", coord4, {}});
        double length = manager.ComputeLength(vector<string>({"Tolstopaltsevo",
                                                              "Marushkino",
                                                              "Rasskazovka",
                                                              "Biryulyovo Zapadnoye"}));
        double result_length = Distance(coord1, coord2) +
                               Distance(coord2, coord3) +
                               Distance(coord3, coord4);
        ASSERT(abs(length - result_length) < 0.0000001);
    }
    {
        Coordinates coord1 = {55.611087, 37.20829};
        Coordinates coord2 = {55.595884, 37.209755};
        Coordinates coord3 = {55.632761, 37.333324};

        StopManager manager;
        manager.AddStop({"Tolstopaltsevo",       coord1, {{"Marushkino", 3900u}}});
        manager.AddStop({"Marushkino",           coord2, {{"Rasskazovka", 9900u},
                                                          {"Tolstopaltsevo", 3900u}}});
        manager.AddStop({"Rasskazovka",          coord3, {{"Marushkino", 9900u}}});
        size_t length = manager.ComputeRealLength(vector<string>({"Tolstopaltsevo",
                                                                  "Marushkino",
                                                                  "Rasskazovka",
                                                                  "Marushkino",
                                                                  "Tolstopaltsevo"}));
        ASSERT((length - 27600u) < 1);
    }
}

void TestRouteManager() {
    {
        BusManager bus_manager;
        bus_manager.AddBus({"297", {"Biryulyovo Zapadnoye",
                                    "Biryulyovo Tovarnaya",
                                    "Universam",
                                    "Biryulyovo Zapadnoye"}});
        bus_manager.AddBus({"635", {"Biryulyovo Tovarnaya",
                                    "Universam",
                                    "Prazhskaya",
                                    "Universam",
                                    "Biryulyovo Tovarnaya"}});

        StopManager stop_manager;
        stop_manager.AddStop({"Biryulyovo Zapadnoye", {}, {{"Biryulyovo Tovarnaya", 2600u}}});
        stop_manager.AddStop({"Universam", {}, {{"Prazhskaya", 4650u},
                                                {"Biryulyovo Tovarnaya", 1380u},
                                                {"Biryulyovo Zapadnoye", 2500u}}});
        stop_manager.AddStop({"Biryulyovo Tovarnaya", {}, {{"Universam", 890u}}});
        stop_manager.AddStop({"Prazhskaya", {}, {}});

        RouteManager manager(bus_manager, stop_manager, {6, 40});
        {
            Path path = {"Biryulyovo Zapadnoye", "Universam"};
            auto route_items = manager.GetRoute(path).value();
            ASSERT(abs(manager.GetRouteTime(path).value() - 11.235) < 0.0001);
            ASSERT_EQUAL(route_items.size(), 2u);
        }
        {
            Path path = {"Biryulyovo Zapadnoye", "Prazhskaya"};
            auto route_items = manager.GetRoute(path).value();
            ASSERT_EQUAL(manager.GetRouteTime(path).value(), 24.21);
            ASSERT_EQUAL(route_items.size(), 4u);

            auto wait_item_1 = dynamic_cast<Statistic::WaitItem*>(route_items[0].get());
            auto bus_item_2  = dynamic_cast<Statistic::BusItem*>(route_items[1].get());
            auto wait_item_3 = dynamic_cast<Statistic::WaitItem*>(route_items[2].get());
            auto bus_item_4  = dynamic_cast<Statistic::BusItem*>(route_items[3].get());

            ASSERT_EQUAL(*wait_item_1, Statistic::WaitItem("Biryulyovo Zapadnoye", 6.0));
            ASSERT_EQUAL(*bus_item_2,  Statistic::BusItem("297", 1, 3.9));
            ASSERT_EQUAL(*wait_item_3, Statistic::WaitItem("Biryulyovo Tovarnaya", 6.0));
            ASSERT_EQUAL(*bus_item_4,  Statistic::BusItem("635", 2, 8.31));

        }
    }
    {
        BusManager bus_manager;
        bus_manager.AddBus({"297", {"Biryulyovo Zapadnoye",
                                    "Biryulyovo Tovarnaya",
                                    "Universam",
                                    "Biryusinka",
                                    "Apteka",
                                    "Biryulyovo Zapadnoye"}});
        bus_manager.AddBus({"635", {"Biryulyovo Tovarnaya",
                                    "Universam",
                                    "Biryusinka",
                                    "TETs 26",
                                    "Pokrovskaya",
                                    "Prazhskaya",
                                    "Pokrovskaya",
                                    "TETs 26",
                                    "Biryusinka",
                                    "Universam",
                                    "Biryulyovo Tovarnaya"}});
        bus_manager.AddBus({"828", {"Biryulyovo Zapadnoye",
                                    "TETs 26",
                                    "Biryusinka",
                                    "Universam",
                                    "Pokrovskaya",
                                    "Rossoshanskaya ulitsa",
                                    "Pokrovskaya",
                                    "Universam",
                                    "Biryusinka",
                                    "TETs 26",
                                    "Biryulyovo Zapadnoye"}});
        bus_manager.AddBus({"750", {"Tolstopaltsevo",
                                    "Rasskazovka",
                                    "Tolstopaltsevo"}});

        StopManager stop_manager;
        stop_manager.AddStop({"Biryulyovo Zapadnoye", {}, {{"Biryulyovo Tovarnaya", 2600u},
                                                           {"TETs 26", 1100u}}});
        stop_manager.AddStop({"Universam", {}, {{"Biryusinka", 760u},
                                                {"Biryulyovo Tovarnaya", 1380u},
                                                {"Pokrovskaya", 2460u}}});
        stop_manager.AddStop({"Biryulyovo Tovarnaya", {}, {{"Universam", 890u}}});
        stop_manager.AddStop({"Biryusinka", {}, {{"Apteka", 210u},
                                                 {"TETs 26", 400u}}});
        stop_manager.AddStop({"Apteka", {}, {{"Biryulyovo Zapadnoye", 1420u}}});
        stop_manager.AddStop({"TETs 26", {}, {{"Pokrovskaya", 2850u}}});
        stop_manager.AddStop({"Pokrovskaya", {}, {{"Rossoshanskaya ulitsa", 3140u}}});
        stop_manager.AddStop({"Rossoshanskaya ulitsa", {}, {{"Pokrovskaya", 3210u}}});
        stop_manager.AddStop({"Prazhskaya", {}, {{"Pokrovskaya", 2260u}}});
        stop_manager.AddStop({"Tolstopaltsevo", {}, {{"Rasskazovka", 13800u}}});
        stop_manager.AddStop({"Rasskazovka", {}, {}});

        RouteManager manager(bus_manager, stop_manager, {2, 30});
        {
            Path path = {"Biryulyovo Zapadnoye", "Apteka"};
            auto route_items = manager.GetRoute(path).value();
            ASSERT_EQUAL(manager.GetRouteTime(path).value(), 7.42);
            ASSERT_EQUAL(route_items.size(), 4u);
        }
        {
            Path path = {"Biryulyovo Zapadnoye", "Pokrovskaya"};
            auto route_items = manager.GetRoute(path).value();
            ASSERT_EQUAL(manager.GetRouteTime(path).value(), 11.44);
            ASSERT_EQUAL(route_items.size(), 2u);
        }
        {
            Path path = {"Biryulyovo Tovarnaya", "Pokrovskaya"};
            auto route_items = manager.GetRoute(path).value();
            ASSERT_EQUAL(manager.GetRouteTime(path).value(), 10.7);
            ASSERT_EQUAL(route_items.size(), 4u);
        }
        {
            Path path = {"Biryulyovo Tovarnaya", "Biryulyovo Zapadnoye"};
            auto route_items = manager.GetRoute(path).value();
            ASSERT(abs(manager.GetRouteTime(path).value() - 8.56) < 0.0001);
            ASSERT_EQUAL(route_items.size(), 2u);
        }
        {
            Path path = {"Biryulyovo Tovarnaya", "Prazhskaya"};
            auto route_items = manager.GetRoute(path).value();
            ASSERT_EQUAL(manager.GetRouteTime(path).value(), 16.32);
            ASSERT_EQUAL(route_items.size(), 2u);
        }
        {
            Path path = {"Apteka", "Biryulyovo Tovarnaya"};
            auto route_items = manager.GetRoute(path).value();
            ASSERT_EQUAL(manager.GetRouteTime(path).value(), 12.04);
            ASSERT_EQUAL(route_items.size(), 4u);
        }
        {
            Path path = {"Biryulyovo Zapadnoye", "Tolstopaltsevo"};
            ASSERT(manager.GetRoute(path) == nullopt);
            ASSERT(manager.GetRouteTime(path) == nullopt);
        }
    }
    {
        BusManager bus_manager;
        bus_manager.AddBus({"289", {"Zagorye",
                                    "Lipetskaya ulitsa 46",
                                    "Lipetskaya ulitsa 40",
                                    "Lipetskaya ulitsa 40",
                                    "Lipetskaya ulitsa 46",
                                    "Moskvorechye",
                                    "Zagorye"}});
        StopManager stop_manager;
        stop_manager.AddStop({"Zagorye", {}, {{"Lipetskaya ulitsa 46", 230u}}});
        stop_manager.AddStop({"Lipetskaya ulitsa 46", {}, {{"Lipetskaya ulitsa 40", 390u},
                                                           {"Moskvorechye", 12400u}}});
        stop_manager.AddStop({"Lipetskaya ulitsa 40", {}, {{"Lipetskaya ulitsa 40", 1090u},
                                                           {"Lipetskaya ulitsa 46", 380u}}});
        stop_manager.AddStop({"Moskvorechye", {}, {{"Zagorye", 10000u}}});

        RouteManager manager(bus_manager, stop_manager, {2, 30});
        {
            Path path = {"Zagorye", "Moskvorechye"};
            auto route_items = manager.GetRoute(path).value();
            ASSERT_EQUAL(manager.GetRouteTime(path).value(), 29.26);
            ASSERT_EQUAL(route_items.size(), 4u);
        }
        {
            Path path = {"Moskvorechye", "Zagorye"};
            auto route_items = manager.GetRoute(path).value();
            ASSERT_EQUAL(manager.GetRouteTime(path).value(), 22);
            ASSERT_EQUAL(route_items.size(), 2u);
        }
        {
            Path path = {"Lipetskaya ulitsa 40", "Lipetskaya ulitsa 40"};
            auto route_items = manager.GetRoute(path).value();
            ASSERT_EQUAL(manager.GetRouteTime(path).value(), 0);
            ASSERT_EQUAL(route_items.size(), 0u);
        }
    }
}

void TestJsonPipeline() {
    ifstream input("tests/input_pipeline_d.json");
    ifstream result("tests/output_pipeline_d.json");
    string answer;
    getline(result, answer);

    Document doc = Load(input);
    const auto requests = ReadRequests(doc.GetRoot());
    ASSERT_EQUAL(requests.size(), 19u);

    TransportManager manager;
    vector<ResponsePtr> responses = ProcessRequests(manager, requests);
    ASSERT_EQUAL(responses.size(), 6u);

    ostringstream output;
    PrintResponcesInJsonFormat(responses, output);
    ASSERT_EQUAL(output.str(), answer);
}

void TestJsonPipelineE_1() {
    ifstream input("tests/input1.json");
    ifstream result("tests/output1.json");
    string answer;
    getline(result, answer);

    Document doc = Load(input);
    const auto requests = ReadRequests(doc.GetRoot());
    ASSERT_EQUAL(requests.size(), 12u);

    TransportManager manager;
    vector<ResponsePtr> responses = ProcessRequests(manager, requests);
    ASSERT_EQUAL(responses.size(), 5u);

    ostringstream output;
    PrintResponcesInJsonFormat(responses, output);
    ASSERT(CompareResponseJsonStrings(output.str(), answer));
}

void TestJsonPipelineE_2() {
    ifstream input("tests/input2.json");
    ifstream result("tests/output2.json");
    string answer;
    getline(result, answer);

    Document doc = Load(input);
    const auto requests = ReadRequests(doc.GetRoot());
    ASSERT_EQUAL(requests.size(), 27u);

    TransportManager manager;
    vector<ResponsePtr> responses = ProcessRequests(manager, requests);
    ASSERT_EQUAL(responses.size(), 11u);

    ostringstream output;
    PrintResponcesInJsonFormat(responses, output);
    ASSERT(CompareResponseJsonStrings(output.str(), answer));
}

void TestJsonPipelineE_3() {
    ifstream input("tests/input3.json");
    ifstream result("tests/output3.json");
    string answer;
    getline(result, answer);

    Document doc = Load(input);
    const auto requests = ReadRequests(doc.GetRoot());
    ASSERT_EQUAL(requests.size(), 10u);

    TransportManager manager;
    vector<ResponsePtr> responses = ProcessRequests(manager, requests);
    ASSERT_EQUAL(responses.size(), 4u);

    ostringstream output;
    PrintResponcesInJsonFormat(responses, output);
    ASSERT(CompareResponseJsonStrings(output.str(), answer));
}

void TestJsonPipelineE_4() {
    ifstream input("tests/input4.json");
    ifstream result("tests/output4.json");
    string answer;
    getline(result, answer);

    Document doc = Load(input);
    vector<RequestPtr> requests;
    {
        LOG_DURATION("TestJsonPipelineE_4 ReadRequests");
        requests = ReadRequests(doc.GetRoot());
        ASSERT_EQUAL(requests.size(), 2201u);
    }

    vector<ResponsePtr> responses;
    {
        LOG_DURATION("TestJsonPipelineE_4 ProcessRequests");
        TransportManager manager;
        responses = ProcessRequests(manager, requests);
        ASSERT_EQUAL(responses.size(), 2000u);
    }

    ostringstream output;
    PrintResponcesInJsonFormat(responses, output);
    ASSERT(CompareResponseJsonStrings(output.str(), answer));
}

int main() {
#ifdef LOCAL_BUILD
    TestRunner tr;
    RUN_TEST(tr, TestJson);
    RUN_TEST(tr, TestJsonCompare);
    RUN_TEST(tr, TestParseJsonRoutingSettings);
    RUN_TEST(tr, TestAddBusRequestParseFromJson);
    RUN_TEST(tr, TestAddStopRequestParseFromJson);
    RUN_TEST(tr, TestBusInfoRequestParseFromJson);
    RUN_TEST(tr, TestStopInfoRequestParseFromJson);
    RUN_TEST(tr, TestRouteInfoRequestParseFromJson);

    RUN_TEST(tr, TestParseJsonRequest);
    RUN_TEST(tr, TestReadJsonRequests);
    RUN_TEST(tr, TestPrintJsonResponse);
    RUN_TEST(tr, TestPrintResponcesInJsonFormat);
    RUN_TEST(tr, TestPrintWaitItem);
    RUN_TEST(tr, TestPrintBusItem);

    RUN_TEST(tr, TestLengthBetweenCoords);
    RUN_TEST(tr, TestBusManager);
    RUN_TEST(tr, TestStopManager);
    RUN_TEST(tr, TestRouteManager);

    RUN_TEST(tr, TestJsonPipeline);
    RUN_TEST(tr, TestJsonPipelineE_1);
    RUN_TEST(tr, TestJsonPipelineE_2);
    RUN_TEST(tr, TestJsonPipelineE_3);
    RUN_TEST(tr, TestJsonPipelineE_4);
#else
    Document doc = Load(cin);
    const auto requests = ReadRequests(doc.GetRoot());
    TransportManager manager;
    vector<ResponsePtr> responses = ProcessRequests(manager, requests);
    PrintResponcesInJsonFormat(responses);
#endif
    return 0;
}