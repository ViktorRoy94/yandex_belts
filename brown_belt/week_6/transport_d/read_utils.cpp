using namespace std;

#include "read_utils.h"

pair<string_view, optional<string_view>> SplitTwoStrict(string_view s, string_view delimiter) {
    const size_t pos = s.find(delimiter);
    if (pos == s.npos) {
        return {s, nullopt};
    } else {
        return {s.substr(0, pos), s.substr(pos + delimiter.length())};
    }
}

pair<string_view, string_view> SplitTwo(string_view s, string_view delimiter) {
    const auto [lhs, rhs_opt] = SplitTwoStrict(s, delimiter);
    return {lhs, rhs_opt.value_or("")};
}

string_view ReadToken(string_view& s, string_view delimiter) {
    const auto [lhs, rhs] = SplitTwo(s, delimiter);
    s = Lstrip(rhs);
    return Rstrip(lhs);
}

int ConvertToInt(string_view str) {
    // use std::from_chars when available to get rid of string copy
    size_t pos;
    const int result = stoi(string(str), &pos);
    if (pos != str.length()) {
        std::stringstream error;
        error << "string " << str << " contains " << (str.length() - pos) << " trailing chars";
        throw invalid_argument(error.str());
    }
    return result;
}

double ConvertToDouble(std::string_view str) {
    // use std::from_chars when available to get rid of string copy
    size_t pos;
    const double result = stod(string(str), &pos);
    if (pos != str.length()) {
        std::stringstream error;
        error << "string " << str << " contains " << (str.length() - pos) << " trailing chars";
        throw invalid_argument(error.str());
    }
    return result;
}

string_view Lstrip(string_view line) {
    while (!line.empty() && isspace(line[0])) {
        line.remove_prefix(1);
    }
    return line;
}

string_view Rstrip(string_view line) {
    while (!line.empty() && isspace(line[line.size() - 1])) {
        line.remove_suffix(1);
    }
    return line;
}



