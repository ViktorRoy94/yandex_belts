#include "json.h"
#include "test_runner.h"

#include <regex>

using namespace std;

namespace Json {

Node LoadArray(istream& input) {
    vector<Node> result;

    for (char c; input >> c && c != ']'; ) {
        if (c != ',') {
            input.putback(c);
        }
        result.push_back(LoadNode(input));
    }

    return Node(move(result));
}

Node LoadBool(istream& input) {
    string s;
    while (isalpha(input.peek())) {
        s.push_back(input.get());
    }
    return Node(s == "true");
}

Node LoadNumber(istream& input) {
    bool is_negative = false;
    if (input.peek() == '-') {
        is_negative = true;
        input.get();
    }
    int int_part = 0;
    while (isdigit(input.peek())) {
        int_part *= 10;
        int_part += input.get() - '0';
    }
    if (input.peek() != '.') {
        return Node(int_part * (is_negative ? -1 : 1));
    }
    input.get();  // '.'
    double result = int_part;
    double frac_mult = 0.1;
    while (isdigit(input.peek())) {
        result += frac_mult * (input.get() - '0');
        frac_mult /= 10;
    }
    return Node(result * (is_negative ? -1 : 1));
}

Node LoadString(istream& input) {
    string line;
    getline(input, line, '"');
    return Node(move(line));
}

Node LoadDict(istream& input) {
    Dict result;

    for (char c; input >> c && c != '}'; ) {
        if (c == ',') {
            input >> c;
        }

        string key = LoadString(input).AsString();
        input >> c;
        result.emplace(move(key), LoadNode(input));
    }

    return Node(move(result));
}

Node LoadNode(istream& input) {
    char c;
    input >> c;

    if (c == '[') {
        return LoadArray(input);
    } else if (c == '{') {
        return LoadDict(input);
    } else if (c == '"') {
        return LoadString(input);
    } else if (c == 't' || c == 'f') {
        input.putback(c);
        return LoadBool(input);
    } else {
        input.putback(c);
        return LoadNumber(input);
    }
}

Document Load(istream& input) {
    return Document{LoadNode(input)};
}

template <>
void PrintValue<string>(const string& value, ostream& output) {
    output << '"' << std::regex_replace(value, std::regex(R"(")"), R"(\")") << '"';
}

template <>
void PrintValue<bool>(const bool& value, std::ostream& output) {
    output << std::boolalpha << value;
}

template <>
void PrintValue<std::vector<Node>>(const std::vector<Node>& nodes, std::ostream& output) {
    output << '[';
    bool first = true;
    for (const Node& node : nodes) {
        if (!first) {
            output << ", ";
        }
        first = false;
        PrintNode(node, output);
    }
    output << ']';
}

template <>
void PrintValue<Dict>(const Dict& dict, std::ostream& output) {
    output << '{';
    bool first = true;
    for (const auto& [key, node]: dict) {
        if (!first) {
            output << ", ";
        }
        first = false;
        PrintValue(key, output);
        output << ": ";
        PrintNode(node, output);
    }
    output << '}';
}

void PrintNode(const Json::Node& node, ostream& output) {
    visit([&output](const auto& value) { PrintValue(value, output); },
    node.GetBase());
}

void Print(const Document& document, ostream& output) {
    PrintNode(document.GetRoot(), output);
}

bool CompareNodes(const Node& node1, const Node& node2) {
    bool result = true;
    try {
        const auto& arr1 = node1.AsArray();
        const auto& arr2 = node2.AsArray();
        result = arr1.size() == arr2.size();
        for (size_t i = 0; i < arr1.size(); i++) {
            result = result && CompareNodes(arr1[i], arr2[i]);
            if (!result) return result;
        }
    } catch (std::bad_variant_access const&) {
    }
    try {
        const auto& map1 = node1.AsMap();
        const auto& map2 = node2.AsMap();
        result = map1.size() == map2.size();

        for (const auto& [k1, v1] : map1) {
            auto it2 = map2.find(k1);
            result = it2 != map2.end();
            result = result &&  CompareNodes(v1, it2->second);
            if (!result) return result;
        }
    } catch (std::bad_variant_access const&) {
    }
    try {
        const auto& int1 = node1.AsInt();
        const auto& int2 = node2.AsInt();
        return result && int1 == int2;
    } catch (std::bad_variant_access const&) {
    }
    try {
        const auto& double1 = node1.AsDouble();
        const auto& double2 = node2.AsDouble();
        return result && abs(double1 - double2) < 0.001;
    } catch (std::bad_variant_access const&) {
    }
    try {
        const auto& bool1 = node1.AsBool();
        const auto& bool2 = node2.AsBool();
        return result && bool1 == bool2;
    } catch (std::bad_variant_access const&) {
    }
    try {
        const auto& string1 = node1.AsString();
        const auto& string2 = node2.AsString();
        return result && string1 == string2;
    } catch (std::bad_variant_access const&) {
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

bool CompareJsonStrings(std::string str1, std::string str2) {
    istringstream input1(str1);
    istringstream input2(str2);
    Document doc1 = Load(input1);
    Document doc2 = Load(input2);

    return CompareNodes(doc1.GetRoot(), doc2.GetRoot());
}

void TestCompareNodes() {
    {
        string input(R"({"int": 10, "double": 1823423.1231, "bool": true, "array": [1, 2, 3], "string": "data_string"})");
        string input_changed_string(R"({"int": 10, "double": 1823423.1231, "bool": true, "array": [1, 2, 3], "string": "data"})");
        string input_changed_int(R"({"int": 9, "double": 1823423.1231, "bool": true, "array": [1, 2, 3], "string": "data_string"})");
        string input_changed_double(R"({"int": 10, "double": 1823423.14, "bool": true, "array": [1, 2, 3], "string": "data_string"})");
        string input_changed_bool(R"({"int": 10, "double": 1823423.1231, "bool": false, "array": [1, 2, 3], "string": "data_string"})");
        string input_changed_map(R"({"int": 10, "double": 1823423.1231, "bool": true, "string": "data_string"})");
        string input_changed_array(R"({"int": 10, "double": 1823423.1231, "bool": true, "array": [10, 2, 3], "string": "data_string"})");
        ASSERT( CompareJsonStrings(input, input));
        ASSERT(!CompareJsonStrings(input, input_changed_string));
        ASSERT(!CompareJsonStrings(input, input_changed_int));
        ASSERT(!CompareJsonStrings(input, input_changed_double));
        ASSERT(!CompareJsonStrings(input, input_changed_bool));
        ASSERT(!CompareJsonStrings(input, input_changed_map));
        ASSERT(!CompareJsonStrings(input, input_changed_array));
    }
}

}
