#ifndef _NETINET_TCP_H
#define _NETINET_TCP_H

#ifdef __cplusplus
extern "C" {
#endif

// Define some macros using same ABI as Linux
#define TCP_NODELAY 1
#define TCP_MAXSEG 2
#define TCP_KEEPIDLE 4
#define TCP_KEEPINTVL 5
#define TCP_KEEPCNT 6
#define TCP_DEFER_ACCEPT 9
#define TCP_CONGESTION 13
#define TCP_FASTOPEN 23

#define SOL_TCP 6

typedef        u_int32_t tcp_seq;

struct tcphdr{
    u_int16_t th_sport;                /* source port */
    u_int16_t th_dport;                /* destination port */
    tcp_seq th_seq;                /* sequence number */
    tcp_seq th_ack;                /* acknowledgement number */
#  if __BYTE_ORDER == __LITTLE_ENDIAN
    u_int8_t th_x2:4;                /* (unused) */
    u_int8_t th_off:4;                /* data offset */
#  endif
#  if __BYTE_ORDER == __BIG_ENDIAN
    u_int8_t th_off:4;                /* data offset */
    u_int8_t th_x2:4;                /* (unused) */
#  endif
    u_int8_t th_flags;
#  define TH_FIN        0x01
#  define TH_SYN        0x02
#  define TH_RST        0x04
#  define TH_PUSH        0x08
#  define TH_ACK        0x10
#  define TH_URG        0x20
    u_int16_t th_win;                /* window */
    u_int16_t th_sum;                /* checksum */
    u_int16_t th_urp;                /* urgent pointer */
};

#ifdef __cplusplus
}
#endif

#endif // _NETINET_TCP_H
