//
// Created by 刘文景 on 2020-03-29.
//

#include <unistd.h>
#include <chrono>
#include <iostream>
#include "asynLogging.h"
#include "logging.h"

size_t RollSize = 500 * 1000 * 1000;
AsynLog::asynLogging *g_asynlog = nullptr;

void asynOutput(const char *msg, size_t len) { g_asynlog->append(msg, len); }

void benchTest() {
  AsynLog::Logger::setOutput(asynOutput);
  int cnt = 0, batch = 2000;
  for (int count = 0; count < 30; ++count) {
    std::chrono::steady_clock::time_point start =
        std::chrono::steady_clock::now();
    for (int i = 0; i < batch; ++i) {
      LOG_INFO << "Hello AsynLog " << cnt;
      ++cnt;
    }
    std::chrono::steady_clock::time_point end =
        std::chrono::steady_clock::now();
    std::chrono::steady_clock::duration timeCustum =
        std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    std::cout<<"it need " << timeCustum.count() << " microSeconds to run a batch" << std::endl;
  }
}

int main(int argc, char *argv[]) {
  std::cout << "pid = " << getpid() << std::endl;
  char name[256] = {0};
  strncpy(name, argv[0], sizeof(name) - 1);
  AsynLog::asynLogging asynLog(name, RollSize);
  g_asynlog = &asynLog;
  asynLog.start();
  benchTest();
  return 0;
}
