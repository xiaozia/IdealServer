
/******************************************************
*   Copyright (C)2019 All rights reserved.
*
*   Author        : owb
*   Email         : 2478644416@qq.com
*   File Name     : StringPiece.h
*   Last Modified : 2019-11-23 14:12
*   Describe      :
*
*******************************************************/

#ifndef  _IDEAL_BASE_STRINGPIECE_H
#define  _IDEAL_BASE_STRINGPIECE_H

#include "ideal/base/Copyable.h"

#include <string>
#include <string.h>

namespace ideal {

class StringPiece {
public:
    StringPiece() :
        _ptr(nullptr),
        _length(0) {
    }

    StringPiece(const char* str) :
        _ptr(str),
        _length(static_cast<int>(strlen(_ptr))) {
    }

    StringPiece(const unsigned char* str) :
        _ptr(reinterpret_cast<const char*>(str)),
        _length(static_cast<int>(strlen(_ptr))) {
    }

    StringPiece(const std::string& str) :
        _ptr(str.data()),
        _length(static_cast<int>(str.size())) {
    }

    StringPiece(const char* offset, int len) :
        _ptr(offset),
        _length(len) {
    }


    const char* data() const { return _ptr; }
    int size() const { return _length; }
    bool empty() const { return _length == 0; }
    const char* begin() const { return _ptr; }
    const char* end() const { return _ptr + _length; }
    
    void clear() { _ptr = nullptr; _length = 0; }
    void set(const char* buffer, int len) { _ptr = buffer, _length = len; }
    void set(const char* str) {
        _ptr = str;
        _length = static_cast<int>(strlen(str));
    }
    void set(const void* buffer, int len) {
        _ptr = reinterpret_cast<const char*>(buffer);
        _length = len;
    }

    char operator[](int i) const { return _ptr[i]; }
    bool operator==(const StringPiece& x) const {
        return ((_length == x.size()) && (memcmp(_ptr, x.data(), _length) == 0));
    }
    bool operator!=(const StringPiece& x) const {
        return !(*this == x);
    }

#define STRINGPIECE_BINARY_PREDICATE(cmp, auxcmp)  \
    bool operator cmp (const StringPiece& x) const {  \
        int ret = memcmp(_ptr, x.data(), _length < x.size()? _length : x.size());  \
        return ((ret auxcmp 0) || ((ret == 0)&&(_length cmp x.size())));  \
    }
    STRINGPIECE_BINARY_PREDICATE(<, <);
    STRINGPIECE_BINARY_PREDICATE(<=, <);
    STRINGPIECE_BINARY_PREDICATE(>, >);
    STRINGPIECE_BINARY_PREDICATE(>=, >);
#undef STRINGPIECE_BINARY_PREDICAT
    
    void removePerfix(int n) { _ptr += n; _length -= n; }
    void removeSuffix(int n) { _length -= n; }

    int compare(const StringPiece& x) const {
   		int ret = memcmp(_ptr, x.data(), _length < x.size() ? _length : x.size());
    	if(ret == 0) {
      		if(_length < x.size())
				ret = -1;
      		else if (_length > x.size()) 
				ret = +1;
    	}
    	return ret;
	}
	
    std::string asString() const {
        return std::string(_ptr, _length);
    }

    void CopyToString(std::string* target) const {
        target->assign(_ptr, _length);
    }

    bool isStartWith(const StringPiece& x) const {
        return ((_length >= x.size()) && (memcmp(_ptr, x.data(), x.size())));
    }


private:
    const char* _ptr;
    int _length;
};


class StringArg : public Copyable {
public:
    StringArg(const char* str) :
        _str(str) {
    }

    StringArg(const std::string& str) :
        _str(str.c_str()) {
    }

    const char* c_str() const {
        return _str;
    }

private:
    const char* _str;
};

}


#endif // _IDEAL_BASE_STRINGPIECE_H


