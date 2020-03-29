//
// Created by 刘文景 on 2020-03-25.
//

#include "num2String.h"
#include <iostream>

using namespace AsynLog::Util;

int main(){
    uint32_t u32 = UINT32_MAX;
    int32_t i32 = INT32_MIN;
    uint64_t u64 = UINT64_MAX;
    int64_t i64 = INT64_MAX;
    uint16_t u16 = UINT16_MAX;
    int16_t i16 = INT16_MIN;
    char buf[128] = {0};
    u32toa(u32, buf);
    std::cout<<buf<<std::endl;
    i32toa(i32, buf);
    std::cout<<buf<<std::endl;
    u64toa(u64, buf);
    std::cout<<buf<<std::endl;
    i64toa(i64, buf);
    std::cout<<buf<<std::endl;
    u16toa(u16, buf);
    std::cout<<buf<<std::endl;
    i16toa(i16, buf);
    std::cout<<buf<<std::endl;
}
