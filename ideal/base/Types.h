
/******************************************************
*   Copyright (C)2019 All rights reserved.
*
*   Author        : owb
*   Email         : 2478644416@qq.com
*   File Name     : Types.h
*   Last Modified : 2019-11-05 15:53
*   Describe      :
*
*******************************************************/

#ifndef  _IDEAL_BASE_TYPES_H
#define  _IDEAL_BASE_TYPES_H

// refer to boost/implicit_cast.hpp
namespace ideal {

template<class T>
struct icast_identity {
    typedef T type;
};

template <typename T>
inline T implicit_cast(typename icast_identity<T>::type x) {
    return x;
}

}

#endif // _IDEAL_BASE_TYPES_H

