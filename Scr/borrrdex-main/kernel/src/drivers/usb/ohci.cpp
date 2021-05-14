#include "ohci.h"
#include "drivers/x86_64/pit.h"
#include "KernelUtil.h"
#include "Panic.h"
#include <vector>
using namespace std;

vector<volatile ohci_registers_t *> controllers;

// Avoid compiler optimization in order to always read an entire
// DWORD from the register.  Bad things happen otherwise.
__attribute__((noinline)) static uint32_t masked_read(uint32_t input, uint32_t mask) {
    return input & mask;
}

static bool really_ohci(void* base) {
    volatile ohci_registers_t* regs = (ohci_registers_t *)base;
    if(masked_read(regs->revision, ohci::REV_VERSION_MASK) != 0x10) {
        return false;
    }

    uint32_t cmd_status = regs->cmd_status;
    cmd_status |= ohci::CMD_STAT_HC_RESET_FLAG;
    regs->cmd_status = cmd_status;
    pit_sleepms(1);

    if(masked_read(regs->cmd_status, ohci::CMD_STAT_HC_RESET_FLAG)) {
        return false;
    }

    if(masked_read(regs->frame_interval, 0x3fff) != 0x2edf) {
        return false;
    }

    return true;
}

static bool setup_controller(void* base) {
    volatile ohci_registers_t* regs = (ohci_registers_t *)base;
    regs->hcca = 0xffffffff;
    uint32_t actual = regs->hcca;
    uint32_t neededAlignment = ~actual + 1;

    void* address = PageFrameAllocator::SharedAllocator()->RequestPage();
    KERNEL_ASSERT(((uint64_t)address % neededAlignment) == 0);

    // Reset the controller
    regs->control = 0;
    pit_sleepms(50);
    regs->control = ohci::OP_STATE_SUSPEND << ohci::CONTROL_HC_FUNC_STATE_OFFSET;

    // Set some defaults
    regs->frame_interval = 0xa7782edf; // bit 31 (new value) / full speed max packet size (2778) / default frame interval 2edf
    regs->periodic_start = 0x2a2f; // Priority given to interrupts after 1200 frames (10% interval)

    // Enable power management per port instead of global
    uint32_t rh_desc_a = regs->rh_desc_a;
    uint8_t port_count = rh_desc_a & ohci::RH_DESCA_PORT_COUNT_MASK;
    rh_desc_a |= ohci::RH_DESCA_POW_SW_MODE_FLAG;
    rh_desc_a &= ~ohci::RH_DESCA_NO_POW_SW_FLAG;
    regs->rh_desc_a = rh_desc_a;

    // Power on each port
    uint32_t rh_desc_b = regs->rh_desc_b;
    for(int i = 0; i < port_count; i++) {
        rh_desc_b |= 1 << (i + 17);
    }
    regs->rh_desc_b = rh_desc_b;

    // Set the communication buffer
    regs->hcca = (uint32_t)(uint64_t)(address);

    // TODO: Endpoint descriptor set

    regs->control = ohci::CONTROL_BL_ENABLE_FLAG | ohci::CONTROL_CL_ENABLE_FLAG 
        | ohci::CONTROL_HAS_REM_WAKEUP_FLAG | ohci::OP_STATE_RUNNING << ohci::CONTROL_HC_FUNC_STATE_OFFSET
        | ohci::CONTROL_REM_WAKEUP_ON_FLAG;
    regs->rh_status = ohci::RH_STAT_REM_WAKEUP_ON_FLAG;
    regs->int_status = ohci::INT_START_OF_FRAME_FLAG;

    
    
    controllers.push_back(regs);
}

int ohci_init(pci_device_t* dev) {
    void* page = (void *)(uint64_t)(dev->bar[0] & ~0xfff);
    KernelPageTableManager()->MapMemory(page, page, false);

    void* register_addr = (void *)(uint64_t)(dev->bar[0] & ~0xf);
    if(!really_ohci(register_addr)) {
        return -1;
    }

    setup_controller(register_addr);

    return 0;
}

