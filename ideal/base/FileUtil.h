
/******************************************************
*   Copyright (C)2019 All rights reserved.
*
*   Author        : owb
*   Email         : 2478644416@qq.com
*   File Name     : FileUtil.h
*   Last Modified : 2019-06-06 20:20
*   Describe      :
*
*******************************************************/

#ifndef  _IDEAL_BASE_FILEUTIL_H
#define  _IDEAL_BASE_FILEUTIL_H

#include "ideal/base/noncopyable.h"
#include "ideal/base/StringPiece.h"

namespace ideal {

namespace FileUtil {

class ReadSmallFile : public noncopyable {
public:
    ReadSmallFile(StringArg filename);
    ~ReadSmallFile();

    template <typename String>
    int readToString(int maxSize, 
                     String* content,
                     int64_t* fileSize,
                     int64_t* modifyTime,
                     int64_t* createTime);

    int readToBuffer(int* size);

    const char* buffer() const { return _buf; }

    static const int kBufferSize = 64 * 1024;

private:
    int _fd;
    int _err;
    char _buf[kBufferSize];
};

template <typename String>
int readFile(StringArg filename,
             int maxSize,
             String* content,
             int64_t* fileSize = nullptr,
             int64_t* modifyTime = nullptr,
             int64_t* createTime = nullptr) {
    ReadSmallFile file(filename);
    return file.readToString(maxSize, content, fileSize, modifyTime, createTime);
}


class AppendFile : public noncopyable {
public:
    explicit AppendFile(StringArg filename);
    ~AppendFile();

    void append(const char* logline, size_t len);
    void flush();
    off_t writtenBytes() const  { return _writtenBytes; }

private:
    size_t write(const char* logline, size_t len);

private:
    FILE* _fp;
    char _buffer[64 * 1024];
    off_t _writtenBytes;
};

}

}

#endif // _IDEAL_BASE_FILEUTIL_H


