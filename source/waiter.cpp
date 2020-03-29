//
// Created by 刘文景 on 2020-03-23.
//

#include "waiter.h"

using namespace AsynLog;

Waiter::Waiter(int count) : count_(count) {}

void Waiter::wait() {
  std::unique_lock<std::mutex> lck(mutex_);
  while (count_ > 0) condition_.wait(lck);
}

void Waiter::finish() {
  std::unique_lock<std::mutex> uck(mutex_);
  --count_;
  if (count_ == 0) condition_.notify_all();
}

int Waiter::getCount() const {
  std::unique_lock<std::mutex> uck(mutex_);
  return count_;
}
