//
// Created by 刘文景 on 2020-03-26.
//

#include "logging.h"
#include "currentThread.h"
#include <errno.h>
#include <time.h>
#include <sys/time.h>
#include <inttypes.h>

namespace AsynLog {

int64_t MicroSecondsPerSecond = 1000 * 1000;

thread_local char t_errnobuf[512] = {0};
thread_local char t_time[64] = {0};
thread_local int t_time_len = 0;
thread_local time_t t_lastSecond;

Logger::LogLevel initAlogLevel() {
#ifdef ALogTrace
    return Logger::TRACE;
#endif
#ifdef ALogDebug
    return Logger::DEBUG;
#endif
    return Logger::INFO;
}

Logger::LogLevel AlogLevel = initAlogLevel();

const char *LogLevelNameSameLen[Logger::NUM_LOG_LEVELS] = {
    " TRACE ", " DEBUG ", " INFO  ", " WARN  ", " ERROR ", " FATAL ",
};

/// similar to class PreDefineMacroHelper,
/// if we know the length of the str, so we can
/// use this class to optimize
class FixedLenCharArr {
 public:
  FixedLenCharArr(const char *str, size_t len) : str_(str), len_(len) {}
  const char *str_;
  const size_t len_;
};

LogStream &operator<<(LogStream &logStream, FixedLenCharArr fixedLenCharArr) {
    logStream.append(fixedLenCharArr.str_, fixedLenCharArr.len_);
    return logStream;
}

LogStream &operator<<(LogStream &logStream,
                      Logger::PreDefineMacroHelper preDefineMacroHelper) {
    logStream.append(preDefineMacroHelper.data_, preDefineMacroHelper.size_);
    return logStream;
}

void defaultOutput(const char *data, size_t len) {
    fwrite(data, 1, len, stdout);
}

void defaultFlush() { fflush(stdout); }

Logger::OutputFunc AOutput = defaultOutput;
Logger::FlushFunc AFlush = defaultFlush;

Logger::Logger(AsynLog::Logger::PreDefineMacroHelper fileName,
               AsynLog::Logger::PreDefineMacroHelper functionName, int line,
               AsynLog::Logger::LogLevel level)
    : insideHelper_(fileName, functionName, line, level, 0) {}

Logger::Logger(AsynLog::Logger::PreDefineMacroHelper fileName,
               AsynLog::Logger::PreDefineMacroHelper functionName, int line,
               bool toAbort)
    : insideHelper_(fileName, functionName, line, toAbort ? FATAL : ERROR,
                    errno) {}

//Logger::Logger(const char *fileName,
//               const char *functionName,
//               int line,
//               AsynLog::Logger::LogLevel level) :insideHelper_(fileName, functionName, line, level, 0) {}

Logger::~Logger() {
    insideHelper_.finish();
    const LogStream::AppendBuffer &buffer(stream().getAppendBuffer());
    AOutput(buffer.data(), buffer.length());
    if (insideHelper_.level_ == FATAL) {
        AFlush();
        abort();
    }
}

void Logger::setOutput(AsynLog::Logger::OutputFunc outputFunc) {
    AOutput = outputFunc;
}

void Logger::setFlush(AsynLog::Logger::FlushFunc flushFunc) {
    AFlush = flushFunc;
}

int64_t getNowTime() {
    struct timeval tv = {0};
    gettimeofday(&tv, NULL);
    int64_t seconds = tv.tv_sec, microSeconds = tv.tv_usec;
    return seconds * MicroSecondsPerSecond + microSeconds;
}

Logger::InsideHelper::InsideHelper(
    AsynLog::Logger::PreDefineMacroHelper fileName,
    AsynLog::Logger::PreDefineMacroHelper functionName, int line,
    AsynLog::Logger::LogLevel level, int last_errno)
    : fileName_(fileName),
      functionName_(functionName),
      line_(line),
      level_(level),
      curtime_(getNowTime()) {
    formatTime();
    CurrentThread::tid();
    logStream_ << FixedLenCharArr(CurrentThread::tidString(), CurrentThread::getTidStrLength());
    logStream_ << FixedLenCharArr(LogLevelNameSameLen[level], 7);
    if (last_errno != 0)
        logStream_ << " error = " << strerror(last_errno);
}

//Logger::InsideHelper::InsideHelper(const char *fileName,
//                                   const char *functionName,
//                                   int line,
//                                   AsynLog::Logger::LogLevel level,
//                                   int) {}

///we use thread_cached seconds to avoid format time
///when logs are generated in the same second
void Logger::InsideHelper::formatTime() {
    int64_t seconds = curtime_ / MicroSecondsPerSecond;
    int64_t microSeconds = curtime_ % MicroSecondsPerSecond;
    if (seconds != t_lastSecond) {
        t_lastSecond = seconds;
        struct tm tm_time = {0};
        ::gmtime_r(&seconds, &tm_time);
        t_time_len = snprintf(t_time, sizeof(t_time), "%4d%02d%02d %02d:%02d:%02d",
                              tm_time.tm_year + 1900, tm_time.tm_mon + 1, tm_time.tm_mday,
                              tm_time.tm_hour, tm_time.tm_min, tm_time.tm_sec);
    }
//    char buf[12] = {0};
//    int len = snprintf(buf, sizeof(buf), ".%06" PRId64, microSeconds);
//    if (len < 0) {
//        logStream_ << "snprintf error = " << strerror(errno);
//        return;
//    }
//    logStream_ << FixedLenCharArr(t_time, t_time_len) << FixedLenCharArr(buf, len) << ' ';
    logStream_ << FixedLenCharArr(t_time, t_time_len) << '.' << microSeconds << ' ';
}

void Logger::InsideHelper::finish() {
    logStream_ << ' ' << fileName_ << ':' << functionName_ << ':' << line_ << '\n';
}

}  // namespace AsynLog
