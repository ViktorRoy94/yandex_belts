#pragma once

#include "http_request.h"

#include <string_view>
#include <map>
#include <set>

using namespace std;

class Stats {
public:
    Stats();
    void AddMethod(string_view method);
    void AddUri(string_view uri);
    const map<string_view, int>& GetMethodStats() const;
    const map<string_view, int>& GetUriStats() const;
private:
    map<string_view, int> methods_data_;
    map<string_view, int> uri_data_;

    inline static const set<string> supported_methods_ = {"GET", "POST", "PUT", "DELETE"};
    inline static const set<string> supported_uri_ = {"/", "/order", "/product", "/basket", "/help"};

    inline static const string unknown_method_ = "UNKNOWN";
    inline static const string unknown_uri_ = "unknown";
};

HttpRequest ParseRequest(string_view line);
