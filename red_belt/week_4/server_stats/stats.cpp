#include "stats.h"
#include <vector>
#include <iostream>

Stats::Stats() {
    for (const auto& m : supported_methods_) {
        methods_data_[m] = 0;
    }
    methods_data_[unknown_method_] = 0;
    for (const auto& m : supported_uri_) {
        uri_data_[m] = 0;
    }
    uri_data_[unknown_uri_] = 0;
}

void Stats::AddMethod(string_view method) {
    auto it = supported_methods_.find(string(method));
    if ( it != supported_methods_.end()) {
        methods_data_[*it]++;
    } else {
        methods_data_[unknown_method_]++;
    }
}
void Stats::AddUri(string_view uri) {
    auto it = supported_uri_.find(string(uri));
    if ( it != supported_uri_.end()) {
        uri_data_[*it]++;
    } else {
        uri_data_[unknown_uri_]++;
    }
}
const map<string_view, int>& Stats::GetMethodStats() const {
    return methods_data_;
}
const map<string_view, int>& Stats::GetUriStats() const {
    return uri_data_;
}

void LeftStrip(string_view& sv) {
    const char* white_spaces = " \t\v\r\n";
    std::size_t start = sv.find_first_not_of(white_spaces);
    sv.remove_prefix(start);
}

string_view ReadToken(string_view& sv) {
    auto pos = sv.find(' ');
    auto result = sv.substr(0, pos);
    sv.remove_prefix(pos != sv.npos ? pos + 1 : sv.size());
    return result;
}

HttpRequest ParseRequest(string_view line) {
    LeftStrip(line);

    string_view method = ReadToken(line);
    string_view uri = ReadToken(line);
    string_view protocol = ReadToken(line);

    return {method, uri, protocol};
}
