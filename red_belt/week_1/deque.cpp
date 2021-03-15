#ifdef LOCAL_BUILD
#include "test_runner.h"
#endif

#include <vector>
#include <stdexcept>

using namespace std;

template <typename T>
class Deque {
public:
    Deque() {}
    bool Empty() const {
        return back_.empty() && front_.empty();
    }
    size_t Size() const {
        return back_.size() + front_.size();
    }

    T& operator [](size_t i) {
        return i < front_.size() ? front_[front_.size() - 1 - i] : back_[i - front_.size()];
    }
    const T& operator [](size_t i) const {
        return i < front_.size() ? front_[front_.size() - 1 - i] : back_[i - front_.size()];
    }

    T& At(size_t i) {
        CheckIndex(i);
        return operator [](i);
    }
    const T& At(size_t i) const {
        CheckIndex(i);
        return operator [](i);
    }

    T& Front() {
        return front_.empty() ? back_.front() : front_.back();
    }
    const T& Front() const {
        return front_.empty() ? back_.front() : front_.back();
    }
    T& Back() {
        return back_.empty() ? front_.front() : back_.back();
    }
    const T& Back() const {
        return back_.empty() ? front_.front() : back_.back();
    }

    void PushBack(const T& value) {
        back_.push_back(value);
    }
    void PushFront(const T& value) {
        front_.push_back(value);
    }

private:
    vector<T> back_;
    vector<T> front_;

    void CheckIndex(size_t i) {
        if (i < 0 || i >= Size()) {
            throw out_of_range("Deque: index is out of range");
        }
    }
};

#ifdef LOCAL_BUILD
void TestDeque() {
    Deque<int> d;
    ASSERT(d.Empty());
    ASSERT_EQUAL(d.Size(), 0u);
    try {
        d.At(0);
    } catch (out_of_range& e) {}

    d.PushBack(10);
    d.PushFront(1);
    ASSERT(!d.Empty());
    ASSERT_EQUAL(d.Size(), 2u);
    ASSERT_EQUAL(d.Front(), 1);
    ASSERT_EQUAL(d.Back(), 10);
    ASSERT_EQUAL(d[0], 1);
    ASSERT_EQUAL(d[1], 10);

    d.PushBack(11);
    d.PushFront(2);

    ASSERT_EQUAL(d[0], 2);
    ASSERT_EQUAL(d[1], 1);
    ASSERT_EQUAL(d[2], 10);
    ASSERT_EQUAL(d[3], 11);
    ASSERT_EQUAL(d.At(0), 2);
    ASSERT_EQUAL(d.At(3), 11);
    try {
        d.At(4);
    } catch (out_of_range& e) {}
    try {
        d.At(-1);
    } catch (out_of_range& e) {}

    Deque<int> d1;
    d1.PushBack(1);
    d1.PushBack(2);
    d1.PushBack(3);
    ASSERT_EQUAL(d1.Front(), 1);
}

int main() {
    TestRunner tr;
    RUN_TEST(tr, TestDeque);
    return 0;
}
#endif
