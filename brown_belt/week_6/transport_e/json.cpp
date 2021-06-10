#include "json.h"

#include <cmath>
#include <sstream>

using namespace std;

namespace Json {

Document::Document(Node root) : root(move(root)) {
}

const Node& Document::GetRoot() const {
    return root;
}

Node LoadNode(istream& input);

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

Node LoadDigit(istream& input) {
    unsigned long result = 0;
    int digit_after_point = 0;
    int sign = 1;
    char c = input.peek();
    while(c != '-' && !isdigit(c)) {
    }
    c = input.peek();
    if (c == '-') {
        sign = -1;
        input.get();
    }

    c = input.peek();
    while (isdigit(c)) {
        result *= 10;
        result += input.get() - '0';
        if (digit_after_point > 0) {
            digit_after_point++;
        }
        if (input.peek() == '.') {
            input.get();
            digit_after_point++;
        }
        c = input.peek();
    }
    if (digit_after_point > 0) {
        double number = result * 1.0 / pow(10, digit_after_point-1);
        return Node(sign * number);
    } else {
        return Node(sign * static_cast<int>(result));
    }

}

bool CheckInputContainString(istream& input, string str) {
    size_t index = 0;
    while (input.peek() == str[index++]) {
        input.get();
    }
    if (index == str.size() + 1) {
        return true;
    } else {
        return false;
    }
}

Node LoadBool(istream& input) {
    if (CheckInputContainString(input, "false")) {
        return Node(false);
    }
    if (CheckInputContainString(input, "true")) {
        return Node(true);
    }
    return Node("");
}

Node LoadString(istream& input) {
    string line;
    getline(input, line, '"');
    return Node(move(line));
}

Node LoadDict(istream& input) {
    map<string, Node> result;

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
    } else if (isdigit(c) || c == '-'){
        input.putback(c);
        return LoadDigit(input);
    } else {
        input.putback(c);
        return LoadBool(input);
    }
}

Document Load(istream& input) {
    return Document{LoadNode(input)};
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
        return result && abs(double1 - double2) < 0.01;
    } catch (std::bad_variant_access const&) {
    }
    try {
        const auto& bool1 = node1.AsBool();
        const auto& bool2 = node2.AsBool();
        return result && bool1 == bool2;
    } catch (std::bad_variant_access const&) {
    }
    return result;
}

bool CompareJsonStrings(std::string str1, std::string str2) {
    istringstream input1(str1);
    istringstream input2(str2);
    Document doc1 = Load(input1);
    Document doc2 = Load(input2);

    return CompareNodes(doc1.GetRoot(), doc2.GetRoot());
}

}
