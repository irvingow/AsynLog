//
// Created by 刘文景 on 2020-03-23.
//

#ifndef ASYNLOG_WAITER_H
#define ASYNLOG_WAITER_H

#include <condition_variable>
#include <mutex>
#include "noncopyable.h"

namespace AsynLog {

///辅助类，要求count个数量的事件必须完成，才能进行下一步
class Waiter : noncopyable {
 public:
  explicit Waiter(int count);

  void wait();

  void finish();

  int getCount() const;

 private:
  mutable std::mutex mutex_;
  std::condition_variable condition_;
  int count_;
};

}  // namespace AsynLog

#endif  // ASYNLOG_WAITER_H
