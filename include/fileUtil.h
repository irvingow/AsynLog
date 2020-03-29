//
// Created by 刘文景 on 2020-03-24.
//

#ifndef ASYNLOG_FILEUTIL_H
#define ASYNLOG_FILEUTIL_H

#include <string>
#include "noncopyable.h"
namespace AsynLog{

namespace File{

class AppendFile : noncopyable{
 public:
  explicit AppendFile(const std::string& file_name);
  ~AppendFile();
  void append(const char* logline, size_t len);
  void flush();
  size_t writtenByes() const {return writtenBytes_;}
 private:
  size_t write(const char* logline, size_t len);
  FILE* fp_;
  char buffer_[64*1024];///64KB buffer as file buffer
  size_t writtenBytes_;
};

}
}

#endif //ASYNLOG_FILEUTIL_H
