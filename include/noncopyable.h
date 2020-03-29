//
// Created by 刘文景 on 2020-03-23.
//

#ifndef ASYNLOG_NONCOPYABLE_H
#define ASYNLOG_NONCOPYABLE_H

namespace AsynLog{

class noncopyable{
 public:
  noncopyable(const noncopyable&) = delete;
  void operator=(const noncopyable&) = delete;
 protected:
  noncopyable() = default;
  ~noncopyable() = default;
};

}

#endif //ASYNLOG_NONCOPYABLE_H
