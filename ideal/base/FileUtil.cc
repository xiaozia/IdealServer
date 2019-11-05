
/******************************************************
*   Copyright (C)2019 All rights reserved.
*
*   Author        : owb
*   Email         : 2478644416@qq.com
*   File Name     : FileUtil.cc
*   Last Modified : 2019-11-05 15:59
*   Describe      :
*
*******************************************************/

#include "ideal/base/FileUtil.h"
#include "ideal/base/Logger.h"
#include "ideal/base/Types.h"

#include <stdio.h>
#include <assert.h>

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

using namespace ideal;

FileUtil::ReadSmallFile::ReadSmallFile(StringArg filename) :
    _fd(::open(filename.c_str(), O_RDONLY | O_CLOEXEC)),
    _err(0) {
    _buf[0] = '\0';
    if(_fd < 0) {
        _err = errno;
    }
}

FileUtil::ReadSmallFile::~ReadSmallFile() {
    if(_fd >= 0) {
        ::close(_fd);
    }
}

template <typename String>
int FileUtil::ReadSmallFile::readToString(int maxSize, 
                                          String* content,
                                          int64_t* fileSize,
                                          int64_t* modifyTime,
                                          int64_t* createTime) {
    static_assert(sizeof(off_t) == 8, "_FILE_OFFSET_BITS = 64");
    assert(content != nullptr);
    int err = _err;
    if(_fd >= 0) {
        content->clear();
        if(fileSize) {
            struct stat statbuf;
            if(::fstat(_fd, &statbuf) == 0) {
                if(S_ISREG(statbuf.st_mode)) {
                    *fileSize = statbuf.st_size;
                    content->reserve(static_cast<int>(std::min(implicit_cast<int64_t>(maxSize), *fileSize)));
                }
                else if(S_ISDIR(statbuf.st_mode)) {
                    err = EISDIR;
                }

                if(modifyTime) {
                    *modifyTime = statbuf.st_mtime;
                }
                if(createTime) {
                    *createTime = statbuf.st_ctime;
                }
            }
            else {
                err = errno;
            }
        }

        while(content->size() <implicit_cast<size_t>(maxSize)) {
            size_t toRead = std::min(implicit_cast<size_t>(maxSize) - content->size(), sizeof _buf);
            ssize_t n = ::read(_fd, _buf, toRead);
            if( n > 0) {
                content->append(_buf, n);
            }
            else {
                if(n < 0) {
                    err = errno;
                }
                break;
            }
        }
    }
    return err;
}

int FileUtil::ReadSmallFile::readToBuffer(int* size) {
    int err = _err;
    if(_fd >= 0) {
        ssize_t n = ::pread(_fd, _buf, sizeof(_buf)-1, 0);
        if(n >= 0) {
            if(size) {
                *size = static_cast<int>(n);
            }
            _buf[n] = '\0';
        }
        else {
            err = errno;
        }
    }
    return err;
}


template int FileUtil::readFile(StringArg filename,
                                int maxSize,
                                std::string* content,
                                int64_t*, int64_t*, int64_t*);

template int FileUtil::ReadSmallFile::readToString(
    int maxSize,
    std::string* content,
    int64_t*, int64_t*, int64_t*);


FileUtil::AppendFile::AppendFile(StringArg filename) : 
    _fp(::fopen(filename.c_str(), "ae")),
    _writtenBytes(0) {
    assert(_fp);
    ::setbuffer(_fp, _buffer, sizeof _buffer);
}

FileUtil::AppendFile::~AppendFile() {
    ::fclose(_fp);
}

void FileUtil::AppendFile::append(const char* logline, size_t len) {
    size_t n =write(logline, len);
    size_t remain = len - n;
    while(remain > 0) {
        size_t x = write(logline + n, remain);
        if(x == 0) {
            int err = ferror(_fp);
            if(err) {
                fprintf(stderr, "AppendFile::append() failed %s\n", strerror_tl(err));
            }
            break;
        }
        n += x;
        remain = len - n;
    }
    _writtenBytes += len;
}

void FileUtil::AppendFile::flush() {
    ::fflush(_fp);
}

size_t FileUtil::AppendFile::write(const char* logline, size_t len) {
    return ::fwrite_unlocked(logline, 1, len, _fp);
}

