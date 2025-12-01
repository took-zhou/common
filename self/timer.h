#pragma once

#include <chrono>
#include <iostream>
#include <string>

#if 0
#define BENCH_TEST
#endif

class Timer {
 public:
  Timer() { Reset(); }

  void Reset() { start_ = std::chrono::high_resolution_clock::now(); }

  float Elapsed() {
    return std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now() - start_).count() * 0.001f *
           0.001f * 0.001f;
  }

  float ElapsedMillis() { return Elapsed() * 1000.0f; }

 private:
  std::chrono::time_point<std::chrono::high_resolution_clock> start_;
};

class ScopedTimer {
 public:
  ScopedTimer(const std::string& name) : name_(name) {}
  ~ScopedTimer() {
    float time = timer_.ElapsedMillis();
    std::cout << "[TIMER] " << name_ << " - " << time << "ms\n";
  }

 private:
  std::string name_;
  Timer timer_;
};
