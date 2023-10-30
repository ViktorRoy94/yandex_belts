#include <iomanip>
#include <iostream>
#include <vector>
#include <utility>
#include <map>
#include <numeric>

#include "test_runner.h"

using namespace std;

class ReadingManager {
public:
    ReadingManager()
        : user_page_counts_(MAX_USER_COUNT_, -1)
        , page_achived_count_(MAX_USER_PAGES_, 0) {}

    void Read(int user_id, int page_count) {
        for (int i = user_page_counts_[user_id] + 1; i < page_count + 1; i++) {
            page_achived_count_[i]++;
        }
        user_page_counts_[user_id] = page_count;
    }

    double Cheer(int user_id) const {
        int pages_count = user_page_counts_[user_id];
        if (pages_count == -1) {
            return 0;
        }
        int user_count = page_achived_count_[0];
        if (page_achived_count_[0] == 1) {
            return 1;
        }

        return (user_count - page_achived_count_[pages_count]) * 1.0 / (user_count - 1);
    }

private:
    // Статическое поле не принадлежит какому-то конкретному
    // объекту класса. По сути это глобальная переменная,
    // в данном случае константная.
    // Будь она публичной, к ней можно было бы обратиться снаружи
    // следующим образом: ReadingManager::MAX_USER_COUNT.
    static const int MAX_USER_COUNT_ = 1000000;
    static const int MAX_USER_PAGES_ = 1000;

    vector<int> user_page_counts_;
    vector<int> page_achived_count_;
};

void TestReadingManager() {
    ReadingManager manager;

    ostringstream os;
    os << setprecision(6) << manager.Cheer(5) << "\n";
    manager.Read(1, 10);
    os << setprecision(6) << manager.Cheer(1) << "\n";
    manager.Read(2, 5);
    manager.Read(3, 7);
    os << setprecision(6) << manager.Cheer(2) << "\n";
    os << setprecision(6) << manager.Cheer(3) << "\n";
    manager.Read(3, 10);
    os << setprecision(6) << manager.Cheer(3) << "\n";
    manager.Read(3, 11);
    os << setprecision(6) << manager.Cheer(3) << "\n";
    os << setprecision(6) << manager.Cheer(1) << "\n";

    ASSERT_EQUAL(os.str(),
                           "0\n"
                           "1\n"
                           "0\n"
                           "0.5\n"
                           "0.5\n"
                           "1\n"
                           "0.5\n");
}

int main() {
#ifdef LOCAL_BUILD
    TestRunner tr;
    RUN_TEST(tr, TestReadingManager);
#else
    // Для ускорения чтения данных отключается синхронизация
    // cin и cout с stdio,
    // а также выполняется отвязка cin от cout
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    ReadingManager manager;

    int query_count;
    cin >> query_count;

    for (int query_id = 0; query_id < query_count; ++query_id) {
        string query_type;
        cin >> query_type;
        int user_id;
        cin >> user_id;

        if (query_type == "READ") {
            int page_count;
            cin >> page_count;
            manager.Read(user_id, page_count);
        } else if (query_type == "CHEER") {
            cout << setprecision(6) << manager.Cheer(user_id) << "\n";
        }
    }
#endif
    return 0;
}