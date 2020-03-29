//
// Created by 刘文景 on 2020-03-28.
//

#include "asynLogging.h"
#include "logFile.h"
#include <stdio.h>
#include <chrono>

using namespace AsynLog;

asynLogging::asynLogging(std::string basename, size_t rollSize,
                         size_t flushIntervalSeconds)
    : flushIntervalSeconds_(flushIntervalSeconds),
      running_(false),
      basename_(std::move(basename)),
      rollSize_(rollSize),
      thread_(std::bind(&asynLogging::threadFunc, this), nullptr, "Logging"),
      waiter_(1),
      mutex_(),
      cond_(),
      currentBuffer_(new Buffer),
      nextBuffer_(new Buffer),
      buffers_() {
  currentBuffer_->setZero();
  nextBuffer_->setZero();
  buffers_.reserve(16);
}

void asynLogging::append(const char *logOneLine, int len) {
    if(len < 0)
        return;
    std::unique_lock<std::mutex> lck(mutex_);
    if(currentBuffer_->avail() >= len){
        currentBuffer_->append(logOneLine, len);
    }
    else{
        buffers_.push_back(std::move(currentBuffer_));
        if(nextBuffer_){
            currentBuffer_ = std::move(nextBuffer_);
        }
        else{
//            currentBuffer_.reset(new Buffer);///if c++14 not supported use this line to replace make_unique
            currentBuffer_ = std::make_unique<Buffer>();///make_unique is new feature in c++14, so
            ///if your system dont support c++14, use reset to replace this
        }
        currentBuffer_->append(logOneLine, static_cast<size_t >(len));
        printf("notify backend thread to work");
        cond_.notify_all();
    }
}

void asynLogging::threadFunc() {
    assert(running_ == true);
    waiter_.finish();
    LogFile logFile(basename_, rollSize_, flushIntervalSeconds_);
    BufferUPtr newBuffer1(new Buffer);
    BufferUPtr newBuffer2(new Buffer);
    newBuffer1->setZero();
    newBuffer2->setZero();
    BufferUPtrVector buffersToWrite;
    buffersToWrite.reserve(16);
    auto sec = std::chrono::seconds(1);
    while(running_){
        assert(newBuffer1 && newBuffer1->length() == 0);
        assert(newBuffer2 && newBuffer2->length() == 0);
        assert(buffersToWrite.empty());
        {
            std::unique_lock<std::mutex> lck(mutex_);
            if(buffers_.empty()){
                cond_.wait_for(lck, flushIntervalSeconds_*sec, [=]{return !buffers_.empty();});
            }
            ///当调用到这里时，不论因为超时还是条件变量的唤醒，都需要讲currentBuffer_中的内容加入
            ///待写入的buffers_
            buffers_.push_back(std::move(currentBuffer_));
            currentBuffer_ = std::move(newBuffer1);
            buffersToWrite.swap(buffers_);
            if(!nextBuffer_)
                newBuffer1 = std::move(newBuffer2);
        }
        if(buffersToWrite.empty())
            continue;
        for(const auto& buffer : buffersToWrite)
            logFile.append(buffer->data(), buffer->length());
        if(buffersToWrite.size() > 2)
            buffersToWrite.resize(2);
        if(!newBuffer1){
            ///if newBuffer1 has been used, we can just reuse buffer in buffersToWrite
            assert(!buffersToWrite.empty());
            newBuffer1 = std::move(buffersToWrite.back());
            buffersToWrite.pop_back();
            newBuffer1->reset();
        }
        if(!newBuffer2){
            ///if newBuffer1 has been used, we can just reuse buffer in buffersToWrite
            assert(!buffersToWrite.empty());
            newBuffer2 = std::move(buffersToWrite.back());
            buffersToWrite.pop_back();
            newBuffer2->reset();
        }
        buffersToWrite.clear();
        logFile.flush();
    }
    logFile.flush();
}
































































