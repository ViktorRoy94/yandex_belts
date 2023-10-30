#include "descriptions.h"
#include "json.h"
#include "requests.h"
#include "sphere.h"
#include "transport_catalog.h"
#include "utils.h"
#include "svg.h"
#include "test_runner.h"
#include "profile.h"

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
            result = result && time_diff < 0.01;
            if (!result)
                return result;
        }
    }
    return result;
}

void RunPipeline(istream& input, ostream& output) {
    const auto input_doc = Json::Load(input);
    const auto& input_map = input_doc.GetRoot().AsMap();

    const Json::Array& base_requests = input_map.at("base_requests").AsArray();
    const Json::Array& stat_requests = input_map.at("stat_requests").AsArray();
    const Json::Dict& routing_settings = input_map.at("routing_settings").AsMap();
    Json::Dict render_settings = Json::Dict();
    if (input_map.count("render_settings") > 0) {
        render_settings = input_map.at("render_settings").AsMap();
    }

    auto requests = Descriptions::ReadDescriptions(base_requests);
    const TransportCatalog db(requests, routing_settings, render_settings);

    auto responses = Requests::ProcessAll(db, stat_requests);
    Json::PrintValue(responses, output);
}

void TestTransportMap() {
    {
        ifstream input("tests/transport_g_input_1.json");
        ifstream result("tests/transport_g_output_1.json");

        const auto input_doc = Json::Load(input);
        const auto& input_map = input_doc.GetRoot().AsMap();

        const Json::Array& base_requests = input_map.at("base_requests").AsArray();
        const Json::Dict& routing_settings = input_map.at("routing_settings").AsMap();
        Json::Dict render_settings = Json::Dict();
        if (input_map.count("render_settings") > 0) {
            render_settings = input_map.at("render_settings").AsMap();
        }

        auto requests = Descriptions::ReadDescriptions(base_requests);
        const TransportCatalog db(requests, routing_settings, render_settings);

//        const auto output_svg = Json::Load(result).GetRoot().AsArray()[2].AsMap().at("map").AsString();
        ASSERT_EQUAL(db.RenderMap(), R"~(<?xml version="1.0" encoding="UTF-8" ?><svg xmlns="http://www.w3.org/2000/svg" version="1.1"><polyline points="202.7046433700865,725.1651507897237 99.25163555140999,520.6462422211027 202.7046433700865,725.1651507897237" fill="none" stroke="green" stroke-width="14" stroke-linecap="round" stroke-linejoin="round" /><polyline points="1150.0,432.1126535822507 580.955800223462,137.79639380890507 99.25163555140999,520.6462422211027 491.26376256587264,861.7217169297415 592.7509175043582,846.6267751715245 690.4467847454753,819.3856709748779 695.2561033988173,598.1921174405119 1150.0,432.1126535822507" fill="none" stroke="rgb(255,160,0)" stroke-width="14" stroke-linecap="round" stroke-linejoin="round" /><polyline points="695.2561033988173,598.1921174405119 660.3973192915555,441.8014999201921 580.955800223462,137.79639380890507 50.0,50.0 580.955800223462,137.79639380890507 660.3973192915555,441.8014999201921 695.2561033988173,598.1921174405119" fill="none" stroke="red" stroke-width="14" stroke-linecap="round" stroke-linejoin="round" /><circle cx="491.26376256587264" cy="861.7217169297415" r="5" fill="white" stroke="none" stroke-width="1" /><circle cx="592.7509175043582" cy="846.6267751715245" r="5" fill="white" stroke="none" stroke-width="1" /><circle cx="202.7046433700865" cy="725.1651507897237" r="5" fill="white" stroke="none" stroke-width="1" /><circle cx="660.3973192915555" cy="441.8014999201921" r="5" fill="white" stroke="none" stroke-width="1" /><circle cx="690.4467847454753" cy="819.3856709748779" r="5" fill="white" stroke="none" stroke-width="1" /><circle cx="99.25163555140999" cy="520.6462422211027" r="5" fill="white" stroke="none" stroke-width="1" /><circle cx="50.0" cy="50.0" r="5" fill="white" stroke="none" stroke-width="1" /><circle cx="695.2561033988173" cy="598.1921174405119" r="5" fill="white" stroke="none" stroke-width="1" /><circle cx="1150.0" cy="432.1126535822507" r="5" fill="white" stroke="none" stroke-width="1" /><circle cx="580.955800223462" cy="137.79639380890507" r="5" fill="white" stroke="none" stroke-width="1" /><text x="491.26376256587264" y="861.7217169297415" dx="7" dy="-3" font-size="20" font-family="Verdana" fill="rgba(255,255,255,0.85)" stroke="rgba(255,255,255,0.85)" stroke-width="3" stroke-linecap="round" stroke-linejoin="round" >Гостиница Сочи</text><text x="491.26376256587264" y="861.7217169297415" dx="7" dy="-3" font-size="20" font-family="Verdana" fill="black" stroke="none" stroke-width="1" >Гостиница Сочи</text><text x="592.7509175043582" y="846.6267751715245" dx="7" dy="-3" font-size="20" font-family="Verdana" fill="rgba(255,255,255,0.85)" stroke="rgba(255,255,255,0.85)" stroke-width="3" stroke-linecap="round" stroke-linejoin="round" >Кубанская улица</text><text x="592.7509175043582" y="846.6267751715245" dx="7" dy="-3" font-size="20" font-family="Verdana" fill="black" stroke="none" stroke-width="1" >Кубанская улица</text><text x="202.7046433700865" y="725.1651507897237" dx="7" dy="-3" font-size="20" font-family="Verdana" fill="rgba(255,255,255,0.85)" stroke="rgba(255,255,255,0.85)" stroke-width="3" stroke-linecap="round" stroke-linejoin="round" >Морской вокзал</text><text x="202.7046433700865" y="725.1651507897237" dx="7" dy="-3" font-size="20" font-family="Verdana" fill="black" stroke="none" stroke-width="1" >Морской вокзал</text><text x="660.3973192915555" y="441.8014999201921" dx="7" dy="-3" font-size="20" font-family="Verdana" fill="rgba(255,255,255,0.85)" stroke="rgba(255,255,255,0.85)" stroke-width="3" stroke-linecap="round" stroke-linejoin="round" >Параллельная улица</text><text x="660.3973192915555" y="441.8014999201921" dx="7" dy="-3" font-size="20" font-family="Verdana" fill="black" stroke="none" stroke-width="1" >Параллельная улица</text><text x="690.4467847454753" y="819.3856709748779" dx="7" dy="-3" font-size="20" font-family="Verdana" fill="rgba(255,255,255,0.85)" stroke="rgba(255,255,255,0.85)" stroke-width="3" stroke-linecap="round" stroke-linejoin="round" >По требованию</text><text x="690.4467847454753" y="819.3856709748779" dx="7" dy="-3" font-size="20" font-family="Verdana" fill="black" stroke="none" stroke-width="1" >По требованию</text><text x="99.25163555140999" y="520.6462422211027" dx="7" dy="-3" font-size="20" font-family="Verdana" fill="rgba(255,255,255,0.85)" stroke="rgba(255,255,255,0.85)" stroke-width="3" stroke-linecap="round" stroke-linejoin="round" >Ривьерский мост</text><text x="99.25163555140999" y="520.6462422211027" dx="7" dy="-3" font-size="20" font-family="Verdana" fill="black" stroke="none" stroke-width="1" >Ривьерский мост</text><text x="50.0" y="50.0" dx="7" dy="-3" font-size="20" font-family="Verdana" fill="rgba(255,255,255,0.85)" stroke="rgba(255,255,255,0.85)" stroke-width="3" stroke-linecap="round" stroke-linejoin="round" >Санаторий Родина</text><text x="50.0" y="50.0" dx="7" dy="-3" font-size="20" font-family="Verdana" fill="black" stroke="none" stroke-width="1" >Санаторий Родина</text><text x="695.2561033988173" y="598.1921174405119" dx="7" dy="-3" font-size="20" font-family="Verdana" fill="rgba(255,255,255,0.85)" stroke="rgba(255,255,255,0.85)" stroke-width="3" stroke-linecap="round" stroke-linejoin="round" >Улица Докучаева</text><text x="695.2561033988173" y="598.1921174405119" dx="7" dy="-3" font-size="20" font-family="Verdana" fill="black" stroke="none" stroke-width="1" >Улица Докучаева</text><text x="1150.0" y="432.1126535822507" dx="7" dy="-3" font-size="20" font-family="Verdana" fill="rgba(255,255,255,0.85)" stroke="rgba(255,255,255,0.85)" stroke-width="3" stroke-linecap="round" stroke-linejoin="round" >Улица Лизы Чайкиной</text><text x="1150.0" y="432.1126535822507" dx="7" dy="-3" font-size="20" font-family="Verdana" fill="black" stroke="none" stroke-width="1" >Улица Лизы Чайкиной</text><text x="580.955800223462" y="137.79639380890507" dx="7" dy="-3" font-size="20" font-family="Verdana" fill="rgba(255,255,255,0.85)" stroke="rgba(255,255,255,0.85)" stroke-width="3" stroke-linecap="round" stroke-linejoin="round" >Электросети</text><text x="580.955800223462" y="137.79639380890507" dx="7" dy="-3" font-size="20" font-family="Verdana" fill="black" stroke="none" stroke-width="1" >Электросети</text></svg>)~");
    }

}

void TestJsonPipeline() {
    istringstream input(R"({"routing_settings": {"bus_wait_time": 6,"bus_velocity": 40},"base_requests": [{"type": "Bus","name": "297","stops": ["Biryulyovo Zapadnoye","Biryulyovo Tovarnaya","Universam","Biryulyovo Zapadnoye"],"is_roundtrip": true},{"type": "Bus","name": "635","stops": ["Biryulyovo Tovarnaya","Universam","Prazhskaya"],"is_roundtrip": false},{"type": "Stop","road_distances": {"Biryulyovo Tovarnaya": 2600},"longitude": 37.6517,"name": "Biryulyovo Zapadnoye","latitude": 55.574371},{"type": "Stop","road_distances": {"Prazhskaya": 4650,"Biryulyovo Tovarnaya": 1380,"Biryulyovo Zapadnoye": 2500},"longitude": 37.645687,"name": "Universam","latitude": 55.587655},{"type": "Stop","road_distances": {"Universam": 890},"longitude": 37.653656,"name": "Biryulyovo Tovarnaya","latitude": 55.592028},{"type": "Stop","road_distances": {},"longitude": 37.603938,"name": "Prazhskaya","latitude": 55.611717}],"stat_requests": [{"type": "Bus","name": "297","id": 1},{"type": "Bus","name": "635","id": 2},{"type": "Stop","name": "Universam","id": 3},{"type": "Route","from": "Biryulyovo Zapadnoye","to": "Universam","id": 4},{"type": "Route","from": "Biryulyovo Zapadnoye","to": "Prazhskaya","id": 5}]})");
    string answer = R"([{"curvature": 1.42963,"unique_stop_count": 3,"stop_count": 4,"request_id": 1,"route_length": 5990},{"curvature": 1.30156,"unique_stop_count": 3,"stop_count": 5,"request_id": 2,"route_length": 11570},{"request_id": 3,"buses": ["297","635"]},{"total_time": 11.235,"items": [{"time": 6,"type": "Wait","stop_name": "Biryulyovo Zapadnoye"},{"span_count": 2,"bus": "297","type": "Bus","time": 5.235}],"request_id": 4},{"total_time": 24.21,"items": [{"time": 6,"type": "Wait","stop_name": "Biryulyovo Zapadnoye"},{"span_count": 2,"bus": "297","type": "Bus","time": 5.235},{"time": 6,"type": "Wait","stop_name": "Universam"},{"span_count": 1,"bus": "635","type": "Bus","time": 6.975}],"request_id": 5}])";

    ostringstream output;
    RunPipeline(input, output);
    ASSERT(CompareResponseJsonStrings(output.str(), answer));
}

void TestPipelineE() {
    {
        ifstream input("tests/transport_e_input_1.json");
        ifstream result("tests/transport_e_output_1.json");
        string answer;
        getline(result, answer);

        ostringstream output;
        RunPipeline(input, output);
        ASSERT(CompareResponseJsonStrings(output.str(), answer));
    }
    {
        ifstream input("tests/transport_e_input_2.json");
        ifstream result("tests/transport_e_output_2.json");
        string answer;
        getline(result, answer);

        ostringstream output;
        RunPipeline(input, output);
        ASSERT(CompareResponseJsonStrings(output.str(), answer));
    }
    {
        ifstream input("tests/transport_e_input_3.json");
        ifstream result("tests/transport_e_output_3.json");
        string answer;
        getline(result, answer);

        ostringstream output;
        RunPipeline(input, output);
        ASSERT(CompareResponseJsonStrings(output.str(), answer));
    }
}

void TestPipelineE_Perfomance() {
    ifstream input("tests/transport_e_input_4.json");
    ifstream result("tests/transport_e_output_4.json");
    string answer;
    getline(result, answer);

    ostringstream output;
    LOG_DURATION("TestPipelineE_Perfomance");
    RunPipeline(input, output);
    ASSERT(CompareResponseJsonStrings(output.str(), answer));
}

void TestPipelineG() {
    {
        ifstream input("tests/transport_g_input_1.json");
        ifstream result("tests/transport_g_output_1.json");
        string answer;
        getline(result, answer);

        ostringstream output;
        RunPipeline(input, output);
//        ASSERT_EQUAL(output.str(), answer);
        ASSERT(CompareResponseJsonStrings(output.str(), answer));
    }
    {
        ifstream input("tests/transport_g_input_2.json");
        ifstream result("tests/transport_g_output_2.json");
        string answer;
        getline(result, answer);

        ostringstream output;
        RunPipeline(input, output);
        ASSERT(CompareResponseJsonStrings(output.str(), answer));
    }
}

int main() {
#ifdef LOCAL_BUILD
    TestRunner tr;
    RUN_TEST(tr, Json::TestJson);
    RUN_TEST(tr, Json::TestCompareNodes);
    RUN_TEST(tr, Svg::TestPoint);
    RUN_TEST(tr, Svg::TestRgb);
    RUN_TEST(tr, Svg::TestColor);
    RUN_TEST(tr, Svg::TestCircle);
    RUN_TEST(tr, Svg::TestPolyline);
    RUN_TEST(tr, Svg::TestText);
    RUN_TEST(tr, Svg::TestPipeline);

//    RUN_TEST(tr, TestTransportMap);
    RUN_TEST(tr, TestJsonPipeline);
    RUN_TEST(tr, TestPipelineE);
    RUN_TEST(tr, TestPipelineE_Perfomance);
    RUN_TEST(tr, TestPipelineG);
#else
    RunPipeline(cin, cout);
#endif
    return 0;
}
