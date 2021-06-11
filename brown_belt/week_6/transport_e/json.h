#pragma once

#include <istream>
#include <map>
#include <string>
#include <variant>
#include <vector>

namespace Json {

class Node : std::variant<std::vector<Node>,
                          std::map<std::string, Node>,
                          int,
                          bool,
                          double,
                          std::string> {
public:
    using variant::variant;

    const auto& AsArray() const {
        return std::get<std::vector<Node>>(*this);
    }
    const auto& AsMap() const {
        return std::get<std::map<std::string, Node>>(*this);
    }
    int AsInt() const {
        return std::get<int>(*this);
    }
    bool AsBool() const {
        return std::get<bool>(*this);
    }
    double AsDouble() const {
        return std::holds_alternative<double>(*this) ?
                    std::get<double>(*this) :
                    std::get<int>(*this);
    }
    const auto& AsString() const {
        return std::get<std::string>(*this);
    }
};

class Document {
public:
    explicit Document(Node root);

    const Node& GetRoot() const;

private:
    Node root;
};

Document Load(std::istream& input);

bool CompareNodes(const Node& node1, const Node& node2);
bool CompareJsonStrings(std::string str1, std::string str2);

}
