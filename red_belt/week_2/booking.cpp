#include <iomanip>
#include <iostream>
#include <vector>
#include <utility>
#include <map>
#include <numeric>
#include <queue>
#include <algorithm>

#include "test_runner.h"

using namespace std;

struct Booking {
    int64_t time;
    int64_t client_id;
    int rooms;
};

class HotelInfo {
public:
    void Book(const Booking& booking) {
        events_.push(booking);
        client_counts_[booking.client_id]++;
        rooms_used_ += booking.rooms;
    }

    int ClientsCount(uint64_t current_time) {
        DeleteOldBooking(current_time);
        return client_counts_.size();
    }

    int RoomsCount(uint64_t current_time) {
        DeleteOldBooking(current_time);
        return rooms_used_;
    }

private:
    static const int64_t SECONDS_IN_DAY = 86400;
    int rooms_used_;
    map<int, int> client_counts_;
    queue<Booking> events_;

    void DeleteOldBooking(uint64_t time) {
        while (!events_.empty() && time - events_.front().time >= SECONDS_IN_DAY) {
            PopBooking();
        }
    }

    void PopBooking() {
        int client_id = events_.front().client_id;
        int rooms = events_.front().rooms;
        client_counts_[client_id]--;
        if (client_counts_[client_id] <= 0) {
            client_counts_.erase(client_id);
        }
        rooms_used_ -= rooms;
        events_.pop();
    }
};

class BookingManager {
public:
    BookingManager() {}
    void AddBooking(int64_t time, string hotel_name, int client_id, int room_count) {
        current_time_ = time;
        hotels_[hotel_name].Book({time, client_id, room_count});
    }

    int Clients(string hotel_name) {
        return hotels_[hotel_name].ClientsCount(current_time_);
    }

    int Rooms(string hotel_name) {
        return hotels_[hotel_name].RoomsCount(current_time_);
    }
private:
    map<string, HotelInfo> hotels_;
    uint64_t current_time_;
};

void TestBookingManager() {
    {
        BookingManager manager;

        ASSERT_EQUAL(manager.Clients("Marriott"), 0);
        ASSERT_EQUAL(manager.Rooms("Marriott"), 0);

        manager.AddBooking(10, "FourSeasons", 1, 2);
        manager.AddBooking(10, "Marriott", 1, 1);
        manager.AddBooking(86409, "FourSeasons", 2, 1);

        ASSERT_EQUAL(manager.Clients("FourSeasons"), 2);
        ASSERT_EQUAL(manager.Rooms("FourSeasons"), 3);
        ASSERT_EQUAL(manager.Clients("Marriott"), 1);

        manager.AddBooking(86410, "Marriott", 2, 10);

        ASSERT_EQUAL(manager.Rooms("FourSeasons"), 1);
        ASSERT_EQUAL(manager.Rooms("Marriott"), 10);
    }
    {
        BookingManager manager;

        // Test empty bookings
        ASSERT_EQUAL(manager.Clients("mariott"), 0);
        ASSERT_EQUAL(manager.Rooms("mariott"), 0);

        manager.AddBooking(0, "mariott", 1, 10);
        manager.AddBooking(0, "mariott", 2, 1);
        manager.AddBooking(0, "mariott", 3, 1);
        manager.AddBooking(0, "mariott", 4, 1);
        manager.AddBooking(0, "hilton", 1, 1);
        manager.AddBooking(1, "hilton", 2, 1);

        // Test correctness
        ASSERT_EQUAL(manager.Clients("mariott"), 4);
        ASSERT_EQUAL(manager.Rooms("mariott"), 13);
        ASSERT_EQUAL(manager.Clients("hilton"), 2);
        ASSERT_EQUAL(manager.Rooms("hilton"), 2);

        // Test event past 1 day resets statics
        manager.AddBooking(86400, "mariott", 1, 1);
        ASSERT_EQUAL(manager.Clients("mariott"), 1);
        ASSERT_EQUAL(manager.Rooms("mariott"), 1);
        ASSERT_EQUAL(manager.Clients("hilton"), 1);
        ASSERT_EQUAL(manager.Rooms("hilton"), 1);

        // Test no clients and room for the last day
        manager.AddBooking(86401, "mariott", 5, 1);
        ASSERT_EQUAL(manager.Clients("mariott"), 2);
        ASSERT_EQUAL(manager.Rooms("mariott"), 2);
        ASSERT_EQUAL(manager.Clients("hilton"), 0);
        ASSERT_EQUAL(manager.Rooms("hilton"), 0);
    }
    {
        BookingManager b;
        b.AddBooking(0, "a", 0, 1);
        b.AddBooking(1, "a", 1, 2);
        b.AddBooking(86400, "a", 1, 3);
        ASSERT_EQUAL(b.Clients("a"), 1);
        ASSERT_EQUAL(b.Rooms("a"), 5);
    }
}

void Test1() {
    BookingManager hm;
    ASSERT_EQUAL(hm.Clients("M"), 0);
    ASSERT_EQUAL(hm.Rooms("M"), 0);
    hm.AddBooking(10,"F",1,2);
    hm.AddBooking(10,"M",1,1);
    hm.AddBooking(86409,"F",2,1);
    ASSERT_EQUAL(hm.Clients("F"), 2);
    ASSERT_EQUAL(hm.Rooms("F"), 3);
    ASSERT_EQUAL(hm.Clients("M"), 1);
    hm.AddBooking(86410,"M",2,10);
    ASSERT_EQUAL(hm.Rooms("F"), 1);
    ASSERT_EQUAL(hm.Rooms("M"), 10);
}
void Test2() {
    BookingManager hm;

    ASSERT_EQUAL(hm.Rooms("a"), 0);
    ASSERT_EQUAL(hm.Clients("a"), 0);
    hm.AddBooking(-100000, "a", 100000, 1000);
    hm.AddBooking(-100000, "a", 100003, 1000);
    hm.AddBooking(-100000, "b", 100002, 1000);
    ASSERT_EQUAL(hm.Rooms("a"), 2000);
    ASSERT_EQUAL(hm.Clients("a"), 2);
    hm.AddBooking(-10000, "a", 100002, 1000);
    ASSERT_EQUAL(hm.Rooms("a"), 1000);
    ASSERT_EQUAL(hm.Clients("a"), 1);
    ASSERT_EQUAL(hm.Rooms("b"), 0);
    ASSERT_EQUAL(hm.Clients("b"), 0);
}
void Test3() {
    BookingManager manager;

    // Test empty bookings
    ASSERT_EQUAL(manager.Clients("mariott"), 0);
    ASSERT_EQUAL(manager.Rooms("mariott"), 0);

    manager.AddBooking(0, "mariott", 1, 10);
    manager.AddBooking(0, "mariott", 2, 1);
    manager.AddBooking(0, "mariott", 3, 1);
    manager.AddBooking(0, "mariott", 4, 1);
    manager.AddBooking(0, "hilton", 1, 1);
    manager.AddBooking(1, "hilton", 2, 1);

    // Test correctness
    ASSERT_EQUAL(manager.Clients("mariott"), 4);
    ASSERT_EQUAL(manager.Rooms("mariott"), 13);
    ASSERT_EQUAL(manager.Clients("hilton"), 2);
    ASSERT_EQUAL(manager.Rooms("hilton"), 2);

    // Test event past 1 day resets statics
    manager.AddBooking(86400, "mariott", 1, 1);
    ASSERT_EQUAL(manager.Clients("mariott"), 1);
    ASSERT_EQUAL(manager.Rooms("mariott"), 1);
    ASSERT_EQUAL(manager.Clients("hilton"), 1);
    ASSERT_EQUAL(manager.Rooms("hilton"), 1);

    // Test no clients and room for the last day
    manager.AddBooking(86401, "mariott", 5, 1);
    ASSERT_EQUAL(manager.Clients("mariott"), 2);
    ASSERT_EQUAL(manager.Rooms("mariott"), 2);
    ASSERT_EQUAL(manager.Clients("hilton"), 0);
    ASSERT_EQUAL(manager.Rooms("hilton"), 0);
}
void Test4() {
    BookingManager b;

    ASSERT_EQUAL(b.Rooms("a"),0);
    ASSERT_EQUAL(b.Clients("a"),0);

    ASSERT_EQUAL(b.Rooms("b"),0);
    ASSERT_EQUAL(b.Clients("c"),0);

    b.AddBooking(-100000,"a",100000,1000);
    b.AddBooking(-100000,"a",100003,1000);
    b.AddBooking(-100000,"b",100002,1000);
    ASSERT_EQUAL(b.Rooms("a"),2000);
    ASSERT_EQUAL(b.Clients("a"),2);

    b.AddBooking(-10000,"a",100002,1000);
    ASSERT_EQUAL(b.Rooms("a"),1000);
    ASSERT_EQUAL(b.Clients("a"),1);
    ASSERT_EQUAL(b.Rooms("b"),0);
    ASSERT_EQUAL(b.Clients("b"),0);
}
void Test5() {
    BookingManager b;

    b.AddBooking(0, "a", 0, 1);
    b.AddBooking(1, "a", 1, 2);
    b.AddBooking(86400, "a", 1, 3);
    ASSERT_EQUAL(b.Clients("a"), 1);
    ASSERT_EQUAL(b.Rooms("a"), 5);
}
void Test6() {
    BookingManager b;

    b.AddBooking(-86400, "a", 2, 2);
    b.AddBooking(0, "a", 1, 5);
    ASSERT_EQUAL(b.Clients("a"), 1);
    ASSERT_EQUAL(b.Rooms("a"), 5);
}
void Test7() {
    BookingManager b;

    b.AddBooking(1, "a", 11, 4);
    b.AddBooking(50000, "a", 11, 5);
    b.AddBooking(90000, "a", 22, 2);
    ASSERT_EQUAL(b.Clients("a"), 2);
    ASSERT_EQUAL(b.Rooms("a"), 7);
}
void Test8() {
    BookingManager b;

    b.AddBooking(2147483647, "a", 1, 3);
    b.AddBooking(2147483648, "a", 2, 11);
    b.AddBooking(4147483648, "a", 3, 13);
    ASSERT_EQUAL(b.Clients("a"), 1);
    ASSERT_EQUAL(b.Rooms("a"), 13);
}

void Test9() {
    BookingManager b;

    ASSERT_EQUAL(b.Rooms("a"), 0);
    ASSERT_EQUAL(b.Clients("a"), 0);
    b.AddBooking(-100000, "a", 100000, 1000);
    b.AddBooking(-100000, "a", 100003, 1000);
    b.AddBooking(-100000, "b", 100002, 1000);
    ASSERT_EQUAL(b.Rooms("a"), 2000);
    ASSERT_EQUAL(b.Clients("a"), 2);
    b.AddBooking(-10000, "a", 100002, 1000);
    ASSERT_EQUAL(b.Rooms("a"), 1000);
    ASSERT_EQUAL(b.Clients("a"), 1);
    ASSERT_EQUAL(b.Rooms("b"), 0);
    ASSERT_EQUAL(b.Clients("b"), 0);
}

void Test10() {
    BookingManager b;

    b.AddBooking(1000000000000000000, "a", 1, 3);
    b.AddBooking(1000000000000000000, "a", 2, 11);
    b.AddBooking(2000000000000000000, "a", 3, 13);
    ASSERT_EQUAL(b.Clients("a"), 1);
    ASSERT_EQUAL(b.Rooms("a"), 13);
}

void Test11() {
    BookingManager b;

    b.AddBooking(2147483647, "a", 100000001, 3);
    b.AddBooking(2147483648, "a", 100000002, 11);
    b.AddBooking(4147483648, "a", 100000003, 13);
    ASSERT_EQUAL(b.Clients("a"), 1);
    ASSERT_EQUAL(b.Rooms("a"), 13);
}

void Test12() {
    BookingManager b;

    b.AddBooking(0, "a", 1, 1);
    b.AddBooking(86500, "b", 1, 1);
    ASSERT_EQUAL(b.Clients("a"), 0);
}

void TestClients() {
    BookingManager bm;
    bm.AddBooking(0, "q", 0, 1);
    ASSERT_EQUAL(bm.Clients("q"), 1);
    ASSERT_EQUAL(bm.Rooms("q"), 1);
    bm.AddBooking(10, "q", 0, 3);
    ASSERT_EQUAL(bm.Clients("q"), 1);
    ASSERT_EQUAL(bm.Rooms("q"), 4);
    bm.AddBooking(86411, "q", 3, 1);
    ASSERT_EQUAL(bm.Clients("q"), 1);
    ASSERT_EQUAL(bm.Rooms("q"), 1);
}

int main() {
#ifdef LOCAL_BUILD
    TestRunner tr;
    RUN_TEST(tr, TestBookingManager);
    RUN_TEST(tr, Test1);
    RUN_TEST(tr, Test2);
    RUN_TEST(tr, Test3);
    RUN_TEST(tr, Test4);
    RUN_TEST(tr, Test5);
    RUN_TEST(tr, Test6);
    RUN_TEST(tr, Test7);
    RUN_TEST(tr, Test8);
    RUN_TEST(tr, Test9);
    RUN_TEST(tr, Test10);
    RUN_TEST(tr, Test11);
    RUN_TEST(tr, Test12);
    RUN_TEST(tr, TestClients);
#else
    // Для ускорения чтения данных отключается синхронизация
    // cin и cout с stdio,
    // а также выполняется отвязка cin от cout
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    BookingManager manager;

    int query_count;
    cin >> query_count;

    for (int query_id = 0; query_id < query_count; ++query_id) {
        string query_type;
        cin >> query_type;

        if (query_type == "BOOK") {
            int64_t time;
            cin >> time;
            string hotel_name;
            cin >> hotel_name;
            int client_id, room_count;
            cin >> client_id >> room_count;
            manager.AddBooking(time, hotel_name, client_id, room_count);
        } else if (query_type == "CLIENTS") {
            string hotel_name;
            cin >> hotel_name;
            cout << manager.Clients(hotel_name) << "\n";
        } else if (query_type == "ROOMS") {
            string hotel_name;
            cin >> hotel_name;
            cout << manager.Rooms(hotel_name) << "\n";
        }
    }
#endif
    return 0;
}