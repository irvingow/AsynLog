//
// Created by 刘文景 on 2020-03-25.
//

#ifndef ASYNLOG_NUM2STRING_H
#define ASYNLOG_NUM2STRING_H

#pragma once

#include <stdint.h>

namespace AsynLog {

namespace Util{


/// Conversion unsigned integer to string \p to , and return string length.
void u16toa(uint16_t number, char *to);
void u32toa(uint32_t number, char *to);
void u64toa(uint64_t number, char *to);

/// Conversion signed integer to string \p to , and return string length.
void i16toa(int16_t number, char *to);
void i32toa(int32_t number, char *to);
void i64toa(int64_t number, char *to);

}

}

#endif //ASYNLOG_NUM2STRING_H
