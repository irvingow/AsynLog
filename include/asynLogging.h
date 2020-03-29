//
// Created by 刘文景 on 2020-03-28.
//

#ifndef ASYNLOG_ASYNLOGGING_H
#define ASYNLOG_ASYNLOGGING_H

#include <atomic>
#include <vector>
#include <mutex>
#include <thread>
#include <condition_variable>
#include "waiter.h"
#include "logStream.h"
#include "Thread.h"

namespace AsynLog{

class asynLogging : noncopyable{
 public:
  asynLogging(std::string basename, size_t rollSize, size_t flushIntervalSeconds = 3);
  ~asynLogging(){
      if(running_)
          stop();
  }
  void append(const char* logOneLine, int len);
  void start(){
      running_ = true;
      thread_.start();
      waiter_.wait();
  }
  void stop(){
      running_ = false;
      cond_.notify_all();
      thread_.join();
  }
 private:
  void threadFunc();

  typedef Util::FixedSizeBuffer<Util::LargeBuffer> Buffer;
  typedef std::vector<std::unique_ptr<Buffer>> BufferUPtrVector;
  typedef BufferUPtrVector::value_type BufferUPtr;

  const int flushIntervalSeconds_;
  std::atomic_bool running_;
  const std::string basename_;
  const size_t rollSize_;
  AsynLog::Thread thread_;
  AsynLog::Waiter waiter_;
  std::mutex mutex_;
  std::condition_variable cond_;
  BufferUPtr currentBuffer_;
  BufferUPtr nextBuffer_;
  BufferUPtrVector buffers_;

};

}

#endif //ASYNLOG_ASYNLOGGING_H
