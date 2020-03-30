//
// Created by lwj on 2020/3/30.
//

#include <unistd.h>
#include <chrono>
#include <iostream>
#include <sys/time.h>
#include "asynLogging.h"
#include "logging.h"
#include "Thread.h"

size_t RollSize = 500 * 1000 * 1000;
AsynLog::asynLogging *g_asynlog = nullptr;

void asynOutput(const char *msg, size_t len) { g_asynlog->append(msg, len); }

void benchTest(void *thread_num) {
    int threadNum = reinterpret_cast<int>(threadNum);
    AsynLog::Logger::setOutput(asynOutput);
    int cnt = 0, batch = 20000;
    for (int count = 0; count < 50; ++count) {
        for (int i = 0; i < batch; ++i) {
            LOG_INFO << "Hello AsynLog threadNum:" << threadNum << ' ' << cnt;
            ++cnt;
        }
    }
}

int main(int argc, char *argv[]) {
    std::cout << "pid = " << getpid() << std::endl;
    char name[256] = {0};
    strncpy(name, argv[0], sizeof(name) - 1);
    AsynLog::asynLogging asynLog(name, RollSize);
    g_asynlog = &asynLog;
    asynLog.start();
    int nums[] = {1, 2, 3};
    AsynLog::Thread thread1(benchTest, &nums[0], "Loggging Thread1");
    AsynLog::Thread thread2(benchTest, &nums[1], "Loggging Thread2");
    AsynLog::Thread thread3(benchTest, &nums[2], "Loggging Thread3");
    thread1.start(), thread2.start(), thread3.start();
    thread1.join(), thread2.join(), thread3.join();
//    benchTest();
    return 0;
}

