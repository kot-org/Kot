#ifndef LIB_GUID_H
#define LIB_GUID_H 1

#include <stdint.h>

typedef struct{
    uint32_t data0;

    uint16_t data1;
    uint16_t data2;
    uint16_t data3;

    uint8_t data4[6];
}__attribute__((packed)) guid_t;

#endif // LIB_GUID_H