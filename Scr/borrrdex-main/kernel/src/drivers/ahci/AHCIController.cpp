#include "AHCIController.h"
#include "string.h"
#include "paging/PageTableManager.h"
#include "KernelUtil.h"
#include "Panic.h"
#include "drivers/x86_64/pit.h"
#include "drivers/x86_64/_disk.h" // For IDE commands
#include "drivers/atareg.h"
#include "memory/heap.h"

constexpr uint32_t AHCI_INTERNAL_VER_0_95   = 0x000905;
constexpr uint32_t AHCI_INTERNAL_VER_1_0    = 0x010000;
constexpr uint32_t AHCI_INTERNAL_VER_1_1    = 0x010100;
constexpr uint32_t AHCI_INTERNAL_VER_1_2    = 0x010200;
constexpr uint32_t AHCI_INTERNAL_VER_1_3    = 0x010300;
constexpr uint32_t AHCI_INTERNAL_VER_1_3_1  = 0x010301;

constexpr uint32_t SATA_SIG_ATA     = 0x00000101;
constexpr uint32_t SATA_SIG_ATAPI   = 0xEB140101;
constexpr uint32_t SATA_SIG_SEMB    = 0xC33C0101;
constexpr uint32_t SATA_SIG_PM      = 0x96690101;

static AHCIController* s_detected_controllers[4];
static int s_detected_count = 0;

static inline void swap16_multi(uint16_t* buf, size_t count) {
    while(count--) {
        *buf = __builtin_bswap16(*buf);
        buf++;
    }
}

static inline void trim_end(uint8_t* buf_end) {
    while(*buf_end == ' ') {
        *buf_end-- = 0;
    }
}

typedef struct {
    uint8_t command_fis_length:5;
    uint8_t atapi:1;
    uint8_t write:1;
    uint8_t prefetchable:1;

    uint8_t reset:1;
    uint8_t bist:1;
    uint8_t clear:1;
    uint8_t reserved0:1;
    uint8_t port_multiplier_port:4;

    uint16_t prdt_entries;

    volatile uint32_t pdtr_bytes_written;

    uint64_t command_table_base;

    uint32_t reserved1[4];
} achi_hba_cmd_header_t;

// Serial ATA 3.0 (p. 384)
typedef struct {
    // DW 0
    uint8_t fis_type;
    uint8_t port_multiplier:4;
    uint8_t reserved0:3;
    uint8_t is_command:1;
    uint8_t cmd_register;
    uint8_t feature_low;

    // DW 1
    uint8_t lba0;
    uint8_t lba1;
    uint8_t lba2;
    uint8_t device;

    // DW 2
    uint8_t lba3;
    uint8_t lba4;
    uint8_t lba5;
    uint8_t feature_high;

    // DW 3
    uint16_t count;
    uint8_t icc;
    uint8_t control;

    // DW 4
    uint32_t reserved1;
} fis_reg_h2d_t;
static_assert(sizeof(fis_reg_h2d_t) == 5 * sizeof(uint32_t));

typedef struct {
    // DW 0
    uint8_t fis_type;
    uint8_t port_multiplier:4;
    uint8_t reserved0:1;
    uint8_t direction:1;
    uint8_t interrupt:1;
    uint8_t auto_activate:1;
    uint16_t reserved1;

    // DW 1 - 2
    uint64_t dma_buffer_id;

    // DW 3
    uint32_t reserved2;

    // DW 4
    uint32_t dma_buffer_offset;

    // DW 5
    uint32_t transfer_count;

    // DW 6
    uint32_t reserved;
} __attribute__((packed)) fis_dma_setup_t;
static_assert(sizeof(fis_dma_setup_t) == 7 * sizeof(uint32_t));

typedef struct {
    // DW 0
    uint8_t fis_type;
    uint8_t port_multiplier:4;
    uint8_t reserved0:1;
    uint8_t direction:1;
    uint8_t interrupt:1;
    uint8_t reserved1:1;
    uint8_t status;
    uint8_t error;

    // DW 1
    uint8_t lba0;
    uint8_t lba1;
    uint8_t lba2;
    uint8_t device;

    // DW 2
    uint8_t lba3;
    uint8_t lba4;
    uint8_t lba5;
    uint8_t reserved2;

    // DW 3
    uint16_t count;
    uint8_t reserved3;
    uint8_t e_status;

    // DW 4
    uint16_t transfer_count;
    uint16_t reserved4;
} fis_pio_setup_t;
static_assert(sizeof(fis_pio_setup_t) == 5 * sizeof(uint32_t));

typedef struct {
    // DW 0
    uint8_t fis_type;
    uint8_t port_multiplier:4;
    uint8_t reserved0:2;
    uint8_t interrupt:1;
    uint8_t reserved1:1;
    uint8_t status;
    uint8_t error;

    // DW 1
    uint8_t lba0;
    uint8_t lba1;
    uint8_t lba2;
    uint8_t device;

    // DW 2
    uint8_t lba3;
    uint8_t lba4;
    uint8_t lba5;
    uint8_t reserved2;

    // DW 3
    uint16_t count;
    uint16_t reserved3;

    // DW 4
    uint32_t reserved4;
} fis_reg_d2h_t;
static_assert(sizeof(fis_reg_d2h_t) == 5 * sizeof(uint32_t));

typedef struct {
    // DW 0
    uint8_t fis_type;
    uint8_t port_multiplier:4;
    uint8_t reserved0:2;
    uint8_t interrupt:1;
    uint8_t notification:1;
    uint8_t status_lo:3;
    uint8_t reserved1:1;
    uint8_t status_hi:3;
    uint8_t reserved2:1;
    uint8_t error;

    // DW 1
    uint32_t protocol;
} fis_device_bits_t;
static_assert(sizeof(fis_device_bits_t) == 2 * sizeof(uint32_t));

typedef struct {
    uint64_t db_addr;
    uint32_t reserved0;

    uint32_t byte_count:22;
    uint32_t reserved1:9;
    uint32_t interrupt_on_complete:1;
} hba_prdt_entry_t;

typedef struct {
    uint8_t command_fis[64];
    uint8_t atapi_command[16];
    uint8_t reserved[48];
    hba_prdt_entry_t prdt_entries[0];
} hba_cmd_tbl_t;

typedef struct {
    fis_dma_setup_t dsfis;
    uint32_t pad0;

    fis_pio_setup_t psfis;
    uint8_t pad1[12];

    fis_reg_d2h_t rfis;
    uint32_t pad2;

    fis_device_bits_t sdbfis;

    uint8_t ufis[64];

    uint8_t reserved[0x100 - 0xA0];
} __attribute__((packed)) hba_received_fis_t;
static_assert(sizeof(hba_received_fis_t) == 0x100);

enum FISType : uint8_t {
    RegisterHostToDevice    = 0x27,
    RegisterDeviceToHost    = 0x34,
    DMAActivate             = 0x39,
    DMASetup                = 0x41,
    Data                    = 0x46,
    BuiltinSelfTest         = 0x58,
    PIOSetup                = 0x5F,
    DeviceBits              = 0xA1
};

static int ahci_init(pci_header_t* mem) {
    s_detected_controllers[s_detected_count++] = new AHCIController((pci_device_t *)mem);
    return 0;
}

static inline void set_enabled(volatile uint32_t* val, uint32_t mask, bool enabled) {
    if(enabled) {
        *val |= mask;
    } else {
        *val &= ~mask;
    }
}

static int find_cmdslot(ahci_hba_port_t* port, int slot_count) {
    for(int i = 0; i < slot_count; i++) {
        if((port->command_issue & (1 << i)) == 0) {
            return i;
        }
    }

    return -1;
}

static achi_hba_cmd_header_t* prepare_cmd_header(ahci_hba_port_t* port, int slot, uint64_t sector_count) {
    achi_hba_cmd_header_t* cmdHeader = ((achi_hba_cmd_header_t *)port->cmd_list_base) + slot;
    
    memset(cmdHeader, 0, sizeof(achi_hba_cmd_header_t));
    cmdHeader->command_fis_length = sizeof(fis_reg_h2d_t) / sizeof(uint32_t);
    cmdHeader->prdt_entries = (uint16_t)((sector_count-1)>>4) + 1;
    cmdHeader->command_table_base = port->cmd_list_base + sizeof(achi_hba_cmd_header_t) * 32;

    return cmdHeader;
}

static void prepare_cmd_fis(hba_cmd_tbl_t* cmd_tbl, uint8_t command, uint64_t start, uint64_t count) {
    fis_reg_h2d_t* cmdfis = (fis_reg_h2d_t *)(cmd_tbl->command_fis);
    cmdfis->fis_type = RegisterHostToDevice;
    cmdfis->cmd_register = command;
    cmdfis->is_command = 1;
    cmdfis->device = 0x40;
    cmdfis->count = (uint16_t)(count & 0xffff);
    cmdfis->lba0 = (uint8_t)start;
    cmdfis->lba1 = (uint8_t)(start >> 8);
    cmdfis->lba2 = (uint8_t)(start >> 16);
    cmdfis->lba3 = (uint8_t)(start >> 24);
    cmdfis->lba4 = (uint8_t)(start >> 32);
    cmdfis->lba5 = (uint8_t)(start >> 40);
}

static void prepare_prdt_entries(hba_cmd_tbl_t* cmd_tbl, uint8_t* buf, uint16_t entry_count, uint64_t sector_count) {
    int last_entry = entry_count - 1;
    for(int i = 0; i < last_entry; i++) {
        cmd_tbl->prdt_entries[i].db_addr = (uint64_t)buf;
        cmd_tbl->prdt_entries[i].byte_count = 0x1fff;
        buf += 0x2000;
        sector_count -= 16;
    }

    cmd_tbl->prdt_entries[last_entry].db_addr = (uint64_t)buf;
    cmd_tbl->prdt_entries[last_entry].byte_count = (sector_count<<9)-1;
}

static int issue_ahci_command(ahci_hba_port_t* port, uint8_t slot) {
    int spin = 0;
    while((port->task_file_data & (ahci::PXTFD_STS_BUSY_FLAG | ahci::PXTFD_STS_DRQ_FLAG)) && spin < 1000000) {
        spin++;
    }
    
    if(spin == 1000000) {
        return -1;
    }

    port->command_issue = (1 << slot);
    while(port->command_issue & (1 << slot)) {
        pit_sleepms(100);
    }

    if(spin == 1000000 || port->interrupt_status & ahci::PXIS_TFES_FLAG) {
        hba_received_fis_t* received = (hba_received_fis_t*)port->fis_base;
        return -1;
    }

    return 0;
}

static bool identify_device(ahci_hba_port_t* port, int slot_count) {
    port->interrupt_status = -1;
    int spin = 0;
    int slot = find_cmdslot(port, slot_count);
    if(slot == -1) {
        return false;
    }

    uint8_t sector[512];
    memset(sector, 0, 512);

    achi_hba_cmd_header_t* cmdHeader = ((achi_hba_cmd_header_t *)port->cmd_list_base) + slot;
    
    memset(cmdHeader, 0, sizeof(achi_hba_cmd_header_t));
    cmdHeader->command_fis_length = sizeof(fis_reg_h2d_t) / sizeof(uint32_t);
    cmdHeader->prdt_entries = 1;
    cmdHeader->command_table_base = port->cmd_list_base + sizeof(achi_hba_cmd_header_t) * 32;

    hba_cmd_tbl_t* cmd_tbl = (hba_cmd_tbl_t *)cmdHeader->command_table_base;
    memset(&cmd_tbl->command_fis, 0, 64);
    fis_reg_h2d_t* cmdfis = (fis_reg_h2d_t *)(cmd_tbl->command_fis);
    cmdfis->fis_type = RegisterHostToDevice;
    cmdfis->cmd_register = IDE_COMMAND_IDENTIFY;
    cmdfis->is_command = 1;

    memset(&cmd_tbl->prdt_entries[0], 0, sizeof(hba_prdt_entry_t));
    cmd_tbl->prdt_entries[0].byte_count = 0x1ff;
    cmd_tbl->prdt_entries[0].db_addr = (uint64_t)&sector[0];

    port->command_issue = (1 << slot);
    while(port->command_issue & (1 << slot)) {
        pit_sleepms(10);
    }

    if(port->interrupt_status & ahci::PXIS_TFES_FLAG) {
        return false;
    }

    return true;
}

int ahci_read_block(gbd_t* gbd, gbd_request_t* request) {
    device_t* disk = (device_t *)gbd->device;
    uint8_t* buf = (uint8_t *)request->buf;
    uint64_t start = request->start;
    uint64_t count = request->count;
    int slot_count = ((ahci_device_info_t *)gbd->context)->slot_count;
    ahci_hba_port_t* port = (ahci_hba_port_t *)disk->real_device;

    port->interrupt_status = -1;
    int slot = find_cmdslot(port, slot_count);
    if(slot == -1) {
        return -1;
    }

    achi_hba_cmd_header_t* cmdHeader = prepare_cmd_header(port, slot, count);

    hba_cmd_tbl_t* cmd_tbl = (hba_cmd_tbl_t *)cmdHeader->command_table_base;
    memset(cmd_tbl, 0, sizeof(hba_cmd_tbl_t) + sizeof(hba_prdt_entry_t) * cmdHeader->prdt_entries);
    prepare_prdt_entries(cmd_tbl, buf, cmdHeader->prdt_entries, count);
    
    prepare_cmd_fis(cmd_tbl, ahci::IDE_COMMAND_DMA_READ_EX, start, count);
    if(issue_ahci_command(port, slot) == 0) {
        return count * 512;
    }

    return -1;
}

int ahci_write_block(gbd_t* gbd, gbd_request_t* request) {
    device_t* disk = (device_t *)gbd->device;
    uint8_t* buf = (uint8_t *)request->buf;
    uint64_t start = request->start;
    uint64_t count = request->count;
    int slot_count = ((ahci_device_info_t *)gbd->context)->slot_count;
    ahci_hba_port_t* port = (ahci_hba_port_t *)disk->real_device;

    port->interrupt_status = -1;
    int slot = find_cmdslot(port, slot_count);
    if(slot == -1) {
        return -1;
    }

    achi_hba_cmd_header_t* cmdHeader = prepare_cmd_header(port, slot, count);

    hba_cmd_tbl_t* cmd_tbl = (hba_cmd_tbl_t *)cmdHeader->command_table_base;
    memset(cmd_tbl, 0, sizeof(hba_cmd_tbl_t) + sizeof(hba_prdt_entry_t) * cmdHeader->prdt_entries);
    prepare_prdt_entries(cmd_tbl, buf, cmdHeader->prdt_entries, count);
    
    prepare_cmd_fis(cmd_tbl, ahci::IDE_COMMAND_DMA_WRITE_EX, start, count);
    if(issue_ahci_command(port, slot) == 0) {
        return count * 512;
    }

    return -1;
}

uint32_t ahci_block_size(gbd_t* gbd) {
    uint32_t existing = ((ahci_device_info_t *)gbd->context)->block_size;
    if(existing) {
        return existing;
    }

    ahci_hba_port_t* port = (ahci_hba_port_t *)gbd->device->real_device;
    uint8_t slot_count = ((ahci_device_info_t *)gbd->context)->slot_count;
    port->interrupt_status = -1;
    int slot = find_cmdslot(port, slot_count);
    if(slot == -1) {
        return -1;
    }

    uint8_t buf[512];

    achi_hba_cmd_header_t* cmdHeader = prepare_cmd_header(port, slot, 1);
    hba_cmd_tbl_t* cmd_tbl = (hba_cmd_tbl_t *)cmdHeader->command_table_base;
    memset(cmd_tbl, 0, sizeof(hba_cmd_tbl_t) + sizeof(hba_prdt_entry_t) * cmdHeader->prdt_entries);
    prepare_prdt_entries(cmd_tbl, buf, cmdHeader->prdt_entries, 1);

    prepare_cmd_fis(cmd_tbl, ahci::IDE_COMMAND_IDENTIFY, 0, 1);
    issue_ahci_command(port, slot);

    ataparams * ident_data = (ataparams *)buf;
    ((ahci_device_info_t *)gbd->context)->block_size = ident_data->atap_config & WDC_CFG_ATAPI_MASK ? 2048 : 512;
    return ((ahci_device_info_t *)gbd->context)->block_size;
}

uint64_t ahci_block_count(gbd_t* gbd) {
    uint64_t existing = ((ahci_device_info_t *)gbd->context)->block_count;
    if(existing) {
        return existing;
    }


    ahci_hba_port_t* port = (ahci_hba_port_t *)gbd->device->real_device;
    uint8_t slot_count = ((ahci_device_info_t *)gbd->context)->slot_count;
    port->interrupt_status = -1;
    int slot = find_cmdslot(port, slot_count);
    if(slot == -1) {
        return -1;
    }

    uint8_t buf[512];

    achi_hba_cmd_header_t* cmdHeader = prepare_cmd_header(port, slot, 1);
    hba_cmd_tbl_t* cmd_tbl = (hba_cmd_tbl_t *)cmdHeader->command_table_base;
    memset(cmd_tbl, 0, sizeof(hba_cmd_tbl_t) + sizeof(hba_prdt_entry_t) * cmdHeader->prdt_entries);
    prepare_prdt_entries(cmd_tbl, buf, cmdHeader->prdt_entries, 1);

    prepare_cmd_fis(cmd_tbl, ahci::IDE_COMMAND_IDENTIFY, 0, 1);
    issue_ahci_command(port, slot);

    ataparams * ident_data = (ataparams *)buf;
    if(ident_data->atap_cmd_set2 & ATAPI_CMD2_48AD) {
        ((ahci_device_info_t *)gbd->context)->block_count = ((uint64_t)ident_data->atap_max_lba[3] << 48) |
                ((uint64_t)ident_data->atap_max_lba[2] << 32) |
                ((uint64_t)ident_data->atap_max_lba[1] << 16) |
                (uint64_t)ident_data->atap_max_lba[0];
    } else {
        ((ahci_device_info_t *)gbd->context)->block_count = ((uint64_t)ident_data->atap_capacity[1] << 16) |
            ident_data->atap_capacity[0];
    }

    return ((ahci_device_info_t *)gbd->context)->block_count;
}

static void take_controller_ownership(ahci_hba_mem_t* controller) {
    if(controller->version < AHCI_INTERNAL_VER_1_2) {
        return; 
    }

    if(!(controller->capabilities2 & ahci::CAP2_BOH_FLAG)) {
        return;
    }

    controller->bios_handoff_ctrl |= ahci::BOHC_OOS_FLAG;
    pit_sleepms(25);

    if(!controller->bios_handoff_ctrl & (ahci::BOHC_BB_FLAG | ahci::BOHC_BOS_FLAG)) {
        // BIOS no longer owns the controller, and is not busy
        return;
    }

    //Wait for the BIOS to stop being busy
    pit_sleepms(2000);

    // If it's not done by now, it is faulty 
    KERNEL_ASSERT(!(controller->bios_handoff_ctrl & (ahci::BOHC_BB_FLAG | ahci::BOHC_BOS_FLAG)));
    KERNEL_ASSERT(controller->bios_handoff_ctrl & ahci::BOHC_OOS_FLAG);

    controller->bios_handoff_ctrl &= ~ahci::BOHC_OOC_FLAG;
}

bool AHCIController::register_achi_disk(ahci_hba_port_t* port, uint32_t index, uint8_t max_cmd_slots) {
    uint32_t ssts = port->sata_status;
    IPMStatus ipm = (IPMStatus)((port->sata_status >> ahci::SCR0_IPM_OFFSET) & ahci::SCR0_IPM_MASK);
    DeviceDetectionStatus det = (DeviceDetectionStatus)(port->sata_status & ahci::SCR0_DET_MASK);
    if(det != PhyComm || ipm != Active) {
        return false;
    }

    _commandPages[index] = PageFrameAllocator::SharedAllocator()->RequestPage();
    memset(_commandPages[index], 0, 0x1000);

    // Idle the drive before modifying it
    // First, halt the command processing
    port->command &= ~ahci::PXCMD_ST_FLAG;
    while(port->command & ahci::PXCMD_CR_FLAG) {
        pit_sleepms(10);
    }

    // Then halt the FIS Receiving
    port->command & ~ahci::PXCMD_FRE_FLAG;
    while(port->command & ahci::PXCMD_FR_FLAG) {
        pit_sleepms(10);
    }

    // Hooray, we can now write the memory address for the buffer
    // for the drive to use.  Be careful, this must be a *physical*
    // address since the controller bypasses the CPU and does not use
    // paging.  The page table manager is identity mapped, so this is ok.
    port->cmd_list_base = (uint64_t)_commandPages[index];

    // Get to work!
    port->command |= (ahci::PXCMD_FRE_FLAG | ahci::PXCMD_ST_FLAG);

    // Now whatever we write to the memory we just created, can be directly
    // processed by the drive via the controller.

    // Not going to use interrupts quite yet, that's for another day...
    port->interrupt_status = 0xffffffff; // Some are read only, controller doesn't seem to mind us trying though
    port->interrupt_enable = 0;

    // Currently hackishly ignoring ATAPI devices because I haven't written support for them yet
    if(!identify_device(port, max_cmd_slots)) {
        PageFrameAllocator::SharedAllocator()->FreePage(_commandPages[index]);
        return false;
    }

    // Register the device_t for the port
    _sata_dev[index] = {
        (void *)port,
        &_sata_gbd[index],
        nullptr,
        index,
        TYPECODE_DISK
    };

    // Register some info to use later
    _sata_devices[index] = {
        max_cmd_slots,
        0,
        0
    };

    // Register as a generic block device (see drivers/gbd.h)
    _sata_gbd[index] = {
        &_sata_dev[index],
        &_sata_devices[index],
        ahci_read_block,
        ahci_write_block,
        ahci_block_size,
        ahci_block_count
    };

    // Send the information over to the device list
    device_register(&_sata_dev[index]);

    return true;
}

int AHCIController::DetectedControllerCount() {
    return s_detected_count;
}

AHCIController* AHCIController::GetDetectedController(size_t index) {
    if(index >= s_detected_count) {
        return nullptr;
    }

    return s_detected_controllers[index];
}

AHCIController::AHCIController(pci_device_t* pciLocation) {
    _abar = (ahci_hba_mem_t *)(uint64_t)pciLocation->bar[5];
    PageTableManager* ptm = KernelPageTableManager();
    uint64_t page = (uint64_t)_abar / 4096 * 4096;
    ptm->MapMemory((void *)page, (void *)page, false);

    take_controller_ownership(_abar);

    uint32_t pi = _abar->port_implemented;
    int idx = 0;
    for(int i = 0; i < 32; i++, pi >>= 1) {
        if(!(pi & 1)) {
            continue;
        }

        if(register_achi_disk(&_abar->ports[i], idx, command_slot_count())) {
            idx++;
        }
    }

    _driveCount = idx;
    memset(_version, 0, 6);
    memset(_commandPages, 0, sizeof(void *) * SATA_MAX_DRIVES);
}

AHCIController::~AHCIController() {
    PageFrameAllocator* allocator = PageFrameAllocator::SharedAllocator();
    for(int i = 0; i < SATA_MAX_DRIVES; i++) {
        if(_commandPages[i]) {
            allocator->FreePage(_commandPages[i]);
            _commandPages[i] = nullptr;
        }
    }
}

void AHCIController::set_achi_only(bool achi_only) {
    if(!has_legacy_interface()) {
        // Unable to do this without a legacy interface
        return;
    }

    set_enabled(&_abar->global_host_control, ahci::GHC_AE_FLAG, achi_only);
}

void AHCIController::set_interrupts_enabled(bool enabled) {
    set_enabled(&_abar->global_host_control, ahci::GHC_IE_FLAG, enabled);
}

// Serial ATA AHCI 1.3.1 p.19
const char* AHCIController::ahci_version() {
    if(*_version != 0) {
        return _version;
    }

    char* cur = _version;
    uint16_t major = _abar->version >> 16;
    *cur++ = major + '0';
    *cur++ = '.';
    *cur++ = ((_abar->version >> 8) & 0xff) + '0';
    
    uint8_t revision = _abar->version >> 8;
    if(revision) {
        if(major != 0) {
            // Version 0 is special I guess
            *cur++ = '.';
        }

        *cur++ = revision + '0';
    }

    return _version;
}

// Gets are guaranteed by spec to return 0, but set should be a no-op if version is too low
void AHCIController::set_ccc_timeout(uint16_t timeout) {
    if(_abar->version < AHCI_INTERNAL_VER_1_1) {
        return;
    }

    _abar->ccc_ctrl |= timeout << ahci::CCC_TV_OFFSET;
}

// Gets are guaranteed by spec to return 0, but set should be a no-op if version is too low
void AHCIController::set_ccc_required_cmd_count(uint8_t count) {
    if(_abar->version < AHCI_INTERNAL_VER_1_1) {
        return;
    }

    _abar->ccc_ctrl |= count << ahci::CCC_CC_OFFSET;
}

// Gets are guaranteed by spec to return 0, but set should be a no-op if version is too low
void AHCIController::set_ccc_enabled(bool enabled) {
    if(_abar->version < AHCI_INTERNAL_VER_1_1) {
        return;
    }

    set_enabled(&_abar->ccc_ctrl, ahci::CCC_EN_FLAG, enabled);
}

// Gets are guaranteed by spec to return 0, but set should be a no-op if version is too low
void AHCIController::set_bios_busy(bool busy) {
    if(_abar->version < AHCI_INTERNAL_VER_1_2) {
        return;
    }

    set_enabled(&_abar->bios_handoff_ctrl, ahci::BOHC_BB_FLAG, busy);
}

void AHCIController::clear_ownership_changed() {
    if(_abar->version < AHCI_INTERNAL_VER_1_2) {
        return;
    }

    _abar->bios_handoff_ctrl |= ahci::BOHC_OOC_FLAG; 
}

void AHCIController::set_bios_smi_on_ooc(bool enabled) {
    if(_abar->version < AHCI_INTERNAL_VER_1_2) {
        return;
    }

    set_enabled(&_abar->bios_handoff_ctrl, ahci::BOHC_SOOE_FLAG, enabled);
}

void AHCIController::set_os_owned_semaphore(bool os_owned) {
    if(_abar->version < AHCI_INTERNAL_VER_1_2) {
        return;
    }
    
    set_enabled(&_abar->bios_handoff_ctrl, ahci::BOHC_OOS_FLAG, os_owned);
}

PCI_MODULE_INIT(AHCI_PCI_MODULE, ahci_init, 0x1, 0x6);