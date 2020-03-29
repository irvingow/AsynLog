//
// Created by 刘文景 on 2020-03-23.
//

#include <stdio.h>
#include <sys/syscall.h>
#include <unistd.h>
#include "Thread.h"
#include "currentThread.h"

namespace AsynLog{


namespace CurrentThread {

thread_local uint64_t cachedTid;
thread_local char
    tid2Str[64];  /// put cachedTid in the char array, easy to print
thread_local int tidStrLen;
thread_local const char* threadName = "defaultThreadName";

/// we acquire that pid_t should be defined as int type
// static_assert(std::is_same<int, pid_t>::value, "pid_t type should be int");

}  // namespace CurrentThread

uint64_t gettid() {
#ifdef __linux
  return static_cast<uint64_t>(::syscall(SYS_gettid));
#else  /// in other unix platform
  uint64_t tid;
  pthread_threadid_np(nullptr, &tid);
  return tid;
#endif
}

void CurrentThread::getCacheTid() {
  if (cachedTid == 0) {
    cachedTid = gettid();
    tidStrLen = snprintf(tid2Str, sizeof(tid2Str), "%5llu", cachedTid);
  }
}

bool CurrentThread::isInMainThread() { return tid() == ::getpid(); }

struct ThreadData {
  typedef AsynLog::Thread::ThreadFunc ThreadFunc;
  ThreadFunc func_;
  std::string thread_name_;
  uint64_t* tid_;
  Waiter* waiter_;
  void* user_data_;

  ThreadData(ThreadFunc func, void* user_data, const std::string& name,
             uint64_t* tid, Waiter* waiter)
      : func_(std::move(func)),
        thread_name_(name),
        tid_(tid),
        waiter_(waiter),
        user_data_(user_data) {}

  void runInThread() {
    *tid_ = CurrentThread::tid();
    tid_ = nullptr;
    AsynLog::CurrentThread::threadName =
        thread_name_.empty() ? "Thread" : thread_name_.c_str();
    ///确保先tid和threadName先有效，才能使用tid和threadName，避免出现race
    ///condition
    waiter_->finish();
    waiter_ = nullptr;
    // TODO add prctl to set thread name
    func_(user_data_);
  }
};

void* startThread(void* obj) {
  auto data = static_cast<ThreadData*>(obj);
  data->runInThread();
  delete data;
  return nullptr;
}

std::atomic_int32_t Thread::counts_;

Thread::Thread(AsynLog::Thread::ThreadFunc func, void* user_data,
               std::string name)
    : started_(false),
      joined_(false),
      pthreadID_(0),
      tid_(0),
      func_(std::move(func)),
      thread_name_(std::move(name)),
      waiter_(1),
      user_data_(user_data) {
  setDefaultName();
}

Thread::~Thread() {
  ///如果Thread对象生命周期短于线程，那么就detach线程，如果Thread对象生命周期长于线程
  ///那么可以调用join来等待线程结束，避免资源泄漏
  if (started_ && !joined_) pthread_detach(pthreadID_);
}

void Thread::setDefaultName() {
  ++counts_;
  int num = counts_;
  if (thread_name_.empty()) {
    char buf[32] = {0};
    snprintf(buf, sizeof(buf), "Thread%d", num);
    thread_name_ = buf;
  }
}

void Thread::start() {
  assert(!started_);
  started_ = true;
  ThreadData* data =
      new ThreadData(func_, user_data_, thread_name_, &tid_, &waiter_);
  if (pthread_create(&pthreadID_, nullptr, &startThread, data)) {
    started_ = false;
    delete (data);
  } else {
    waiter_.wait();
    /// wait until tid_ is set properly to avoid race condition
    assert(tid_ > 0);
  }
}

int Thread::join() {
  assert(started_);
  assert(!joined_);
  joined_ = true;
  return pthread_join(pthreadID_, nullptr);
}

}
