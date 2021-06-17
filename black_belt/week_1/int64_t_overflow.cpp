#include "test_runner.h"

#include <optional>
#include <climits>

using namespace std;

optional<int64_t> Sum(int64_t a, int64_t b) {
    if ((a > 0) && (b > INT64_MAX - a)) {
        return nullopt;
    }
    if ((a < 0) && (b < INT64_MIN - a)) {
        return nullopt;
    }

    return a + b;
}

void TestSum() {
    ASSERT(Sum(INT64_MAX, INT64_MAX) == nullopt);
    ASSERT(Sum(INT64_MAX, INT64_MAX - 1) == nullopt);
    ASSERT(Sum(INT64_MAX - 1, INT64_MAX) == nullopt);
    ASSERT(Sum(9000000000000000000, 9000000000000000000) == nullopt);
    ASSERT(Sum(1, INT64_MAX) == nullopt);
    ASSERT(Sum(1, INT64_MAX - 1).value() == INT64_MAX);
    ASSERT(Sum(INT64_MAX, 1) == nullopt);
    ASSERT(Sum(INT64_MAX - 1, 1).value() == INT64_MAX);
    ASSERT(Sum(INT64_MIN, INT64_MAX).value() == -1);
    ASSERT(Sum(INT64_MAX, INT64_MIN).value() == -1);
    ASSERT(Sum(INT64_MIN, INT64_MIN) == nullopt);
    ASSERT(Sum(INT64_MIN, -1) == nullopt);
    ASSERT(Sum(-1, INT64_MIN) == nullopt);
    ASSERT(Sum(INT64_MIN + 1, -1).value() == INT64_MIN);
    ASSERT(Sum(-1, INT64_MIN + 1).value() == INT64_MIN);
}

int main() {
#ifdef LOCAL_BUILD
    TestRunner tr;
    RUN_TEST(tr, TestSum);
#else
    int64_t a, b;
    cin >> a >> b;

    optional<int64_t> c = Sum(a, b);
    if (c == nullopt) {
        cout << "Overflow!";
    } else {
        cout << c.value();
    }

#endif
    return 0;
}