#include <memory>

#include "common/self/global_sem.h"

GlobalSem::GlobalSem() {
  AddOrderSem(kLoginLogout);
  AddOrderSem(kApiRecv);
  AddOrderSem(kStrategyRsp);
  AddOrderSem(kUpdateInstrumentInfo);
  AddOrderSem(kMarketRsp);
  AddOrderSem(kTraderRsp);
};

bool GlobalSem::AddOrderSem(const SemName &name) {
  if (order_sem_map_.find(name) == order_sem_map_.end()) {
    order_sem_map_[name] = std::make_unique<OrderSem>();
  }
  return true;
}

bool GlobalSem::DelOrderSem(const SemName &name) {
  bool ret = false;

  if (order_sem_map_.find(name) == order_sem_map_.end()) {
    ret = false;
  } else {
    ret = true;
    order_sem_map_.erase(name);
  }

  return ret;
}

int GlobalSem::WaitSemBySemName(const SemName &name, long secs) {
  int res = 0;
  auto pos = order_sem_map_.find(name);
  if (pos != order_sem_map_.end()) {
    res = pos->second->TimeWait(secs);
  }
  return res;
}

void GlobalSem::PostSemBySemName(const SemName &name) {
  auto pos = order_sem_map_.find(name);
  if (pos != order_sem_map_.end()) {
    pos->second->Post();
    return;
  }
}

bool GlobalSem::ExistSem(const SemName &name) {
  bool ret = false;
  auto pos = order_sem_map_.find(name);
  if (pos != order_sem_map_.end()) {
    ret = true;
  }

  return ret;
}
