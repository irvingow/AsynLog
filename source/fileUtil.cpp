//
// Created by 刘文景 on 2020-03-24.
//

#include "fileUtil.h"

using namespace AsynLog;

File::AppendFile::AppendFile(const std::string &file_name)
    : fp_(::fopen(file_name.c_str(), "ae")) , buffer_(), writtenBytes_(0){
  assert(fp_);
  ::setbuffer(fp_, buffer_, sizeof(buffer_));
}

File::AppendFile::~AppendFile() { ::fclose(fp_); }

void File::AppendFile::append(const char *logline, size_t len) {
  size_t cnt = write(logline, len);
  size_t remain = len - cnt;
  while (remain > 0) {
    size_t cur_cnt = write(logline + cnt, remain);
    if (cur_cnt == 0) {
      int err = ferror(fp_);
      if (err) {
        fprintf(stderr, "AppendFile::append() failed %s\n", strerror(err));
      }
      break;
    }
    cnt += cur_cnt;
    remain = len - cnt;
  }
  writtenBytes_ += len;
}

void File::AppendFile::flush() { ::fflush(fp_); }

size_t File::AppendFile::write(const char *logline, size_t len) {
  return ::fwrite(logline, 1, len, fp_);
}

