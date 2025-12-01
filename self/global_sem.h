#ifndef SEMAPHOREPART_H
#define SEMAPHOREPART_H

#include <chrono>
#include <condition_variable>
#include <memory>
#include <mutex>
#include <thread>
#include <unordered_map>

enum SemName { kLoginLogout, kApiRecv, kStrategyRsp, kUpdateInstrumentInfo, kMarketRsp, kTraderRsp };

class OrderSem {
 public:
  explicit OrderSem(void) {}

  void Post() {
    {
      std::lock_guard<std::mutex> lock(mutex_);
      signal_ = true;
    }
    cv_.notify_one();
  }

  void Wait() {
    std::unique_lock<std::mutex> lock(mutex_);
    cv_.wait(lock, [this] { return signal_ == true; });
    signal_ = false;
  }

  int TimeWait(long timeout) {
    std::unique_lock<std::mutex> lock(mutex_);
    if (!cv_.wait_for(lock, std::chrono::seconds(timeout), [this] { return signal_ == true; })) {
      return -1;
    }
    signal_ = false;
    return 0;
  }

 private:
  bool signal_ = false;
  std::mutex mutex_;
  std::condition_variable cv_;
};

struct GlobalSem {
 public:
  GlobalSem();
  GlobalSem(const GlobalSem &) = delete;
  GlobalSem &operator=(const GlobalSem &) = delete;
  static GlobalSem &GetInstance() {
    static GlobalSem instance;
    return instance;
  }
  int WaitSemBySemName(const SemName &name, long secs = 10000);
  void PostSemBySemName(const SemName &name);
  bool ExistSem(const SemName &name);

 private:
  bool AddOrderSem(const SemName &name);
  bool DelOrderSem(const SemName &name);
  std::unordered_map<SemName, std::unique_ptr<OrderSem>> order_sem_map_;
};

#endif
