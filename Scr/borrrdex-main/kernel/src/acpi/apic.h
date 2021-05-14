#pragma once

#ifndef __cplusplus
#error C++ Only
#endif

#include "common.h"
#include <cstddef>
#include <cstdint>

namespace madt {
    // Interrupt Control Structure Types (ACPI 6.4 p.171)
    constexpr uint8_t TYPE_PROCESSOR_LOCAL_APIC     = 0x0;
    constexpr uint8_t TYPE_IO_APIC                  = 0x1;
    constexpr uint8_t TYPE_INTERRUPT_SRC_OVERRIDE   = 0x2;
    constexpr uint8_t TYPE_NMI_SOURCE               = 0x3;
    constexpr uint8_t TYPE_LOCAL_APIC_NMI           = 0x4;
    constexpr uint8_t TYPE_LOCAL_APIC_ADDR_OVERRIDE = 0x5;
    constexpr uint8_t TYPE_IO_SAPIC                 = 0x6;
    constexpr uint8_t TYPE_LOCAL_SAPIC              = 0x7;
    constexpr uint8_t TYPE_PLAT_INTERRUPT_SRC       = 0x8;
    constexpr uint8_t TYPE_PROCESSOR_LOCAL_X2APIC   = 0x9;
    constexpr uint8_t TYPE_LOCAL_X2APIC_NMI         = 0xA;
    constexpr uint8_t TYPE_GIC_CPU_INTERFACE        = 0xB;
    constexpr uint8_t TYPE_GIC_DISTRIBUTOR          = 0xC;
    constexpr uint8_t TYPE_GIC_MSI_FRAME            = 0xD;
    constexpr uint8_t TYPE_GIC_REDISTRIBUTOR        = 0xE;
    constexpr uint8_t TYPE_GIC_INT_TRANSLATION      = 0xF;
    constexpr uint8_t TYPE_MULTIPROCESSOR_WAKEUP    = 0x10;
}

namespace lapic {
    // LAPIC Registers Intel SDM Volume 3 Table 10-1
    constexpr uint16_t REG_OFFSET_ID                        = 0x0030;
    constexpr uint16_t REG_OFFSET_VERSION                   = 0x0040;
    constexpr uint16_t REG_OFFSET_TASK_PRIORITY             = 0x0080;
    constexpr uint16_t REG_OFFSET_ARBITRATION_PRIORITY      = 0x0090;
    constexpr uint16_t REG_OFFSET_PROCESSOR_PRIORITY        = 0x00A0;
    constexpr uint16_t REG_OFFSET_EOI                       = 0x00B0;
    constexpr uint16_t REG_OFFSET_REMOTE_READ               = 0x00C0;
    constexpr uint16_t REG_OFFSET_LOGICAL_DESTINATION       = 0x00D0;
    constexpr uint16_t REG_OFFSET_DESTINATION_FORMAT        = 0x00E0;
    constexpr uint16_t REG_OFFSET_SPURIOUS_INT_VECTOR       = 0x00F0;
    constexpr uint16_t REG_OFFSET_IN_SERVICE                = 0x0100;
    constexpr uint16_t REG_OFFSET_TRIGGER_MODE              = 0x0180;
    constexpr uint16_t REG_OFFSET_INTERRUPT_REQUEST         = 0x0200;
    constexpr uint16_t REG_OFFSET_ERROR_STATUS              = 0x0280;
    constexpr uint16_t REG_OFFSET_CMCI                      = 0x02F0;
    constexpr uint16_t REG_OFFSET_INTERRUPT_COMMAND         = 0x0300;
    constexpr uint16_t REG_OFFSET_LVT_TIMER                 = 0x0320;
    constexpr uint16_t REG_OFFSET_LVT_THERMAL_SENSOR        = 0x0330;
    constexpr uint16_t REG_OFFSET_LVT_PERF_MONITOR          = 0x0340;
    constexpr uint16_t REG_OFFSET_LVT_LINT0                 = 0x0350;
    constexpr uint16_t REG_OFFSET_LVT_LINT1                 = 0x0360;
    constexpr uint16_t REG_OFFSET_LVT_ERROR                 = 0x0370;
    constexpr uint16_t REG_OFFSET_INITIAL_COUNT             = 0x0380;
    constexpr uint16_t REG_OFFSET_CURRENT_COUNT             = 0x0390;
    constexpr uint16_t REG_OFFSET_DIVIDE_CONFIG             = 0x03E0;
}

namespace ioapic {
    // I/O APIC registers and register data (Intel 82093AA data sheet p. 9)
    constexpr uint8_t IOREGSEL_OFFSET                   = 0x00;
    constexpr uint8_t IOWIN_OFFSET                      = 0x10;

    constexpr uint8_t REG_OFFSET_IOAPICID               = 0x00;
    constexpr uint8_t REG_OFFSET_IOAPICVER              = 0x01;
    constexpr uint8_t REG_OFFSET_IOAPICARB              = 0x02;
    constexpr uint8_t REG_OFFSET_IOREBTBL_BASE          = 0x10;

    constexpr uint8_t IOREGSEL_REG_ADDR_MASK            = 0xff;

    constexpr uint8_t IOAPICID_ID_OFFSET                = 24;
    constexpr uint8_t IOAPICID_ID_MASK                  = 0xf;

    constexpr uint8_t IOAPICVER_MAX_REDIR_OFFSET        = 16;
    constexpr uint8_t IOAPICVER_MAX_REDIR_MASK          = 0xff;
    constexpr uint8_t IOAPICVER_VERSION_MASK            = 0xff;

    constexpr uint8_t IOAPICARB_ID_OFFSET               = 24;
    constexpr uint8_t IOAPICARB_ID_MASK                 = 0xf;

    constexpr uint8_t  IOREDTBL_DEST_FIELD_OFFSET       = 56;
    constexpr uint8_t  IOREDTBL_DEST_FIELD_LOG_MASK     = 0xff;
    constexpr uint8_t  IOREDTBL_DEST_FIELD_PHYS_MASK    = 0xf;
    constexpr uint32_t IOREDTBL_INT_MASK_FLAG           = 1 << 16;
    constexpr uint16_t IOREDTBL_TRIG_MODE_FLAG          = 1 << 15;
    constexpr uint16_t IOREDTBL_REMOTE_IRR_FLAG         = 1 << 14;
    constexpr uint16_t IOREDTBL_INTPOL_FLAG             = 1 << 13;
    constexpr uint16_t IOREDTBL_DELIVER_STAT_FLAG       = 1 << 12;
    constexpr uint16_t IOREDTBL_DESTMOD_FLAG            = 1 << 11;
    constexpr uint8_t  IOREDTBL_DELIVER_MODE_OFFSET     = 8;
    constexpr uint8_t  IOREDTBL_DELIVER_MODE_MASK       = 0x7;
    constexpr uint16_t IOREDTBL_INTVEC_MASK             = 0xff;
}

// The header for each entry in the MADT (see below) as described
// in ACPI 6.4 p.171
typedef struct {
    uint8_t type;
    uint8_t length;
} int_controller_header_t;

// The Processor Local APIC entries in MADT
// ACPI 6.4 p.172
typedef struct {
    int_controller_header_t h;
    uint8_t acpi_uid;
    uint8_t apic_id;
    uint32_t flags;
} __attribute__((packed)) proc_local_apic_t;

// The I/O APIC entry in MADT
// ACPI 6.4 p.173
typedef struct {
    int_controller_header_t h;
    uint8_t apic_id;
    uint8_t reserved;
    uint32_t apic_address;
    uint32_t interrupt_base;
} __attribute__((packed)) io_apic_t;

// The interrupt source override entry in MADT
// ACPI 6.4 p.174
typedef struct {
    int_controller_header_t h;
    uint8_t bus; // Always 0 (ISA)
    uint8_t source;
    uint32_t global_interrupt;
    uint16_t flags;
} __attribute__((packed)) int_source_override_t;

// The Local APIC interrupt input that each NMI is connected to
// for each of the processors that such a connection exists
// ACPI 6.4 p.176
typedef struct {
    int_controller_header_t h;
    uint8_t acpi_uid;
    uint16_t flags;
    uint8_t lint_number;
} __attribute__((packed)) local_apic_nmi_t;

// This optional structure supports 64-bit systems by providing an 
// override of the physical address of the local APIC inthe MADT’s 
// table header, which is defined as a 32-bit field
// ACPI 6.4 p.176
typedef struct {
    int_controller_header_t h;
    uint16_t reserved;
    uint64_t local_apic_addr;
} __attribute__((packed)) local_apic_addr_ovr_t;

// The I/O SAPIC structure is very similar to the I/O APIC structure.  
// If both I/O APIC and I/O SAPIC structures existfor a specific APIC ID, 
// the information in the I/O SAPIC structure must be used.
// ACPI 6.4 p.177
typedef struct {
    int_controller_header_t h;
    uint8_t io_apic_id;
    uint8_t reserved;
    uint32_t global_int_base;
    uint64_t io_sapic_addr;
} __attribute__((packed)) io_sapic_t;

// The Processor local SAPIC structure is very similar to the processor local APIC structure.  
// When using the SAPICinterrupt model, each processor in the system is required 
// to have a Processor Local SAPIC record in the MADT, anda processor device object in the DSDT.
// ACPI 6.4 p.177
typedef struct {
    int_controller_header_t h;
    uint8_t acpi_proc_id;
    uint8_t local_sapic_id;
    uint8_t local_sapic_eid;
    uint8_t reserved[3];
    uint32_t flags;
    uint32_t acpi_proc_uid;
    char acpi_proc_uid_str[0];
} __attribute__((packed)) local_sapic_t;

// The Platform Interrupt Source structure is used to communicate 
// which I/O SAPIC interrupt inputs are connected to
// the platform interrupt sources.
typedef struct {
    int_controller_header_t h;
    uint16_t flags;
    uint8_t int_type;
    uint8_t proc_id;
    uint8_t proc_eid;
    uint8_t io_sapic_vector;
    uint32_t global_sys_interrupt;
    uint32_t plat_int_src_flags;
} __attribute__((packed)) plat_int_src_t;

// The Multiple APIC Description Table holds all
// interrupts for the interrupt model that is to be used
// by the system (hardware could support multiple, but
// only one is chosen and remains unchanged)
// ACPI 6.4 p.170
typedef struct {
    acpi_desc_header_t h;
    uint32_t lic_address;
    uint32_t flags;
    uint8_t entries[0];
} __attribute__((packed)) madt_t;

class MADT {
public:
    static constexpr const char* signature = "APIC";

    MADT(void* data)
        :_data((madt_t *)data)
    {}

    size_t count() const;
    int_controller_header_t* get(size_t index) const;

    bool is_valid() const;

    const madt_t* data() const { return _data; }
private:
    madt_t* _data;
};

class LAPIC {
public:
    LAPIC(void* memoryAddress);

    uint32_t id() const;
    void set_id(uint32_t id);

    uint32_t version() const;

    uint32_t task_priority() const;
    void set_task_priority(uint32_t priority);

    uint32_t arbitration_priority() const;

    uint32_t processor_priority() const;

    void eoi(uint32_t val);

    uint32_t remote_read() const;

    uint32_t logical_destination() const;
    void set_logical_destination(uint32_t dest);

    uint32_t destination_format() const;
    void set_destination_format(uint32_t format);

    uint32_t spurious_interrupt_vector() const;
    void set_spurious_interrupt_vector(uint32_t vector);

    // uint64_t* in_service() const;

    // uint64_t* trigger_mode() const;

    // uint64_t* interrupt_request() const;

    uint32_t error_status() const;

    uint32_t cmci() const;
    void set_cmci(uint32_t cmci);

    void interrupt_command(uint32_t* lower, uint32_t* upper) const;
    void set_interrupt_command(uint32_t lower, uint32_t upper);

    uint32_t lvt_timer() const;
    void set_lvt_timer(uint32_t val);

    uint32_t lvt_thermal_sensor() const;
    void set_lvt_thermal_sensor(uint32_t val);

    uint32_t lvt_perf_monitor() const;
    void set_lvt_perf_monitor(uint32_t val);

    uint32_t lvt_lint0() const;
    void set_lvt_lint0(uint32_t val);

    uint32_t lvt_lint1() const;
    void set_lvt_lint1(uint32_t val);

    uint32_t lvt_error() const;
    void set_lvt_error(uint32_t val);

    uint32_t initial_count() const;
    void set_initial_count(uint32_t val);

    uint32_t current_count() const;

    uint32_t divide_config() const;
    void set_divide_config(uint32_t val);

private:
    volatile uint8_t* _memoryAddress;
};

enum IOAPICTriggerMode : uint8_t {
    Edge,
    Level
};

enum IOAPICPolarity : uint8_t {
    ActiveHigh,
    ActiveLow
};

enum IOAPICDestinationMode : uint8_t {
    Physical,
    Logical
};

enum IOAPICDeliveryMode : uint8_t {
    Fixed,
    LowestPriority,
    SMI,
    NMI = 0b100,
    INIT,
    ExtINT = 0b111
};

class IOAPICRedirectionEntry {
public:
    IOAPICRedirectionEntry()
    { 

    }

    IOAPICRedirectionEntry(uint64_t existing)
        :_val(existing)
    {
        
    }

    uint8_t destination_field() const;
    IOAPICRedirectionEntry& set_destination_field(uint8_t df);

    bool interrupt_masked() const;
    IOAPICRedirectionEntry& set_interrupt_masked(bool masked);

    IOAPICTriggerMode trigger_mode() const;
    IOAPICRedirectionEntry& set_trigger_mode(IOAPICTriggerMode mode);

    bool remote_irr() const;

    IOAPICPolarity interrupt_pin_polarity() const;
    IOAPICRedirectionEntry& set_interrupt_pin_polarity(IOAPICPolarity polarity);

    bool is_delivery_pending() const;

    IOAPICDestinationMode destination_mode() const;
    IOAPICRedirectionEntry& set_destination_mode(IOAPICDestinationMode mode);

    IOAPICDeliveryMode delivery_mode() const;
    IOAPICRedirectionEntry& set_delivery_mode(IOAPICDeliveryMode mode);

    uint8_t interrupt_vector() const;
    IOAPICRedirectionEntry& set_interrupt_vector(uint8_t vector);

    explicit operator uint64_t() const {
        return _val;
    }
private:
    uint64_t _val {0};
};

class IOAPIC {
public:
    IOAPIC(void* memoryAddress)
        :_memoryAddress((volatile uint8_t *)memoryAddress)
    {

    }

    uint8_t selected_register() const;
    void select_register(uint8_t reg);

    uint32_t read_current() const;
    void write_current(uint32_t val);

    uint8_t ioapic_id();
    void set_ioapid_id(uint8_t id);

    uint8_t max_redirection_entry();
    uint8_t apic_version();

    uint8_t ioapic_arbitration_id();
    void set_ioapic_arbitration_id(uint8_t id);

    IOAPICRedirectionEntry redirection_entry(size_t index);
    void set_redirection_entry(const IOAPICRedirectionEntry& entry, size_t index);

private:
    volatile uint8_t* _memoryAddress;
};

void apic_init(madt_t* madt);