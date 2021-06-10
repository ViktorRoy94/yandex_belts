#include "request.h"
#include "test_runner.h"
#include "profile.h"

#include <fstream>

using namespace std;
using namespace Json;

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
            result = result && time_diff < 0.01;
        }
    }
    return result;
}

void TestAddBusRequestParseFrom() {
    {
        AddBusRequest request;
        request.ParseFrom("Bus 256: Biryulyovo Zapadnoye > Biryusinka > Universam > Biryulyovo Zapadnoye\n");
        ASSERT_EQUAL(request.bus.name, "256");
        ASSERT_EQUAL(request.bus.is_circle_path, true);
        ASSERT_EQUAL(request.bus.stops, vector<string>({"Biryulyovo Zapadnoye",
                                                    "Biryusinka",
                                                    "Universam",
                                                    "Biryulyovo Zapadnoye"}));
    }
    {
        AddBusRequest request;        request.ParseFrom("Bus 750: Tolstopaltsevo - Marushkino - Rasskazovka ");
        ASSERT_EQUAL(request.bus.name, "750");
        ASSERT_EQUAL(request.bus.is_circle_path, false);
        ASSERT_EQUAL(request.bus.stops, vector<string>({"Tolstopaltsevo",
                                                    "Marushkino",
                                                    "Rasskazovka"}));
    }
}

void TestAddStopRequestParseFrom() {
    {
        AddStopRequest request;
        request.ParseFrom("Stop Tolstopaltsevo: 55.611087, 37.20829\n");
        ASSERT_EQUAL(request.stop.name, "Tolstopaltsevo");
        ASSERT_EQUAL(request.stop.coords, Coordinates({55.611087, 37.20829}));
    }
    {
        AddStopRequest request;
        request.ParseFrom("Stop Marushkino: 55.595884, 37.209755 ");
        ASSERT_EQUAL(request.stop.name, "Marushkino");
        ASSERT_EQUAL(request.stop.coords, Coordinates({55.595884, 37.209755}));
    }
    {
        AddStopRequest request;
        request.ParseFrom("Stop Tolstopaltsevo: 55.611087, 37.20829, 3900m to Marushkino\n");
        ASSERT_EQUAL(request.stop.name, "Tolstopaltsevo");
        ASSERT_EQUAL(request.stop.coords, Coordinates({55.611087, 37.20829}));
        ASSERT_EQUAL(request.stop.distances[0].first, "Marushkino");
        ASSERT_EQUAL(request.stop.distances[0].second, 3900u);
    }
    {
        AddStopRequest request;
        request.ParseFrom("Stop Biryulyovo Zapadnoye: 55.574371, 37.6517, 7500m to Rossoshanskaya ulitsa, 1800m to Biryusinka, 2400m to Universam\n");
        ASSERT_EQUAL(request.stop.name, "Biryulyovo Zapadnoye");
        ASSERT_EQUAL(request.stop.coords, Coordinates({55.574371, 37.6517}));
        ASSERT_EQUAL(request.stop.distances[0].first, "Rossoshanskaya ulitsa");
        ASSERT_EQUAL(request.stop.distances[0].second, 7500u);
        ASSERT_EQUAL(request.stop.distances[1].first, "Biryusinka");
        ASSERT_EQUAL(request.stop.distances[1].second, 1800u);
        ASSERT_EQUAL(request.stop.distances[2].first, "Universam");
        ASSERT_EQUAL(request.stop.distances[2].second, 2400u);
    }
}

void TestBusInfoRequestParseFrom() {
    {
        BusInfoRequest request;
        request.ParseFrom("Bus 256");
        ASSERT_EQUAL(request.bus_name, "256");
    }
    {
        BusInfoRequest request;
        request.ParseFrom("Bus 750\n");
        ASSERT_EQUAL(request.bus_name, "750");
    }
    {
        BusInfoRequest request;
        request.ParseFrom("Bus three word bus 750\n");
        ASSERT_EQUAL(request.bus_name, "three word bus 750");
    }
}

void TestStopInfoRequestParseFrom() {
    {
        StopInfoRequest request;
        request.ParseFrom("Stop Biryulyovo Zapadnoye");
        ASSERT_EQUAL(request.stop_name, "Biryulyovo Zapadnoye");
    }
    {
        StopInfoRequest request;
        request.ParseFrom("Stop Prazhskaya\n");
        ASSERT_EQUAL(request.stop_name, "Prazhskaya");
    }
    {
        StopInfoRequest request;
        request.ParseFrom("Stop three word stop\n");
        ASSERT_EQUAL(request.stop_name, "three word stop");
    }
}

void TestParseRequest() {
    {
        string request_str = "Bus 256: Biryulyovo Zapadnoye > Biryusinka > Universam";
        RequestPtr request = ParseRequest(request_str);
        ASSERT_EQUAL(request->type, Request::Type::ADD_BUS);
    }
    {
        string request_str = "Stop Tolstopaltsevo: 55.611087, 37.20829";
        RequestPtr request = ParseRequest(request_str);
        ASSERT_EQUAL(request->type, Request::Type::ADD_STOP);
    }
    {
        string request_str = "Bus 256";
        RequestPtr request = ParseRequest(request_str);
        ASSERT_EQUAL(request->type, Request::Type::BUS_INFO);
    }
    {
        string request_str = "Stop Tolstopaltsevo";
        RequestPtr request = ParseRequest(request_str);
        ASSERT_EQUAL(request->type, Request::Type::STOP_INFO);
    }
}

void TestReadRequests() {
    {
        istringstream input("3\n"
                            "Stop Tolstopaltsevo: 55.611087, 37.20829\n"
                            "Stop Marushkino: 55.595884, 37.209755\n"
                            "Bus 750: Tolstopaltsevo - Marushkino - Rasskazovka\n"
                            "3\n"
                            "Bus 256\n"
                            "Bus 750\n"
                            "Bus 751\n");
        const auto requests = ReadRequests(input);
        ASSERT_EQUAL(requests.size(), 3u);
        ASSERT_EQUAL(requests[0]->type, Request::Type::ADD_STOP);
        ASSERT_EQUAL(requests[1]->type, Request::Type::ADD_STOP);
        ASSERT_EQUAL(requests[2]->type, Request::Type::ADD_BUS);
    }
}

void TestPrintResponse() {
    {
        BusInfoResponse response(0, {"256", 6, 5, 5950, 1.361239});
        ostringstream output;
        output.precision(7);
        response.Print(output);
        ASSERT_EQUAL(output.str(), "Bus 256: 6 stops on route, 5 unique stops, 5950 route length, 1.361239 curvature\n");
    }
    {
        StopInfoResponse response(1, {"256", set<string>({"111", "121"}), ""});
        ostringstream output;
        response.Print(output);
        ASSERT_EQUAL(output.str(), "Stop 256: buses 111 121\n");
    }
    {
        StopInfoResponse response(2, {"256", {}, ""});
        ostringstream output;
        response.Print(output);
        ASSERT_EQUAL(output.str(), "Stop 256: no buses\n");
    }
    {
        StopInfoResponse response(3, {"256", {}, "not found"});
        ostringstream output;
        response.Print(output);
        ASSERT_EQUAL(output.str(), "Stop 256: not found\n");
    }
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
    RoutingSettings settings = ReadJsonRoutingSettings(node);
    ASSERT_EQUAL(settings.bus_wait_time, 6);
    ASSERT(abs(settings.bus_velocity - 666.66666) < 0.0001);
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
        request.ParseFromJson(node);
        ASSERT_EQUAL(request.bus.name, "828");
        ASSERT_EQUAL(request.bus.is_circle_path, true);
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
        request.ParseFromJson(node);
        ASSERT_EQUAL(request.bus.name, "750");
        ASSERT_EQUAL(request.bus.is_circle_path, false);
        ASSERT_EQUAL(request.bus.stops, vector<string>({"Tolstopaltsevo",
                                                        "Marushkino",
                                                        "Rasskazovka"}));
    }
    {
        istringstream input(R"({ "type": "Bus", "name": "750", "stops": [ "Tolstopaltsevo", "Marushkino", "Rasskazovka" ], "is_roundtrip": false })");

        Node node = Load(input).GetRoot();
        AddBusRequest request;
        request.ParseFromJson(node);
        ASSERT_EQUAL(request.bus.name, "750");
        ASSERT_EQUAL(request.bus.is_circle_path, false);
        ASSERT_EQUAL(request.bus.stops, vector<string>({"Tolstopaltsevo",
                                                        "Marushkino",
                                                        "Rasskazovka"}));
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
        request.ParseFromJson(node);
        ASSERT_EQUAL(request.stop.name, "Tolstopaltsevo");
        ASSERT_EQUAL(request.stop.coords, Coordinates({55.611087, 37.20829}));
        ASSERT_EQUAL(request.stop.distances[0].first, "Marushkino");
        ASSERT_EQUAL(request.stop.distances[0].second, 3900u);
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
        request.ParseFromJson(node);
        ASSERT_EQUAL(request.stop.name, "Marushkino");
        ASSERT_EQUAL(request.stop.coords, Coordinates({55.595884, 37.209755}));
        ASSERT_EQUAL(request.stop.distances[0].first, "Rasskazovka");
        ASSERT_EQUAL(request.stop.distances[0].second, 9900u);
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
        request.ParseFromJson(node);
        ASSERT_EQUAL(request.stop.name, "Universam");
        ASSERT_EQUAL(request.stop.coords, Coordinates({55.587655, 37.645687}));
        ASSERT_EQUAL(request.stop.distances[0].first, "Biryulyovo Tovarnaya");
        ASSERT_EQUAL(request.stop.distances[0].second, 900u);
        ASSERT_EQUAL(request.stop.distances[1].first, "Rossoshanskaya ulitsa");
        ASSERT_EQUAL(request.stop.distances[1].second, 5600u);
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
        request.ParseFromJson(node);
        ASSERT_EQUAL(request.stop.name, "Rasskazovka");
        ASSERT_EQUAL(request.stop.coords, Coordinates({55.632761, 37.333324}));
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
        request.ParseFromJson(node);
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
        request.ParseFromJson(node);
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
        request.ParseFromJson(node);
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
        request.ParseFromJson(node);
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
        request.ParseFromJson(node);
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
        request.ParseFromJson(node);
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
        request.ParseFromJson(node);
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
        RequestPtr request = ParseJsonRequest(node, true);
        ASSERT_EQUAL(request->type, Request::Type::ADD_BUS);
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
        RequestPtr request = ParseJsonRequest(node, true);
        ASSERT_EQUAL(request->type, Request::Type::ADD_STOP);
    }
    {
        istringstream input(R"({
                                  "type": "Bus",
                                  "name": "256",
                                  "id": 1965312327
                               })");
        Node node = Load(input).GetRoot();
        RequestPtr request = ParseJsonRequest(node, false);
        ASSERT_EQUAL(request->type, Request::Type::BUS_INFO);
    }
    {
        istringstream input(R"({
                                  "type": "Stop",
                                  "name": "Chernigovskay",
                                  "id": 1965312327
                               })");
        Node node = Load(input).GetRoot();
        RequestPtr request = ParseJsonRequest(node, false);
        ASSERT_EQUAL(request->type, Request::Type::STOP_INFO);
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
        const auto requests = ReadJsonRequests(doc.GetRoot());
        ASSERT_EQUAL(requests.size(), 6u);
        ASSERT_EQUAL(requests[0]->type, Request::Type::ADD_ROUTE_SETTINGS);
        ASSERT_EQUAL(requests[1]->type, Request::Type::ADD_STOP);
        ASSERT_EQUAL(requests[2]->type, Request::Type::ADD_STOP);
        ASSERT_EQUAL(requests[3]->type, Request::Type::ADD_BUS);
        ASSERT_EQUAL(requests[4]->type, Request::Type::BUS_INFO);
        ASSERT_EQUAL(requests[5]->type, Request::Type::STOP_INFO);
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
        BusStat bus_stat = {"750", 6, 5, 5950, 1.361239, "not found"};
        StopStat stop_stat = {"256", set<string>({"256", "828"}), ""};
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
        WaitItem item = {"Biryulyovo", 6.0};
        ostringstream output;
        item.PrintJson(output);
        ASSERT_EQUAL(output.str(), R"({"type": "Wait", "stop_name": "Biryulyovo", "time": 6})");
    }
}

void TestPrintBusItem() {
    {
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
        double length = LengthBetwenCoords(coord1, coord2);
        ASSERT(abs(length - 1692.99) < 0.01);
    }
    {
        Coordinates coord1 = {56.326797, 44.006516};
        Coordinates coord2 = {55.755819, 37.617644};
        double length = LengthBetwenCoords(coord1, coord2);
        ASSERT(abs(length - 401726.39) < 0.01);
    }
}

void TestBusManager() {
    {
        BusManager manager;
        manager.AddBus({"256", {"Biryulyovo Zapadnoye",
                               "Biryusinka",
                               "Universam",
                               "Biryulyovo Zapadnoye"}, true});
        manager.AddBus({"750", {"Tolstopaltsevo",
                               "Marushkino",
                               "Rasskazovka"}, false});
        ASSERT_EQUAL(manager.GetBusStops("256"), vector<string>({"Biryulyovo Zapadnoye",
                                                                 "Biryusinka",
                                                                 "Universam",
                                                                 "Biryulyovo Zapadnoye"}));
        ASSERT_EQUAL(manager.GetBusStops("750"), vector<string>({"Tolstopaltsevo",
                                                                 "Marushkino",
                                                                 "Rasskazovka",
                                                                 "Marushkino",
                                                                 "Tolstopaltsevo"}));
    }
    {
        BusManager manager;
        manager.AddBus({"256", {"1", "2", "3", "4"}, false});
        manager.AddBus({"750", {"1", "2", "3", "4"}, false});
        manager.AddBus({"111", {"4", "5", "6", "7"}, false});
        ASSERT_EQUAL(manager.GetBusesForStop("1"), set<string>({"256", "750"}));
        ASSERT_EQUAL(manager.GetBusesForStop("4"), set<string>({"256", "750", "111"}));
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
        double result_length = LengthBetwenCoords(coord1, coord2) +
                               LengthBetwenCoords(coord2, coord3) +
                               LengthBetwenCoords(coord3, coord4);
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
        ASSERT(labs(length - 27600) < 1);
    }
}

void TestRouteManager() {
    {
        BusManager bus_manager;
        bus_manager.AddBus({"297", {"Biryulyovo Zapadnoye",
                                    "Biryulyovo Tovarnaya",
                                    "Universam",
                                    "Biryulyovo Zapadnoye"}, true});
        bus_manager.AddBus({"635", {"Biryulyovo Tovarnaya",
                                    "Universam",
                                    "Prazhskaya"}, false});

        StopManager stop_manager;
        stop_manager.AddStop({"Biryulyovo Zapadnoye", {}, {{"Biryulyovo Tovarnaya", 2600u}}});
        stop_manager.AddStop({"Universam", {}, {{"Prazhskaya", 4650u},
                                                {"Biryulyovo Tovarnaya", 1380u},
                                                {"Biryulyovo Zapadnoye", 2500u}}});
        stop_manager.AddStop({"Biryulyovo Tovarnaya", {}, {{"Universam", 890u}}});
        stop_manager.AddStop({"Prazhskaya", {}, {}});

        RouteManager manager(bus_manager, stop_manager);
        manager.UpdateRouteSettings(RoutingSettings(6, 40));
        manager.Initialize();
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

            WaitItem* wait_item_1 = dynamic_cast<WaitItem*>(route_items[0].get());
            BusItem* bus_item_2 = dynamic_cast<BusItem*>(route_items[1].get());
            WaitItem* wait_item_3 = dynamic_cast<WaitItem*>(route_items[2].get());
            BusItem* bus_item_4 = dynamic_cast<BusItem*>(route_items[3].get());

            ASSERT_EQUAL(*wait_item_1, WaitItem("Biryulyovo Zapadnoye", 6.0));
            ASSERT_EQUAL(*bus_item_2, BusItem("297", 2, 5.235));
            ASSERT_EQUAL(*wait_item_3, WaitItem("Universam", 6.0));
            ASSERT_EQUAL(*bus_item_4, BusItem("635", 1, 6.975));
        }
    }
    {
        BusManager bus_manager;
        bus_manager.AddBus({"297", {"Biryulyovo Zapadnoye",
                                    "Biryulyovo Tovarnaya",
                                    "Universam",
                                    "Biryusinka",
                                    "Apteka",
                                    "Biryulyovo Zapadnoye"}, true});
        bus_manager.AddBus({"635", {"Biryulyovo Tovarnaya",
                                    "Universam",
                                    "Biryusinka",
                                    "TETs 26",
                                    "Pokrovskaya",
                                    "Prazhskaya"}, false});
        bus_manager.AddBus({"828", {"Biryulyovo Zapadnoye",
                                    "TETs 26",
                                    "Biryusinka",
                                    "Universam",
                                    "Pokrovskaya",
                                    "Rossoshanskaya ulitsa"}, false});
        bus_manager.AddBus({"750", {"Tolstopaltsevo",
                                    "Rasskazovka"}, false});

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

        RouteManager manager(bus_manager, stop_manager);
        manager.UpdateRouteSettings(RoutingSettings(2, 30));
        manager.Initialize();
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
            ASSERT_EQUAL(manager.GetRouteTime(path).value(), 8.56);
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
                                    "Zagorye"}, true});
        StopManager stop_manager;
        stop_manager.AddStop({"Zagorye", {}, {{"Lipetskaya ulitsa 46", 230u}}});
        stop_manager.AddStop({"Lipetskaya ulitsa 46", {}, {{"Lipetskaya ulitsa 40", 390u},
                                                           {"Moskvorechye", 12400u}}});
        stop_manager.AddStop({"Lipetskaya ulitsa 40", {}, {{"Lipetskaya ulitsa 40", 1090u},
                                                           {"Lipetskaya ulitsa 46", 380u}}});
        stop_manager.AddStop({"Moskvorechye", {}, {{"Zagorye", 10000u}}});

        RouteManager manager(bus_manager, stop_manager);
        manager.UpdateRouteSettings(RoutingSettings(2, 30));
        manager.Initialize();
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

void TestPipeline() {
    istringstream input("13\n"
                        "Stop Tolstopaltsevo: 55.611087, 37.20829, 3900m to Marushkino\n"
                        "Stop Marushkino: 55.595884, 37.209755, 9900m to Rasskazovka\n"
                        "Bus 256: Biryulyovo Zapadnoye > Biryusinka > Universam > Biryulyovo Tovarnaya > Biryulyovo Passazhirskaya > Biryulyovo Zapadnoye\n"
                        "Bus 750: Tolstopaltsevo - Marushkino - Rasskazovka\n"
                        "Stop Rasskazovka: 55.632761, 37.333324\n"
                        "Stop Biryulyovo Zapadnoye: 55.574371, 37.6517, 7500m to Rossoshanskaya ulitsa, 1800m to Biryusinka, 2400m to Universam\n"
                        "Stop Biryusinka: 55.581065, 37.64839, 750m to Universam\n"
                        "Stop Universam: 55.587655, 37.645687, 5600m to Rossoshanskaya ulitsa, 900m to Biryulyovo Tovarnaya\n"
                        "Stop Biryulyovo Tovarnaya: 55.592028, 37.653656, 1300m to Biryulyovo Passazhirskaya\n"
                        "Stop Biryulyovo Passazhirskaya: 55.580999, 37.659164, 1200m to Biryulyovo Zapadnoye\n"
                        "Bus 828: Biryulyovo Zapadnoye > Universam > Rossoshanskaya ulitsa > Biryulyovo Zapadnoye\n"
                        "Stop Rossoshanskaya ulitsa: 55.595579, 37.605757\n"
                        "Stop Prazhskaya: 55.611678, 37.603831\n"
                        "6\n"
                        "Bus 256\n"
                        "Bus 750\n"
                        "Bus 751\n"
                        "Stop Samara\n"
                        "Stop Prazhskaya\n"
                        "Stop Biryulyovo Zapadnoye\n");
    const auto update_requests = ReadRequests(input);
    const auto read_requests = ReadRequests(input);
    ASSERT_EQUAL(update_requests.size(), 13u);
    ASSERT_EQUAL(read_requests.size(), 6u);

    Server manager;
    vector<ResponsePtr> update_responses = ProcessRequests(manager, update_requests);
    vector<ResponsePtr> read_responses = ProcessRequests(manager, read_requests);
    ASSERT_EQUAL(update_responses.size(), 0u);
    ASSERT_EQUAL(read_responses.size(), 6u);

    ostringstream output;
    PrintResponces(read_responses, output);
    ASSERT_EQUAL(output.str(), "Bus 256: 6 stops on route, 5 unique stops, 5950 route length, 1.361239 curvature\n"
                               "Bus 750: 5 stops on route, 3 unique stops, 27600 route length, 1.318084 curvature\n"
                               "Bus 751: not found\n"
                               "Stop Samara: not found\n"
                               "Stop Prazhskaya: no buses\n"
                               "Stop Biryulyovo Zapadnoye: buses 256 828\n");
}

void TestJsonPipeline() {
    ifstream input("tests/input_pipeline_d.json");
    ifstream result("tests/output_pipeline_d.json");
    string answer;
    getline(result, answer);

    Document doc = Load(input);
    const auto requests = ReadJsonRequests(doc.GetRoot());
    ASSERT_EQUAL(requests.size(), 19u);

    Server manager;
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
    const auto requests = ReadJsonRequests(doc.GetRoot());
    ASSERT_EQUAL(requests.size(), 12u);

    Server manager;
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
    const auto requests = ReadJsonRequests(doc.GetRoot());
    ASSERT_EQUAL(requests.size(), 27u);

    Server manager;
    vector<ResponsePtr> responses = ProcessRequests(manager, requests);
    ASSERT_EQUAL(responses.size(), 11u);

    ostringstream output;
    PrintResponcesInJsonFormat(responses, output);
    ASSERT(CompareResponseJsonStrings(output.str(), answer));
//    ASSERT_EQUAL(output.str(), answer);
}

void TestJsonPipelineE_3() {
    ifstream input("tests/input3.json");
    ifstream result("tests/output3.json");
    string answer;
    getline(result, answer);

    Document doc = Load(input);
    const auto requests = ReadJsonRequests(doc.GetRoot());
    ASSERT_EQUAL(requests.size(), 10u);

    Server manager;
    vector<ResponsePtr> responses = ProcessRequests(manager, requests);
    ASSERT_EQUAL(responses.size(), 4u);

    ostringstream output;
    PrintResponcesInJsonFormat(responses, output);
    ASSERT(CompareResponseJsonStrings(output.str(), answer));
//    ASSERT_EQUAL(output.str(), answer);
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
        requests = ReadJsonRequests(doc.GetRoot());
        ASSERT_EQUAL(requests.size(), 2201u);
    }

    vector<ResponsePtr> responses;
    {
        LOG_DURATION("TestJsonPipelineE_4 ProcessRequests");
        Server manager;
        responses = ProcessRequests(manager, requests);
        ASSERT_EQUAL(responses.size(), 2000u);
    }

    ostringstream output;
    PrintResponcesInJsonFormat(responses, output);
    ASSERT(CompareResponseJsonStrings(output.str(), answer));
//    ASSERT_EQUAL(output.str(), answer);
}

int main() {
#ifdef LOCAL_BUILD
    TestRunner tr;
    RUN_TEST(tr, TestAddBusRequestParseFrom);
    RUN_TEST(tr, TestAddStopRequestParseFrom);
    RUN_TEST(tr, TestBusInfoRequestParseFrom);
    RUN_TEST(tr, TestStopInfoRequestParseFrom);
    RUN_TEST(tr, TestParseRequest);
    RUN_TEST(tr, TestReadRequests);
    RUN_TEST(tr, TestPrintResponse);

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

    RUN_TEST(tr, TestPipeline);
    RUN_TEST(tr, TestJsonPipeline);
    RUN_TEST(tr, TestJsonPipelineE_1);
    RUN_TEST(tr, TestJsonPipelineE_2);
    RUN_TEST(tr, TestJsonPipelineE_3);
    RUN_TEST(tr, TestJsonPipelineE_4);
#else
    Document doc = Load(cin);
    const auto requests = ReadJsonRequests(doc.GetRoot());
    Server manager;
    vector<ResponsePtr> responses = ProcessRequests(manager, requests);
    PrintResponcesInJsonFormat(responses);
#endif
    return 0;
}