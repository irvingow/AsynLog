//
// Created by 刘文景 on 2020-03-24.
//

#include "logFile.h"
#include "fileUtil.h"
#include <unistd.h>

using namespace AsynLog;

LogFile::LogFile(std::string basename, size_t rollSize,
                 int flushIntervalSeconds, int checkEveryN)
    : basename_(std::move(basename)),
      rollSize_(rollSize),
      flushIntervalSeconds_(flushIntervalSeconds),
      checkEveryN_(checkEveryN),
      count_(0),
      startOfPeriod_(0),
      lastRoll_(0),
      lastFlush_(0) {
    rollFile();
}

LogFile::~LogFile() = default;

void LogFile::append(const char *logline, size_t len) {
    appendFile_->append(logline, len);
    if(appendFile_->writtenByes() > rollSize_){
        rollFile();
    }
    else{
        ++count_;
        if(count_ >= checkEveryN_){
            count_ = 0;
            time_t now = ::time(nullptr);
            time_t today = now / DaySeconds * DaySeconds;
            if(today != startOfPeriod_)
                rollFile();///means now is a new day
            else if(now - lastFlush_ > flushIntervalSeconds_){
                lastFlush_ = now;
                appendFile_->flush();
            }
        }
    }
}

void LogFile::flush() {
    appendFile_->flush();
}

bool LogFile::rollFile() {
    time_t now = 0;
    std::string newFileName = getLogFileName(basename_, &now);
    time_t today = now / DaySeconds * DaySeconds;
    if(now > lastRoll_){
        lastRoll_ = now;
        lastFlush_ = now;
        startOfPeriod_ = today;
//        appendFile_.reset(new File::AppendFile(newFileName));///if c++14 not supported use this line to replace make_unique
        appendFile_ = std::make_unique<File::AppendFile>(newFileName);///make_unique is new feature in c++14, so
        ///if your system dont support c++14, use reset to replace this
        return true;
    }
    return false;
}

///this function will not be called frequently, so it's
///not necessary to optimize this function
std::string LogFile::getLogFileName(std::string basename, time_t *now) {
    std::string newFileName = std::move(basename);
    char nowDate[32] = {0};
    struct tm t = {0};
    *now = time(nullptr);
    gmtime_r(now, &t);
    strftime(nowDate, sizeof(nowDate), ".%Y%m%d-%H%M%S.", &t);
    newFileName += nowDate;
    char pidbuf[32] = {0};
    snprintf(pidbuf, sizeof(pidbuf), "%d", ::getpid());
    newFileName += pidbuf;
    newFileName += ".log";
    return newFileName;
}



























