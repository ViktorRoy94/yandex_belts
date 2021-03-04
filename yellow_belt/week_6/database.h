#pragma once

#include "date.h"
#include "node.h"

#include <map>
#include <set>
#include <vector>
#include <algorithm>

#include <iostream>

using namespace std;

class Database {
public:
    void Add(const Date& date, const string& event);

    template <class CondFunc>
    int RemoveIf(CondFunc condition) {
        int count = 0;
        vector<Date> dates_for_erase;
        for (auto& kv : data_) {
            const auto& date = kv.first;
            auto& events = kv.second;
            auto split_point = stable_partition(begin(events), end(events),
                                                [date, condition](const string& event){
                return condition(date, event);
            }
            );

            count += split_point - events.begin();
            for (auto it = begin(events); it != split_point; ++it) {
                fast_data_[date].erase(*it);
            }
            events.erase(begin(events), split_point);

            if (events.size() == 0) {
                dates_for_erase.push_back(date);
            }
        }
        for (const auto& date : dates_for_erase) {
            data_.erase(date);
            fast_data_.erase(date);
        }
        return count;
    }

    template <class CondFunc>
    vector<pair<Date, string>> FindIf(CondFunc condition) const {
        vector<pair<Date, string>> result;
        for (const auto& [date, events] : data_) {
            for (const auto& e : events) {
                if (condition(date, e)) {
                    result.push_back(make_pair(date, e));
                }
            }
        }
        return result;
    }

    void Print(ostream& os) const;
    string Last(const Date& d) const;

private:
    map<Date, vector<string>> data_;
    map<Date, set<string>> fast_data_;
};

template <class K, class V>
ostream& operator << (ostream& os, const pair<K, V>& p) {
    return os << p.first << " " << p.second;
}

void TestDatabase();
