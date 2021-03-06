//
// Created by 刘文景 on 2020-03-26.
//

#include "logStream.h"
#include "num2String.h"
#include <algorithm>

using namespace AsynLog;

namespace AsynLog {

const char digitsHex[] = "0123456789ABCDEF";

size_t convertHex(char buf[], uintptr_t val) {
    uintptr_t temp = val;
    char *p = buf;

    do {
        int lsd = static_cast<int>(temp % 16);
        temp /= 16;
        *p++ = digitsHex[lsd];
    } while (temp != 0);
    *p = '\0';
    std::reverse(buf, p);
    return static_cast<size_t >(p - buf);
}

LogStream &LogStream::operator<<(short val) {
    if (buffer_.avail() < MaxNumericSize)
        return *this;
    char *buf = buffer_.current();
    if (sizeof(short) == 2) {
        auto len = Util::i16toa(val, buf);
        buffer_.add(len);
    } else if (sizeof(short) == 4) {
        auto len = Util::i32toa(val, buf);
        buffer_.add(len);
    } else {
        auto len = Util::i64toa(static_cast<int64_t>(val), buf);
        buffer_.add(len);
    }
    return *this;
}

LogStream &LogStream::operator<<(unsigned short val) {
    if (buffer_.avail() < MaxNumericSize)
        return *this;
    char *buf = buffer_.current();
    if (sizeof(short) == 2) {
        auto len = Util::u16toa(val, buf);
        buffer_.add(len);
    } else if (sizeof(short) == 4) {
        auto len = Util::u32toa(val, buf);
        buffer_.add(len);
    } else {
        auto len = Util::u64toa(static_cast<uint64_t>(val), buf);
        buffer_.add(len);
    }
    return *this;
}

LogStream &LogStream::operator<<(int val) {
    if (buffer_.avail() < MaxNumericSize) {
        printf("buffer space not enough\n");
        return *this;
    }
    char *buf = buffer_.current();
    if (sizeof(int) == 2) {
        auto len = Util::i16toa(val, buf);
        buffer_.add(len);
    } else if (sizeof(int) == 4) {
        auto len = Util::i32toa(val, buf);
        buffer_.add(len);
    } else {
        auto len = Util::i64toa(static_cast<int64_t>(val), buf);
        buffer_.add(len);
    }
    return *this;
}

LogStream &LogStream::operator<<(unsigned int val) {
    if (buffer_.avail() < MaxNumericSize)
        return *this;
    char *buf = buffer_.current();
    if (sizeof(int) == 2) {
        auto len = Util::u16toa(val, buf);
        buffer_.add(len);
    } else if (sizeof(int) == 4) {
        auto len = Util::u32toa(val, buf);
        buffer_.add(len);
    } else {
        auto len = Util::u64toa(static_cast<uint64_t>(val), buf);
        buffer_.add(len);
    }
    return *this;
}

LogStream &LogStream::operator<<(long val) {
    if (buffer_.avail() < MaxNumericSize)
        return *this;
    char *buf = buffer_.current();
    if (sizeof(long) == 2) {
        auto len = Util::i16toa(val, buf);
        buffer_.add(len);
    } else if (sizeof(long) == 4) {
        auto len = Util::i32toa(val, buf);
        buffer_.add(len);
    } else {
        auto len = Util::i64toa(static_cast<int64_t>(val), buf);
        buffer_.add(len);
    }
    return *this;
}

LogStream &LogStream::operator<<(unsigned long val) {
    if (buffer_.avail() < MaxNumericSize)
        return *this;
    char *buf = buffer_.current();
    if (sizeof(long) == 2) {
        auto len = Util::u16toa(val, buf);
        buffer_.add(len);
    } else if (sizeof(long) == 4) {
        auto len = Util::u32toa(val, buf);
        buffer_.add(len);
    } else {
        auto len = Util::u64toa(static_cast<uint64_t>(val), buf);
        buffer_.add(len);
    }
    return *this;
}

LogStream &LogStream::operator<<(long long val) {
    if (buffer_.avail() < MaxNumericSize)
        return *this;
    char *buf = buffer_.current();
    if (sizeof(long long) == 2) {
        auto len = Util::i16toa(val, buf);
        buffer_.add(len);
    } else if (sizeof(long long) == 4) {
        auto len = Util::i32toa(val, buf);
        buffer_.add(len);
    } else {
        auto len = Util::i64toa(static_cast<int64_t>(val), buf);
        buffer_.add(len);
    }
    return *this;
}

LogStream &LogStream::operator<<(unsigned long long val) {
    if (buffer_.avail() < MaxNumericSize)
        return *this;
    char *buf = buffer_.current();
    if (sizeof(long long) == 2) {
        auto len = Util::u16toa(val, buf);
        buffer_.add(len);
    } else if (sizeof(long long) == 4) {
        auto len = Util::u32toa(val, buf);
        buffer_.add(len);
    } else {
        auto len = Util::u64toa(static_cast<uint64_t>(val), buf);
        buffer_.add(len);
    }
    return *this;
}

LogStream &LogStream::operator<<(const void *p) {
    uintptr_t val = reinterpret_cast<uintptr_t >(p);
    if (buffer_.avail() >= MaxNumericSize) {
        char *buf = buffer_.current();
        buf[0] = '0';
        buf[1] = 'x';
        size_t len = convertHex(buf + 2, val);
        buffer_.add(len + 2);
    }
    return *this;
}

///can be optimized, but i think this function may not be
///called so much that to be a performance problem
LogStream &LogStream::operator<<(double val) {
    if (buffer_.avail() >= MaxNumericSize) {
        int len = snprintf(buffer_.current(), MaxNumericSize, "%.12g", val);
        buffer_.add(static_cast<size_t >(len));
    }
    return *this;
}

}  // namespace AsynLog
