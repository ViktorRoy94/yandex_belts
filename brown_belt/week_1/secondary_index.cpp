#include "test_runner.h"

#include <iostream>
#include <map>
#include <string>
#include <unordered_map>
#include <list>

using namespace std;

struct Record {
    string id;
    string title;
    string user;
    int timestamp;
    int karma;
    bool operator==(const Record& r) const {
        return tie(  id,   title,   user,   timestamp,   karma) ==
               tie(r.id, r.title, r.user, r.timestamp, r.karma);
    }
};

struct RecordHasher {
    size_t operator() (const Record& r) const {
        size_t coef = 3615701;
        hash<string> shash;
        hash<int> ihash;

        return coef * coef * coef * coef * shash(r.id) +
               coef * coef * coef * shash(r.title) +
               coef * coef * shash(r.user) +
               coef * ihash(r.timestamp) +
               ihash(r.karma);
    }
};

//bool operator<(const Record& l, const Record& r) {
//    return tie(l.id, l.title, l.user, l.timestamp, l.karma) <
//           tie(r.id, r.title, r.user, r.timestamp, r.karma);
//}

// Реализуйте этот класс
class Database {
public:
    bool Put(const Record& record) {
        auto [it, success] = storage_.insert({record.id, Data{record, {}, {}, {}}});
        if (!success) {
            return false;
        } else {
            Data& data = it->second;
            const Record* ptr = &data.record;
            data.timestamp_iter = timestamp_index_.insert({record.timestamp, ptr});
            data.karma_iter = karma_index_.insert({record.karma, ptr});
            data.user_iter = user_index_.insert({record.user, ptr});

            return true;
        }
    }
    const Record* GetById(const string& id) const {
        if (storage_.count(id) == 0) {
            return nullptr;
        } else {
            return &storage_.at(id).record;
        }
    }
    bool Erase(const string& id) {
        auto it = storage_.find(id);
        if (it == storage_.end()) {
            return false;
        } else {
            Data& data = it->second;
            timestamp_index_.erase(data.timestamp_iter);
            karma_index_.erase(data.karma_iter);
            user_index_.erase(data.user_iter);
            storage_.erase(it);

            return true;
        }
    }

    template <typename Callback>
    void RangeByTimestamp(int low, int high, Callback callback) const {
        auto low_it  = timestamp_index_.lower_bound(low);
        auto high_it = timestamp_index_.upper_bound(high);
        IterateThroughRange(low_it, high_it, callback);
    }

    template <typename Callback>
    void RangeByKarma(int low, int high, Callback callback) const {
        auto low_it  = karma_index_.lower_bound(low);
        auto high_it = karma_index_.upper_bound(high);
        IterateThroughRange(low_it, high_it, callback);
    }

    template <typename Callback>
    void AllByUser(const string& user, Callback callback) const {
        auto range = user_index_.equal_range(user);
        IterateThroughRange(range.first, range.second, callback);
    }

private:
    template <typename Type>
    using Index = multimap<Type, const Record*>;

    struct Data {
        Record record;
        Index<int>::iterator timestamp_iter;
        Index<int>::iterator karma_iter;
        Index<string>::iterator user_iter;
    };

    unordered_map<string, Data> storage_;
    Index<int> timestamp_index_;
    Index<int> karma_index_;
    Index<string> user_index_;

    template <typename InputIt, typename Callback>
    void IterateThroughRange(InputIt begin, InputIt end, Callback callback) const {
        for (auto it = begin; it != end; ++it) {
            if (!callback(*it->second)) {
                break;
            }
        }
    }
};

void TestKarmaRangeBoundaries() {
    const int good_karma = 1000;
    const int bad_karma = -10;

    Database db;
    db.Put({"id1", "Hello there", "master", 1536107260, good_karma});
    db.Put({"id2", "O>>-<", "general2", 1536107260, bad_karma});
    db.Put({"id3", "Hello there1", "master1", 1536107261, bad_karma - 1});
    db.Put({"id4", "Hello there2", "master2", 1536107262, good_karma + 1});

    int count = 0;
    db.RangeByKarma(bad_karma, good_karma, [&count](const Record&) {
        ++count;
        return true;
    });

    ASSERT_EQUAL(2, count);

    db.Erase("id1");

    count = 0;
    db.RangeByKarma(bad_karma, good_karma, [&count](const Record&) {
        ++count;
        return true;
    });

    ASSERT_EQUAL(1, count);

}

void TestUserRangeBoundaries() {
    {
        Database db;
        db.Put({"id1", "1", "1", 1536107260, 0});
        db.Put({"id2", "1", "2", 1536107261, 0});
        db.Put({"id3", "1", "3", 1536107262, 0});
        db.Put({"id4", "1", "4", 1536107263, 0});

        int count = 0;
        db.AllByUser("1", [&count](const Record&) {
            ++count;
            return true;
        });

        ASSERT_EQUAL(1, count);

        db.Put({"id5", "1", "1", 1536107264, 0});

        count = 0;
        db.AllByUser("1", [&count](const Record&) {
            ++count;
            return true;
        });
        ASSERT_EQUAL(2, count);
    }
    {
        Database db;
        db.Put({"id1", "1", "1", 1536107260, 0});
        int count = 0;
        db.AllByUser("1", [&count](const Record&) {
            ++count;
            return true;
        });

        ASSERT_EQUAL(1, count);
    }
    {
        Database db;
        db.Put({"id1", "1", "1", 1536107260, 0});
        db.Put({"id2", "1", "1", 1536107260, 0});
        db.Put({"id3", "1", "1", 1536107260, 0});

        db.Erase("id1");

        int count = 0;
        db.AllByUser("1", [&count](const Record&) {
            ++count;
            return true;
        });

        ASSERT_EQUAL(2, count);
    }
}

void TestSameUser() {
    Database db;
    db.Put({"id1", "Don't sell", "master", 1536107260, 1000});
    db.Put({"id2", "Rethink life", "master", 1536107260, 2000});

    int count = 0;
    db.AllByUser("master", [&count](const Record&) {
        ++count;
        return true;
    });

    ASSERT_EQUAL(2, count);
}

void TestReplacement() {
    const string final_body = "Feeling sad";

    Database db;
    db.Put({"id", "Have a hand", "not-master", 1536107260, 10});
    db.Erase("id");
    db.Put({"id", final_body, "not-master", 1536107260, -10});

    auto record = db.GetById("id");
    ASSERT(record != nullptr);
    ASSERT_EQUAL(final_body, record->title);
}

int main() {
    TestRunner tr;
    RUN_TEST(tr, TestKarmaRangeBoundaries);
    RUN_TEST(tr, TestUserRangeBoundaries);
    RUN_TEST(tr, TestSameUser);
    RUN_TEST(tr, TestReplacement);
    return 0;
}
