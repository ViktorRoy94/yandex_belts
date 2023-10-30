#include "descriptions.h"
#include "json.h"
#include "requests.h"
#include "sphere.h"
#include "transport_catalog.h"
#include "utils.h"
#include "test_runner.h"

#include <iostream>
#include <fstream>
#include <sstream>

using namespace std;

bool CompareResponseJsonStrings(std::string str1, std::string str2) {
    istringstream input1(str1);
    istringstream input2(str2);
    Json::Document doc1 = Json::Load(input1);
    Json::Document doc2 = Json::Load(input2);

    const auto& arr1 = doc1.GetRoot().AsArray();
    const auto& arr2 = doc2.GetRoot().AsArray();

    bool result = arr1.size() == arr2.size();
    if (!result) return result;
    for (size_t i = 0; i < arr1.size(); i++) {
        if (arr1[i].AsMap().count("items") == 0) {
            result = result && Json::CompareNodes(arr1[i], arr2[i]);
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

void TestJsonPipeline() {
    istringstream input(R"({"routing_settings": {"bus_wait_time": 6,"bus_velocity": 40},"base_requests": [{"type": "Bus","name": "297","stops": ["Biryulyovo Zapadnoye","Biryulyovo Tovarnaya","Universam","Biryulyovo Zapadnoye"],"is_roundtrip": true},{"type": "Bus","name": "635","stops": ["Biryulyovo Tovarnaya","Universam","Prazhskaya"],"is_roundtrip": false},{"type": "Stop","road_distances": {"Biryulyovo Tovarnaya": 2600},"longitude": 37.6517,"name": "Biryulyovo Zapadnoye","latitude": 55.574371},{"type": "Stop","road_distances": {"Prazhskaya": 4650,"Biryulyovo Tovarnaya": 1380,"Biryulyovo Zapadnoye": 2500},"longitude": 37.645687,"name": "Universam","latitude": 55.587655},{"type": "Stop","road_distances": {"Universam": 890},"longitude": 37.653656,"name": "Biryulyovo Tovarnaya","latitude": 55.592028},{"type": "Stop","road_distances": {},"longitude": 37.603938,"name": "Prazhskaya","latitude": 55.611717}],"stat_requests": [{"type": "Bus","name": "297","id": 1},{"type": "Bus","name": "635","id": 2},{"type": "Stop","name": "Universam","id": 3},{"type": "Route","from": "Biryulyovo Zapadnoye","to": "Universam","id": 4},{"type": "Route","from": "Biryulyovo Zapadnoye","to": "Prazhskaya","id": 5}]})");
    string answer = R"([{"curvature": 1.42963,"unique_stop_count": 3,"stop_count": 4,"request_id": 1,"route_length": 5990},{"curvature": 1.30156,"unique_stop_count": 3,"stop_count": 5,"request_id": 2,"route_length": 11570},{"request_id": 3,"buses": ["297","635"]},{"total_time": 11.235,"items": [{"time": 6,"type": "Wait","stop_name": "Biryulyovo Zapadnoye"},{"span_count": 2,"bus": "297","type": "Bus","time": 5.235}],"request_id": 4},{"total_time": 24.21,"items": [{"time": 6,"type": "Wait","stop_name": "Biryulyovo Zapadnoye"},{"span_count": 2,"bus": "297","type": "Bus","time": 5.235},{"time": 6,"type": "Wait","stop_name": "Universam"},{"span_count": 1,"bus": "635","type": "Bus","time": 6.975}],"request_id": 5}])";

    const auto input_doc = Json::Load(input);
    const auto& input_map = input_doc.GetRoot().AsMap();

    auto requests = Descriptions::ReadDescriptions(input_map.at("base_requests").AsArray());
    ASSERT_EQUAL(requests.size(), 6u);

    const TransportCatalog db(requests, input_map.at("routing_settings").AsMap());
    auto responses = Requests::ProcessAll(db, input_map.at("stat_requests").AsArray());
    ASSERT_EQUAL(responses.size(), 5u);

    ostringstream output;
    Json::PrintValue(responses, output);
    ASSERT(CompareResponseJsonStrings(output.str(), answer));
}

int main() {
#ifdef LOCAL_BUILD
    TestRunner tr;
    RUN_TEST(tr, TestJsonPipeline);
#else
    const auto input_doc = Json::Load(cin);
    const auto& input_map = input_doc.GetRoot().AsMap();

    const TransportCatalog db(
                Descriptions::ReadDescriptions(input_map.at("base_requests").AsArray()),
                input_map.at("routing_settings").AsMap()
                );

    Json::PrintValue(Requests::ProcessAll(db, input_map.at("stat_requests").AsArray()), cout);
    cout << endl;
#endif
    return 0;
}
