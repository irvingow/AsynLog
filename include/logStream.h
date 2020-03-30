//
// Created by 刘文景 on 2020-03-25.
//

#ifndef ASYNLOG_LOGSTREAM_H
#define ASYNLOG_LOGSTREAM_H

#include <cassert>
#include <cstring>
#include <string>
#include "noncopyable.h"

namespace AsynLog {

namespace Util {

const int SmallBuffer = 4000;
const int LargeBuffer = 4000 * 1000;

template <int SIZE>
class FixedSizeBuffer : noncopyable {
 public:
  FixedSizeBuffer() : cur_(data_), written_(0) {}
  ~FixedSizeBuffer() = default;
  void append(const char* buf, size_t len) {
    /// if len is too big, we just append partially
    int size = std::min(len, avail());
    memmove(cur_, buf, size);
    cur_ += size;
    written_ += size;
  }
  const char* data() const { return data_; }
  size_t length() const { return written_; }
  char* current() const { return cur_; }
  size_t avail() const { return SIZE - written_; }
  void add(size_t len) { cur_ += len; written_ += len;}
  void reset() { cur_ = data_; written_ = 0;}
  void setZero() { bzero(data_, sizeof(data_)); }
  std::string toString() const { return std::string(data_, length()); }
  const char* peekBuffer() const{
    *cur_ = '\0';
    return data_;
  };

 private:
  const char* end() const { return data_ + sizeof(data_); }
  char data_[SIZE] = {0};
  char* cur_;
  int64_t written_;
};

}  // namespace Util

class LogStream : noncopyable {
 public:
  typedef LogStream self;
  typedef Util::FixedSizeBuffer<Util::SmallBuffer> AppendBuffer;

 public:
  self& operator<<(bool v) {
    buffer_.append(v ? "1" : "0", 1);
    return *this;
  }
  self& operator<<(short val);
  self& operator<<(unsigned short val);
  self& operator<<(int val);
  self& operator<<(unsigned int val);
  self& operator<<(long val);
  self& operator<<(unsigned long val);
  self& operator<<(long long val);
  self& operator<<(unsigned long long val);
  self& operator<<(const void* p);
  self& operator<<(float val) {
    *this << static_cast<double>(val);
    return *this;
  }
  self& operator<<(double val);
  self& operator<<(const char val) {
    buffer_.append(&val, 1);
    return *this;
  }
  self& operator<<(const char* str) {
    if (str) {
      buffer_.append(str, strlen(str));
    } else {
      buffer_.append("(null)", 6);
    }
    return *this;
  }
  self& operator<<(const unsigned char* str) {
    return operator<<(reinterpret_cast<const char*>(str));
  }
  self& operator<<(const std::string& str) {
    buffer_.append(str.data(), static_cast<size_t>(str.size()));
    return *this;
  }
  void append(const char* data, size_t len) { buffer_.append(data, len); }
  const AppendBuffer& getAppendBuffer() const { return buffer_; }
  void resetAppendBuffer() { buffer_.reset(); }

 private:
  AppendBuffer buffer_;
  static const int MaxNumericSize = 32;
};

}  // namespace AsynLog
#endif  // ASYNLOG_LOGSTREAM_H
