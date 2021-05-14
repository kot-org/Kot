#include "common.h"

bool acpi_checksum_ok(void* ptr, size_t len)
{
    unsigned char sum = 0;
    for(int i = 0; i < len; i++) {
        sum += ((char *)ptr)[i];
    }

    return sum == 0;
}