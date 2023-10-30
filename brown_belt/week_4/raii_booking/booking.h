#pragma once
#include <stdexcept>

namespace RAII {

template<typename Provider>
class Booking {
private:
    using BookingId = typename Provider::BookingId;

public:
    Booking(Provider* provider, const BookingId& id)
        : provider_(provider)
        , booking_id_(id)
    {}
    Booking(const Booking&) = delete;
    Booking(Booking&& other)
        : provider_(other.provider_)
        , booking_id_(other.booking_id_)
    {
        other.provider_ = nullptr;
    }

    Booking& operator=(const Booking&) = delete;
    Booking& operator=(Booking&& other)
    {
        provider_ = other.provider_;
        booking_id_ = other.booking_id_;
        other.provider_ = nullptr;
        return *this;
    }

    ~Booking()
    {
        if (provider_) {
            provider_->CancelOrComplete(*this);
        }
    }
private:
    Provider* provider_;
    BookingId booking_id_;
};

}