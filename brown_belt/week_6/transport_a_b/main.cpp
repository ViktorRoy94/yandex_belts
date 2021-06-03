#include "request.h"
#include "test_runner.h"

using namespace std;

void TestAddBusRequestParseFrom() {
    {
        AddBusRequest request;
        request.ParseFrom("Bus 256: Biryulyovo Zapadnoye > Biryusinka > Universam > Biryulyovo Zapadnoye\n");
        ASSERT_EQUAL(request.bus_name, "256");
        ASSERT_EQUAL(request.is_circle_path, true);
        ASSERT_EQUAL(request.stops, vector<string>({"Biryulyovo Zapadnoye",
                                                    "Biryusinka",
                                                    "Universam",
                                                    "Biryulyovo Zapadnoye"}));

    }
    {
        AddBusRequest request;
        request.ParseFrom("Bus 750: Tolstopaltsevo - Marushkino - Rasskazovka ");
        ASSERT_EQUAL(request.bus_name, "750");
        ASSERT_EQUAL(request.is_circle_path, false);
        ASSERT_EQUAL(request.stops, vector<string>({"Tolstopaltsevo",
                                                    "Marushkino",
                                                    "Rasskazovka"}));

    }
}

void TestAddStopRequestParseFrom() {
    {
        AddStopRequest request;
        request.ParseFrom("Stop Tolstopaltsevo: 55.611087, 37.20829\n");
        ASSERT_EQUAL(request.stop_name, "Tolstopaltsevo");
        ASSERT_EQUAL(request.coords, Coordinates({55.611087, 37.20829}));

    }
    {
        AddStopRequest request;
        request.ParseFrom("Stop Marushkino: 55.595884, 37.209755 ");
        ASSERT_EQUAL(request.stop_name, "Marushkino");
        ASSERT_EQUAL(request.coords, Coordinates({55.595884, 37.209755}));
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
        manager.AddBus("256", {"Biryulyovo Zapadnoye",
                               "Biryusinka",
                               "Universam",
                               "Biryulyovo Zapadnoye"}, true);
        manager.AddBus("750", {"Tolstopaltsevo",
                               "Marushkino",
                               "Rasskazovka"}, false);
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
        manager.AddBus("256", {"1", "2", "3", "4"}, false);
        manager.AddBus("750", {"1", "2", "3", "4"}, false);
        manager.AddBus("111", {"4", "5", "6", "7"}, false);
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
        manager.AddStop("Tolstopaltsevo",       coord1);
        manager.AddStop("Marushkino",           coord2);
        manager.AddStop("Rasskazovka",          coord3);
        manager.AddStop("Biryulyovo Zapadnoye", coord4);
        double length = manager.ComputeLength(vector<string>({"Tolstopaltsevo",
                                                              "Marushkino",
                                                              "Rasskazovka",
                                                              "Biryulyovo Zapadnoye"}));
        double result_length = LengthBetwenCoords(coord1, coord2) +
                               LengthBetwenCoords(coord2, coord3) +
                               LengthBetwenCoords(coord3, coord4);
        ASSERT(abs(length - result_length) < 0.0000001);
    }
}

void TestPrintResponse() {
    {
        BusInfoResponse response({"256", 6, 5, 4371.02});
        ostringstream output;
        response.Print(output);
        ASSERT_EQUAL(output.str(), "Bus 256: 6 stops on route, 5 unique stops, 4371.02 route length\n");
    }
    {
        StopInfoResponse response({"256", true, set<string>({"111", "121"})});
        ostringstream output;
        response.Print(output);
        ASSERT_EQUAL(output.str(), "Stop 256: buses 111 121\n");
    }
    {
        StopInfoResponse response({"256", true, {}});
        ostringstream output;
        response.Print(output);
        ASSERT_EQUAL(output.str(), "Stop 256: no buses\n");
    }
    {
        StopInfoResponse response({"256", false, {}});
        ostringstream output;
        response.Print(output);
        ASSERT_EQUAL(output.str(), "Stop 256: not found\n");
    }
}

void TestPipeline() {
    istringstream input("13\n"
                        "Stop Tolstopaltsevo: 55.611087, 37.20829\n"
                        "Stop Marushkino: 55.595884, 37.209755\n"
                        "Bus 256: Biryulyovo Zapadnoye > Biryusinka > Universam > Biryulyovo Tovarnaya > Biryulyovo Passazhirskaya > Biryulyovo Zapadnoye\n"
                        "Bus 750: Tolstopaltsevo - Marushkino - Rasskazovka\n"
                        "Stop Rasskazovka: 55.632761, 37.333324\n"
                        "Stop Biryulyovo Zapadnoye: 55.574371, 37.6517\n"
                        "Stop Biryusinka: 55.581065, 37.64839\n"
                        "Stop Universam: 55.587655, 37.645687\n"
                        "Stop Biryulyovo Tovarnaya: 55.592028, 37.653656\n"
                        "Stop Biryulyovo Passazhirskaya: 55.580999, 37.659164\n"
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

    TransportManager manager;
    vector<ResponsePtr> update_responses = ProcessRequests(manager, update_requests);
    vector<ResponsePtr> read_responses = ProcessRequests(manager, read_requests);
    ASSERT_EQUAL(update_responses.size(), 0u);
    ASSERT_EQUAL(read_responses.size(), 6u);

    ostringstream output;
    for (const auto& response : read_responses) {
        response->Print(output);
    }
    ASSERT_EQUAL(output.str(), "Bus 256: 6 stops on route, 5 unique stops, 4371.02 route length\n"
                               "Bus 750: 5 stops on route, 3 unique stops, 20939.5 route length\n"
                               "Bus 751: not found\n"
                               "Stop Samara: not found\n"
                               "Stop Prazhskaya: no buses\n"
                               "Stop Biryulyovo Zapadnoye: buses 256 828\n");
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
    RUN_TEST(tr, TestLengthBetweenCoords);
    RUN_TEST(tr, TestBusManager);
    RUN_TEST(tr, TestStopManager);
    RUN_TEST(tr, TestPrintResponse);

    RUN_TEST(tr, TestPipeline);
#else

    const auto update_requests = ReadRequests();
    const auto read_requests = ReadRequests();

    TransportManager manager;
    vector<ResponsePtr> update_responses = ProcessRequests(manager, update_requests);
    vector<ResponsePtr> read_responses = ProcessRequests(manager, read_requests);

    for (const auto& response : read_responses) {
        response->Print();
    }
#endif
    return 0;
}