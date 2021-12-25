#pragma once
#include <cstddef>
#include <iostream>

namespace RAII {
  template<typename Provider>
  class Booking {
  public:
    Booking(Provider* provider, int counter) : provider_(provider), counter_(counter) {
    }
    Booking(const Booking&) = delete;
    Booking(Booking&& other) : provider_(other.provider_), counter_(other.counter_) {
      other.provider_ = nullptr;
    }
    Booking& operator=(const Booking&) = delete;
    Booking& operator=(Booking&& other) {
      delete provider_;
      provider_ = other.provider_;
      counter_ = other.counter_;
      other.provider_ = nullptr;
      other.counter_ = 0;
      return *this;
    }

    ~Booking() {
      if (provider_)
        provider_->CancelOrComplete(*this);
    }
  private:
    Provider* provider_;
    int counter_;
  };
}
