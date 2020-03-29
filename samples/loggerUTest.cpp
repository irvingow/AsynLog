//
// Created by 刘文景 on 2020-03-27.
//

#include <chrono>
#include <iostream>
#include <thread>
#include "Thread.h"
#include "currentThread.h"
#include "logging.h"
#include "waiter.h"

using AsynLog::Thread;
using AsynLog::Waiter;

void func1(void* obj) {
  LOG_WARN << "thread id:"
            << std::string(AsynLog::CurrentThread::tidString(),
                           AsynLog::CurrentThread::getTidStrLength());
  LOG_INFO << "thread name:" << AsynLog::CurrentThread::threadName;
  Waiter* waiter = static_cast<Waiter*>(obj);
  LOG_INFO << "func1 finished" ;
  waiter->finish();
}

void func2(void* obj) {
    LOG_DEBUG << "thread id:"
             << std::string(AsynLog::CurrentThread::tidString(),
                            AsynLog::CurrentThread::getTidStrLength());
    LOG_INFO << "thread name:" << AsynLog::CurrentThread::threadName;
  Waiter* waiter = static_cast<Waiter*>(obj);
  std::this_thread::sleep_for(std::chrono::milliseconds(100));
    LOG_INFO << "func2 finished" ;
  waiter->finish();
}

void func3(void* obj) {
    LOG_ERROR << "thread id:"
             << std::string(AsynLog::CurrentThread::tidString(),
                            AsynLog::CurrentThread::getTidStrLength());
    LOG_INFO << "thread name:" << AsynLog::CurrentThread::threadName;
  Waiter* waiter = static_cast<Waiter*>(obj);
  waiter->wait();
    LOG_INFO << "func3 finished" ;
}

int main() {
  Waiter waiter(2);
  Thread thread1(func1, &waiter, "thread1");
  Thread thread2(func2, &waiter, "thread2");
  {
    Thread thread3(func3, &waiter, "thread3");
    thread3.start();
  }
  thread1.start();
  thread2.start();
  thread1.join();
  thread2.join();
  AsynLog::Logger::setLogLevel(AsynLog::Logger::TRACE);
  LOG_TRACE << "main thread ended";
  return 0;
}
