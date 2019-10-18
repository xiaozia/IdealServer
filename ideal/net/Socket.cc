
/******************************************************
*   Copyright (C)2019 All rights reserved.
*
*   Author        : owb
*   Email         : 2478644416@qq.com
*   File Name     : Socket.cc
*   Last Modified : 2019-07-05 16:16
*   Describe      :
*
*******************************************************/

#include "ideal/net/Socket.h"
#include "ideal/base/Logger.h"
#include "ideal/net/InetAddress.h"
#include "ideal/net/SocketUtil.h"

// IPPROTO_TCP TCP_NODELAY
#include <netinet/in.h>
#include <netinet/tcp.h>

using namespace ideal;
using namespace ideal::net;

Socket::~Socket() {
    sockets::close(_sockfd);
}

void Socket::bindAddress(const InetAddress& localaddr) {
    sockets::bindOrDie(_sockfd, localaddr.getSockAddr());
}

void Socket::listen() {
    sockets::listenOrDie(_sockfd);
}

int Socket::accept(InetAddress* peeraddr) {
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof addr);
    int connfd = sockets::accept(_sockfd, &addr);
    if(connfd >= 0) {
        peeraddr->setSockAddr(addr);
    }
    return connfd;
}

void Socket::shutdownWrite() {
    sockets::shutdownWrite(_sockfd);
}
    
void Socket::setTcpNoDelay(bool on) {
    int optval = on? 1 : 0;
    ::setsockopt(_sockfd, IPPROTO_TCP, TCP_NODELAY, &optval, static_cast<socklen_t>(sizeof optval));
}

void Socket::setReuseAddr(bool on) {
    int optval = on? 1 : 0;
    ::setsockopt(_sockfd, SOL_SOCKET, SO_REUSEADDR, &optval, static_cast<socklen_t>(sizeof optval));
}
    
void Socket::setReusePort(bool on) {
#ifdef SO_REUSEPORT
    int optval = on? 1 : 0;
    int ret = ::setsockopt(_sockfd, SOL_SOCKET, SO_REUSEPORT, &optval, static_cast<socklen_t>(sizeof optval));
    if(ret < 0 && on) {
        LOG_SYSERR << "SO_REUSEPORT failed";
    }
#else
    if(on) {
        LOG_ERR << "SO_REUSEPORT is not supported";
    }
#endif
}
    
void Socket::setKeepAlive(bool on) {
    int optval = on? 1 : 0;
    ::setsockopt(_sockfd, SOL_SOCKET, SO_KEEPALIVE, &optval, static_cast<socklen_t>(sizeof optval));
}

bool Socket::getTcpInfo(struct tcp_info* ti) const {
    socklen_t len = sizeof(*ti);
    memset(ti, 0, len);
    return ::getsockopt(_sockfd, SOL_TCP, TCP_INFO, ti, &len) == 0;
}

// struct tcp_info {
//     __u8 tcpi_state; /* TCP状态 */
//     __u8 tcpi_ca_state; /* TCP拥塞状态 */
//     __u8 tcpi_retransmits; /* 超时重传的次数 */
//     __u8 tcpi_probes; /* 持续定时器或保活定时器发送且未确认的段数*/
//     __u8 tcpi_backoff; /* 退避指数 */
//     __u8 tcpi_options; /* 时间戳选项、SACK选项、窗口扩大选项、ECN选项是否启用*/
//     __u8 tcpi_snd_wscale : 4, tcpi_rcv_wscale : 4; /* 发送、接收的窗口扩大因子*/
// 
//     __u32 tcpi_rto; /* 超时时间，单位为微秒*/
//     __u32 tcpi_ato; /* 延时确认的估值，单位为微秒*/
//     __u32 tcpi_snd_mss; /* 本端的MSS */
//     __u32 tcpi_rcv_mss; /* 对端的MSS */
// 
//     __u32 tcpi_unacked; /* 未确认的数据段数，或者current listen backlog */
//     __u32 tcpi_sacked; /* SACKed的数据段数，或者listen backlog set in listen() */
//     __u32 tcpi_lost; /* 丢失且未恢复的数据段数 */
//     __u32 tcpi_retrans; /* 重传且未确认的数据段数 */
//     __u32 tcpi_fackets; /* FACKed的数据段数 */
// 
//     /* Times. 单位为毫秒 */
//     __u32 tcpi_last_data_sent; /* 最近一次发送数据包在多久之前 */
//     __u32 tcpi_last_ack_sent;  /* 不能用。Not remembered, sorry. */
//     __u32 tcpi_last_data_recv; /* 最近一次接收数据包在多久之前 */
//     __u32 tcpi_last_ack_recv; /* 最近一次接收ACK包在多久之前 */
// 
//     /* Metrics. */
//     __u32 tcpi_pmtu; /* 最后一次更新的路径MTU */
//     __u32 tcpi_rcv_ssthresh; /* current window clamp，rcv_wnd的阈值 */
//     __u32 tcpi_rtt; /* 平滑的RTT，单位为微秒 */
//     __u32 tcpi_rttvar; /* 四分之一mdev，单位为微秒v */
//     __u32 tcpi_snd_ssthresh; /* 慢启动阈值 */
//     __u32 tcpi_snd_cwnd; /* 拥塞窗口 */
//     __u32 tcpi_advmss; /* 本端能接受的MSS上限，在建立连接时用来通告对端 */
//     __u32 tcpi_reordering; /* 没有丢包时，可以重新排序的数据段数 */
// 
//     __u32 tcpi_rcv_rtt; /* 作为接收端，测出的RTT值，单位为微秒*/
//     __u32 tcpi_rcv_space;  /* 当前接收缓存的大小 */
// 
//     __u32 tcpi_total_retrans; /* 本连接的总重传个数 */
// };
bool Socket::getTcpInfoString(char* buf, int len) const {
    struct tcp_info ti;
    bool ok = getTcpInfo(&ti);
    if(ok) {
        snprintf(buf, len, "retransmits=%u " 
                           "rto=%u ato=%u snd_mss=%u rcv_mss=%u "
                           "lost=%u retrans=%u rtt=%u rttvar=%u "
                           "sshthresh=%u cwnd=%u total_retrans=%u", 
                 ti.tcpi_retransmits,   // 超时重传次数
                 ti.tcpi_rto,           // 超时时间：us
                 ti.tcpi_ato,           // 延时确认估值：us 
                 ti.tcpi_snd_mss,       // 本端MSS
                 ti.tcpi_rcv_mss,       // 对端MSS
                 ti.tcpi_lost,          // 丢失且未恢复的数据段数
                 ti.tcpi_retrans,       // 重传且未确认的数据段数
                 ti.tcpi_rtt,           // 平滑的RTT：us
                 ti.tcpi_rttvar,        // 四分之一mdev：us
                 ti.tcpi_snd_ssthresh,  // 慢启动阈值
                 ti.tcpi_snd_cwnd,      // 拥塞窗口
                 ti.tcpi_total_retrans);// 本连接的总重传个数
    }
    return ok;
}

