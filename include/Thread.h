//
// Created by 刘文景 on 2020-03-23.
//

#ifndef ASYNLOG_THREAD_H
#define ASYNLOG_THREAD_H

#include <pthread.h>
#include <atomic>
#include <functional>
#include <memory>
#include "noncopyable.h"
#include "waiter.h"

namespace AsynLog {

class Thread : noncopyable {
 public:
  typedef std::function<void(void *)> ThreadFunc;
  explicit Thread(ThreadFunc func, void *user_data, std::string name = "");
  ~Thread();
  void start();
  int join();  /// return pthread_join
  bool started() const { return started_; }
  pid_t tid() const { return tid_; }
  const std::string &name() const { return thread_name_; }
  static int getCurCount() { return counts_; }

 private:
  void setDefaultName();
  bool started_;
  bool joined_;
  pthread_t pthreadID_;
  uint64_t tid_;  /// in linux system, every thread also have a system unique
               /// pid(use syscall(SYS_gettid) to get)
  ThreadFunc func_;
  std::string thread_name_;
  Waiter waiter_;
  void* user_data_;

  static std::atomic<int32_t> counts_;
};

}  // namespace AsynLog

#endif  //ASYNLOG_THREAD_H
