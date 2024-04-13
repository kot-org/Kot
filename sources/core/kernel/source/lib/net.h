#ifndef LIB_NET_H
#define LIB_NET_H 1

#include <endian.h>

#if BYTE_ORDER == LITTLE_ENDIAN
#	define htonl(x) __bswap_32(x)
#	define htons(x) __bswap_16(x)
#	define ntohl(x) __bswap_32(x)
#	define ntohs(x) __bswap_16(x)

#else
#	define htonl(x) ((uint32_t)x)
#	define htons(x) ((uint16_t)x)
#	define ntohl(x) ((uint32_t)x)
#	define ntohs(x) ((uint16_t)x)

#endif

#endif // LIB_NET_H