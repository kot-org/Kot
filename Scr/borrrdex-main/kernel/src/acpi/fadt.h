#pragma once

#ifndef __cplusplus
#error C++ Only
#endif

#include "common.h"

// The Fixed ACPI Description Table
// ACPI 6.4 p.147
typedef struct {
    acpi_desc_header_t h;
    uint32_t firmware_ctrl;
    uint32_t dsdt;
    uint8_t reserved1;
    uint8_t preferred_pm;
    uint16_t sci_int;
    uint32_t smi_cmd;
    uint8_t acpi_enable;
    uint8_t acpi_disable;
    uint8_t s4bios_req;
    uint8_t pstate_cnt;
    uint32_t pm1a_evt_blk;
    uint32_t pm1b_evt_blk;
    uint32_t pm1a_cnt_blk;
    uint32_t pm1b_cnt_blk;
    uint32_t pm2_cnt_blk;
    uint32_t pm_tmr_blk;
    uint32_t gpe0_blk;
    uint32_t gpe1_blk;
    uint8_t pm1_evt_len;
    uint8_t pm1_cnt_len;
    uint8_t pm2_cnt_len;
    uint8_t pm_tmr_len;
    uint8_t gpe0_blk_len;
    uint8_t gpe1_blk_len;
    uint8_t gpe_base;
    uint8_t cst_cnt;
    uint16_t p_lvl2_lat;
    uint16_t p_lvl3_lat;
    uint16_t flush_size;
    uint16_t flush_stride;
    uint8_t duty_offset;
    uint8_t duty_width;
    uint8_t day_alrm;
    uint8_t mon_alrm;
    uint8_t century;
    uint16_t iapc_boot_arch;
    uint8_t reserved2;
    uint32_t flags;
    uint8_t reset_value;
    uint16_t arm_boot_arch;
    uint8_t fadt_minor_version;
    uint64_t x_firmware_ctrl;
    uint64_t x_dsdt;
    uint32_t x_pm1a_evt_blk[3];
    uint32_t x_pm1b_evt_blk[3];
    uint32_t x_pm1a_cnt_blk[3];
    uint32_t x_pm1b_cnt_blk[3];
    uint32_t x_pm_tmr_blk[3];
    uint32_t x_pm_gpe0_blk[3];
    uint32_t x_pm_gpe1_blk[3];
    uint32_t sleep_control_register[3];
    uint32_t sleep_status_register[3];
    uint64_t hypervisor_vendor_id;
} __attribute__((packed)) fadt_t;

class FADT {
public:
    static constexpr const char* signature = "FACP";

    FADT(void* data)
        :_data((fadt_t *)data)
    {}

    bool is_valid() const;
    
    const fadt_t* data() const { return _data; }
private:
    fadt_t* _data;
};
