#include "test_runner.h"

#include <algorithm>
#include <iostream>
#include <string>
#include <queue>
#include <stdexcept>
#include <set>
using namespace std;

template <class T>
class ObjectPool {
public:
    T* Allocate() {
        T* obj;
        if (free_.empty()) {
            obj = new T;
        } else {
            obj = free_.front();
            free_.pop();
        }
        allocated_.insert(obj);
        return obj;
    }
    T* TryAllocate() {
        if (free_.empty()) {
            return nullptr;
        } else {
            return Allocate();
        }
    }

    void Deallocate(T* object) {
        if (allocated_.count(object) > 0) {
            free_.push(object);
            allocated_.erase(object);
        } else {
            throw invalid_argument("No such object in Pool");
        }
    }

    ~ObjectPool() {
        for (auto& obj : allocated_) {
            delete obj;
        }
        while(!free_.empty()) {
            delete free_.front();
            free_.pop();
        }
    }

private:
    set<T*> allocated_;
    queue<T*> free_;
};

void TestObjectPool() {
    ObjectPool<string> pool;

    auto p1 = pool.Allocate();
    auto p2 = pool.Allocate();
    auto p3 = pool.Allocate();

    *p1 = "first";
    *p2 = "second";
    *p3 = "third";

    pool.Deallocate(p2);
    ASSERT_EQUAL(*pool.Allocate(), "second");

    pool.Deallocate(p3);
    pool.Deallocate(p1);

    ASSERT_EQUAL(*pool.Allocate(), "third");
    ASSERT_EQUAL(*pool.Allocate(), "first");

    pool.Deallocate(p1);
}

int main() {
    TestRunner tr;
    RUN_TEST(tr, TestObjectPool);
    return 0;
}
