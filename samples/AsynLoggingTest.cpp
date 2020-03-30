//
// Created by 刘文景 on 2020-03-29.
//

#include <unistd.h>
#include <chrono>
#include <iostream>
#include <sys/time.h>
#include "asynLogging.h"
#include "logging.h"

size_t RollSize = 500 * 1000 * 1000;
AsynLog::asynLogging *g_asynlog = nullptr;

void asynOutput(const char *msg, size_t len) { g_asynlog->append(msg, len); }

void benchTest() {
  AsynLog::Logger::setOutput(asynOutput);
  int cnt = 0, batch = 20000;
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

int64_t getNow()
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    int64_t seconds = tv.tv_sec;
    return (seconds * 1000 * 1000 + tv.tv_usec);
}

void bench(bool longLog)
{
    AsynLog::Logger::setOutput(asynOutput);

    int cnt = 0;
    const int kBatch = 1000;
    std::string empty = " ";
    std::string longStr(3000, 'X');
    longStr += " ";

    for (int t = 0; t < 30; ++t)
    {
        auto start = getNow();
        for (int i = 0; i < kBatch; ++i)
        {
            LOG_INFO << "Hello 0123456789" << " abcdefghijklmnopqrstuvwxyz "
                     << (longLog ? longStr : empty)
                     << cnt;
            ++cnt;
        }
        auto end = getNow();
        printf("%f\n", static_cast<double>(end- start)/1000.0);
        struct timespec ts = { 0, 500*1000*1000 };
        nanosleep(&ts, NULL);
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
