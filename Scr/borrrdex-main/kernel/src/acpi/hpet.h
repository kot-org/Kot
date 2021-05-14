#pragma once

#ifndef __cplusplus
#error C++ Only
#endif

#include "common.h"

// High Precision Event Timer
// Intel IA-PC HPET spec p.30
typedef struct {
    acpi_desc_header_t h;
    uint32_t evt_tmr_blk_id;
    acpi_generic_addr_t base_address;
    uint8_t hpet_no;
    uint16_t min_clock_tick;
    uint8_t oem_attributes:4;
    uint8_t page_protection:4;
} __attribute__((packed)) hpet_t;

class HPET {
public:
    static constexpr const char* signature = "HPET";

    HPET(void* data)
        :_data((hpet_t *)data)
    {}

    bool is_valid() const;
    const hpet_t* data() const { return _data; }
private:
    hpet_t* _data;
};