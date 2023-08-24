#ifndef _MODULE_STORAGE_PARTITION_CRC32_H
#define _MODULE_STORAGE_PARTITION_CRC32_H

#include <stdint.h>
#include <stddef.h>

uint32_t partition_crc32(uint32_t crc, const char *buf, size_t len);

#endif // _MODULE_STORAGE_PARTITION_CRC32_H