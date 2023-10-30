#pragma once

#include <stdexcept>
#include <array>
using namespace std;

template <typename T, size_t N>
class StackVector {
public:
    explicit StackVector(size_t a_size = 0);

    T& operator[](size_t index);
    const T& operator[](size_t index) const;

    auto begin();
    auto end();
    auto begin() const;
    auto end() const;

    size_t Size() const;
    size_t Capacity() const;

    void PushBack(const T& value);
    T PopBack();

private:
    array<T, N> data_;
    size_t size_ = 0;
};

template <typename T, size_t N>
StackVector<T, N>::StackVector(size_t a_size) : size_(a_size){
    if (size_ > data_.size()) {
        throw invalid_argument("");
    }
}

template <typename T, size_t N>
T& StackVector<T, N>::operator[](size_t index) {
    return data_[index];
}

template <typename T, size_t N>
const T& StackVector<T, N>::operator[](size_t index) const {
    return data_[index];
}

template <typename T, size_t N>
auto StackVector<T, N>::begin() {
    return data_.begin();
}

template <typename T, size_t N>
auto StackVector<T, N>::end() {
    return data_.begin() + size_;
}

template <typename T, size_t N>
auto StackVector<T, N>::begin() const {
    return data_.begin();
}

template <typename T, size_t N>
auto StackVector<T, N>::end() const {
    return data_.begin() + size_;
}

template <typename T, size_t N>
size_t StackVector<T, N>::Size() const {
    return size_;
}

template <typename T, size_t N>
size_t StackVector<T, N>::Capacity() const {
    return data_.size();
}

template <typename T, size_t N>
void StackVector<T, N>::PushBack(const T& value) {
    if (size_ < data_.size()) {
        data_[size_] = value;
        size_++;
    } else {
        throw overflow_error("");
    }
}

template <typename T, size_t N>
T StackVector<T, N>::PopBack() {
    if (size_ > 0) {
        return data_[--size_];
    } else {
        throw underflow_error("");
    }
}
