//
// Created by 刘文景 on 2020-03-23.
//

#ifndef ASYNLOG_CURRENTTHREAD_H
#define ASYNLOG_CURRENTTHREAD_H

namespace AsynLog{

namespace CurrentThread{

extern thread_local uint64_t cachedTid;
extern thread_local char tid2Str[64];///put cachedTid in the char array, easy to print
extern thread_local int tidStrLen;
extern thread_local const char* threadName;
void getCacheTid();


inline int tid(){
    if(cachedTid == 0)
        getCacheTid();
    return cachedTid;
}

inline const char* tidString(){
    return tid2Str;
}

inline int getTidStrLength(){
    return tidStrLen;
}

inline const char* name(){
    return threadName;
}

bool isInMainThread();

}
}

#endif //ASYNLOG_CURRENTTHREAD_H
