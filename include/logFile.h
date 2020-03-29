//
// Created by 刘文景 on 2020-03-24.
//

#ifndef ASYNLOG_LOGFILE_H
#define ASYNLOG_LOGFILE_H

#include <cstdio>
#include "noncopyable.h"
#include <string>
#include <mutex>
#include <memory>
namespace AsynLog {

namespace File {

class AppendFile;

}

class LogFile : noncopyable{
 public:
  LogFile(std::string basename, size_t rollSize, int flushIntervalSeconds, int checkEveryN = 1024);
  ~LogFile();
  void append(const char* logline, size_t len);///not thread_safe
  void flush();
  bool rollFile();
 private:
  static std::string getLogFileName(std::string basename, time_t* now);
  const std::string basename_;
  const size_t rollSize_;
  const int flushIntervalSeconds_;
  const int checkEveryN_;

  int count_;///records the number of @func append called after last roll

  time_t startOfPeriod_;
  time_t lastRoll_;
  time_t lastFlush_;
  std::unique_ptr<File::AppendFile> appendFile_;
  const static int DaySeconds = 24 * 60 * 60 ;
};

}  // namespace AsynLog

#endif  // ASYNLOG_LOGFILE_H
