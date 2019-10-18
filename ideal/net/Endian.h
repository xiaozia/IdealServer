
/******************************************************
*   Copyright (C)2019 All rights reserved.
*
*   Author        : owb
*   Email         : 2478644416@qq.com
*   File Name     : Endian.h
*   Last Modified : 2019-06-03 20:30
*   Describe      :
*
*******************************************************/

#ifndef  _IDEAL_NET_ENDIAN_H
#define  _IDEAL_NET_ENDIAN_H

#include <stdint.h>
#include <endian.h>

namespace ideal {

namespace net {

namespace sockets {

inline uint64_t hostToNetwork64(uint64_t host64) { return htobe64(host64); }
inline uint32_t hostToNetwork32(uint32_t host32) { return htobe32(host32); }
inline uint16_t hostToNetwork16(uint16_t host16) { return htobe16(host16); }

inline uint64_t networkToHost64(uint64_t net64) { return be64toh(net64); }
inline uint32_t networkToHost32(uint32_t net32) { return be32toh(net32); }
inline uint16_t networkToHost16(uint16_t net16) { return be16toh(net16); }

}

}

}

#endif // _IDEAL_NET_ENDIAN_H


