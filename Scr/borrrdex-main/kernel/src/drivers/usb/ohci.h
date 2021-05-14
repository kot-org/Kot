#pragma once

#include "pci/pci.h"
#include <cstdint>

namespace ohci {
    constexpr uint8_t  REV_VERSION_MASK             = 0xff;
    constexpr uint16_t REV_LEGACY_FLAG              = 1 << 8;

    constexpr uint8_t  CONTROL_BSR_MASK             = 0x3;
    constexpr uint8_t  CONTROL_PL_ENABLE_FLAG       = 1 << 2;
    constexpr uint8_t  CONTROL_ISO_ENABLE_FLAG      = 1 << 3;
    constexpr uint8_t  CONTROL_CL_ENABLE_FLAG       = 1 << 4;
    constexpr uint8_t  CONTROL_BL_ENABLE_FLAG       = 1 << 5;
    constexpr uint8_t  CONTROL_HC_FUNC_STATE_OFFSET = 6;
    constexpr uint8_t  CONTROL_HC_FUNC_STATE_MASK   = 0x3;
    constexpr uint16_t CONTROL_INT_ROUTING_FLAG     = 1 << 8;
    constexpr uint16_t CONTROL_HAS_REM_WAKEUP_FLAG  = 1 << 9;
    constexpr uint16_t CONTROL_REM_WAKEUP_ON_FLAG   = 1 << 10;
    
    constexpr uint8_t  CMD_STAT_HC_RESET_FLAG       = 1 << 0;
    constexpr uint8_t  CMD_STAT_CL_FULL_FLAG        = 1 << 1;
    constexpr uint8_t  CMD_STAT_BL_FULL_FLAG        = 1 << 2;
    constexpr uint8_t  CMD_STAT_OWN_CHG_REQ_FLAG    = 1 << 3;
    constexpr uint8_t  CMD_STAT_SCHD_OVERRUN_OFFSET = 16;
    constexpr uint8_t  CMD_STAT_SCHD_OVERRUN_MASK   = 0x3;

    constexpr uint8_t  OP_STATE_RESET               = 0x0;
    constexpr uint8_t  OP_STATE_RESUME              = 0x1;
    constexpr uint8_t  OP_STATE_RUNNING             = 0x2;
    constexpr uint8_t  OP_STATE_SUSPEND             = 0x3;

    constexpr uint8_t  INT_SCHED_OVERRUN_FLAG       = 1 << 0;
    constexpr uint8_t  INT_WRITEBACK_DONE_FLAG      = 1 << 1;
    constexpr uint8_t  INT_START_OF_FRAME_FLAG      = 1 << 2;
    constexpr uint8_t  INT_RESUME_DETECT_FLAG       = 1 << 3;
    constexpr uint8_t  INT_UNRECOVERABLE_ERR_FLAG   = 1 << 4;
    constexpr uint8_t  INT_FRAME_NO_OVERFLOW_FLAG   = 1 << 5;
    constexpr uint8_t  INT_ROOT_HUB_STAT_CHG_FLAG   = 1 << 6;
    constexpr uint32_t INT_OWNERSHIP_CHANGE_FLAG    = 1 << 30;

    constexpr uint8_t  RH_DESCA_PORT_COUNT_MASK     = 0xff;
    constexpr uint16_t RH_DESCA_POW_SW_MODE_FLAG    = 1 << 8;
    constexpr uint16_t RH_DESCA_NO_POW_SW_FLAG      = 1 << 9;
    constexpr uint16_t RH_DESCA_DEV_TYPE_FLAG       = 1 << 10;
    constexpr uint16_t RH_DESCA_OC_PROT_MODE_FLAG   = 1 << 11;
    constexpr uint16_t RH_DESCA_NO_OC_PROT_FLAG     = 1 << 12;
    constexpr uint8_t  RH_DESCA_POW_TO_GOOD_OFFSET  = 24;
    constexpr uint8_t  RH_DESCA_POW_TO_GOOD_MASK    = 0xff;

    constexpr uint8_t  RH_STAT_GLOB_POW_CLEAR_FLAG  = 1 << 1;
    constexpr uint8_t  RH_STAT_OVER_CURRENT_FLAG    = 1 << 2;
    constexpr uint16_t RH_STAT_REM_WAKEUP_ON_FLAG   = 1 << 15;
    constexpr uint32_t RH_STAT_GLOB_POW_SET_FLAG    = 1 << 16;
    constexpr uint32_t RH_STAT_OVC_CHANGE_FLAG      = 1 << 17;
    constexpr uint32_t RH_STAT_REM_WAKEUP_CLR_FLAG  = 1 << 31;

    constexpr uint16_t HCCA_SIZE                    = 256;
}

typedef struct {
    uint32_t revision;
    uint32_t control;
    uint32_t cmd_status;
    uint32_t int_status;
    uint32_t int_enable;
    uint32_t int_disable;
    uint32_t hcca;
    uint32_t period_current_ed;
    uint32_t control_head_ed;
    uint32_t control_current_ed;
    uint32_t bulk_head_ed;
    uint32_t bulk_current_ed;
    uint32_t done_head;
    uint32_t frame_interval;
    uint32_t frame_remaining;
    uint32_t frame_number;
    uint32_t periodic_start;
    uint32_t ls_threshold;
    uint32_t rh_desc_a;
    uint32_t rh_desc_b;
    uint32_t rh_status;
    uint32_t rh_port_status[0];
} __attribute__((packed)) ohci_registers_t;

int ohci_init(pci_device_t* dev);