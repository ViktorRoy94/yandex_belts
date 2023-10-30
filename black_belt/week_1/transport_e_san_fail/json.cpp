#include "json.h"

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
    output << '"' << value << '"';
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
    return result;
}

}
