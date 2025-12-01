#pragma once

#include <cstdint>

class MoveAverage {
 public:
  MoveAverage(uint32_t size) : len_(size) {
    p_ = new double[len_];
    for (uint32_t index = 0; index < size; index++) {
      p_[index] = 0;
    }
  }

  ~MoveAverage() {
    delete[] p_;
    p_ = nullptr;
  }

  void Put(double data) {
    average_ -= (p_[index_] / len_);
    average_ += (data / len_);
    p_[index_] = data;
    recv_count_++;
    index_++;
    if (index_ >= len_) {
      index_ = 0;
    }
  }

  double GetAverage() {
    if (recv_count_ < len_) {
      return 0;
    } else {
      return average_;
    }
  }

  uint64_t GetRecvCount(void) { return recv_count_; }

 private:
  uint64_t recv_count_ = 0;
  uint32_t index_ = 0;
  uint32_t len_;
  double average_ = 0;

  double *p_;
};