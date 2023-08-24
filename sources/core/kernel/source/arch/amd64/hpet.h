#ifndef _AMD64_HPET_H
#define _AMD64_HPET_H 1

#include <stdint.h>
#include <sys/types.h>
#include <impl/time.h>
#include <arch/include.h>
#include ARCH_INCLUDE(acpi.h)

void hpet_init(struct acpi_hpet_header* hpet);

ms_t hpet_get_current_time(void);

void hpet_sleep(ms_t ms);

#endif // _AMD64_HPET_H