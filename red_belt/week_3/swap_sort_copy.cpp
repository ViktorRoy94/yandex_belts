#include "test_runner.h"

#include <algorithm>
#include <numeric>
using namespace std;

template <typename T>
void Swap(T* first, T* second) {
    T temp = *first;
    *first = *second;
    *second = temp;
}

template <typename T>
void SortPointers(vector<T*>& pointers) {
    sort(pointers.begin(), pointers.end(), [](T* l, T* r){ return *l < *r;});
}

template <typename T>
void ReversedCopy(T* source, size_t count, T* destination) {
    T* source_begin = source;
    T* source_end = source + count;
    T* dest_begin = destination;
    T* dest_end = destination + count;
    if (dest_begin >= source_end || dest_end  <= source_begin) {
        reverse_copy(source_begin, source_end, dest_begin);
    } else if (dest_begin > source_begin) {
        for(size_t i = 0; source_begin + i < dest_begin; i++) {
            *(dest_end - 1 - i) = source_begin[i];
        }
        reverse(dest_begin, source_end);
    } else {
        for(size_t i = 0; dest_begin + i < source_begin; i++) {
            dest_begin[i] = *(source_end - 1 - i);
        }
        reverse(source_begin, dest_end);
    }
}

void TestSwap() {
    int a = 1;
    int b = 2;
    Swap(&a, &b);
    ASSERT_EQUAL(a, 2);
    ASSERT_EQUAL(b, 1);

    string h = "world";
    string w = "hello";
    Swap(&h, &w);
    ASSERT_EQUAL(h, "hello");
    ASSERT_EQUAL(w, "world");
}

void TestSortPointers() {
    int one = 1;
    int two = 2;
    int three = 3;

    vector<int*> pointers;
    pointers.push_back(&two);
    pointers.push_back(&three);
    pointers.push_back(&one);

    SortPointers(pointers);

    ASSERT_EQUAL(pointers.size(), 3u);
    ASSERT_EQUAL(*pointers[0], 1);
    ASSERT_EQUAL(*pointers[1], 2);
    ASSERT_EQUAL(*pointers[2], 3);
}

void TestReverseCopy() {
    const size_t count = 7;

    int* source = new int[count];
    int* dest = new int[count];

    for (size_t i = 0; i < count; ++i) {
        source[i] = i + 1;
    }
    ReversedCopy(source, count, dest);
    const vector<int> expected1 = {7, 6, 5, 4, 3, 2, 1};
    ASSERT_EQUAL(vector<int>(dest, dest + count), expected1);

    // Области памяти могут перекрываться
    ReversedCopy(source, count - 1, source + 1);
    const vector<int> expected2 = {1, 6, 5, 4, 3, 2, 1};
    ASSERT_EQUAL(vector<int>(source, source + count), expected2);

    delete[] dest;
    delete[] source;
}

int main() {
    TestRunner tr;
    RUN_TEST(tr, TestSwap);
    RUN_TEST(tr, TestSortPointers);
    RUN_TEST(tr, TestReverseCopy);
    return 0;
}
