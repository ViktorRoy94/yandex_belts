#pragma once

#include <cstdlib>
#include <algorithm>

// Реализуйте шаблон SimpleVector
template <typename T>
class SimpleVector {
public:
    SimpleVector();
    explicit SimpleVector(size_t size);

    SimpleVector(const SimpleVector& other);
    SimpleVector(SimpleVector&& other);

    void operator=(const SimpleVector& other);
    void operator=(SimpleVector&& other);

    ~SimpleVector();

    T& operator[](size_t index);

    T* begin();
    T* end();

    const T* begin() const;
    const T* end() const;

    size_t Size() const;
    size_t Capacity() const;
    void PushBack(const T& value);

private:
    T* data_;
    size_t size_;
    size_t capacity_;
};

template <typename T>
SimpleVector<T>::SimpleVector()
    : data_(nullptr)
    , size_(0)
    , capacity_(0)
{}

template <typename T>
SimpleVector<T>::SimpleVector(size_t size)
    : data_(new T[size])
    , size_(size)
    , capacity_(size)
{}

template <typename T>
SimpleVector<T>::SimpleVector(const SimpleVector<T>& other)
    : data_(new T[other.capacity_])
    , size_(other.size_)
    , capacity_(other.capacity_)
{
    std::copy(other.begin(), other.end(), begin());
}

template <typename T>
SimpleVector<T>::SimpleVector(SimpleVector<T>&& other)
    : data_(other.data_)
    , size_(other.size_)
    , capacity_(other.capacity_)
{
    other.data_ = nullptr;
}

template <typename T>
void SimpleVector<T>::operator=(const SimpleVector<T>& other)
{
    if (other.size_ <= capacity_) {
      std::copy(other.begin(), other.end(), begin());
      size_ = other.size_;
    } else {
      SimpleVector<T> tmp(other);
      std::swap(tmp.data_, data_);
      std::swap(tmp.size_, size_);
      std::swap(tmp.capacity_, capacity_);
    }
}

template <typename T>
void SimpleVector<T>::operator=(SimpleVector<T>&& other)
{
    delete [] data_;
    data_ = other.data_;
    size_ = other.size_;
    capacity_ = other.capacity_;

    other.data_ = nullptr;
    other.size_ = 0;
    other.capacity_ = 0;
}

template <typename T>
SimpleVector<T>::~SimpleVector()
{
    delete [] data_;
}

template <typename T>
T& SimpleVector<T>::operator[](size_t index)
{
    return data_[index];
}

template <typename T>
T* SimpleVector<T>::begin()
{
    return data_;
}

template <typename T>
T* SimpleVector<T>::end()
{
    return data_ + size_;
}

template <typename T>
const T* SimpleVector<T>::begin() const
{
    return data_;
}

template <typename T>
const T* SimpleVector<T>::end() const
{
    return data_ + size_;
}

template <typename T>
size_t SimpleVector<T>::Size() const
{
    return size_;
}

template <typename T>
size_t SimpleVector<T>::Capacity() const
{
    return capacity_;
}

template <typename T>
void SimpleVector<T>::PushBack(const T& value)
{
    if (size_ == capacity_) {
        size_t new_capacity = capacity_ == 0 ? 1 : 2 * capacity_;
        T* new_data = new T[new_capacity];
        for (size_t i = 0; i < size_; i++) {
            new_data[i] = data_[i];
        }
        delete [] data_;
        data_ = new_data;
        capacity_ = new_capacity;
    }
    data_[size_++] = value;
}
