#include "database.h"

#include <algorithm>
#include <iostream>
#include <sstream>

void Database::Add(const Date& date, const std::string& event) {
    if (fast_data_[date].count(event) == 0) {
        data_[date].push_back(event);
        fast_data_[date].insert(event);
    }
}

void Database::Print(ostream& os) const {
    for (const auto& [date, events]: data_) {
        for (const auto& e : events) {
            os << date << " " << e << endl;
        }
    }
}

string Database::Last(const Date& d) const {
    auto it = data_.upper_bound(d);
    stringstream ss("No entries");
    if (it != data_.begin()) {
        --it;
        ss << it->first << " " << it->second.back();
    }
    return ss.str();
}