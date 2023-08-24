#ifndef _AMD64_APIC_H
#define _AMD64_APIC_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#include <arch/include.h>
#include ARCH_INCLUDE(acpi.h)

#define LOCAL_APIC_ENABLE               0x800
#define LOCAL_APIC_SPURIOUS_ALL         0x100
#define LOCAL_APIC_SPURIOUS_ENABLE_APIC 0xff

#define TRAMPOLINE_ADDRESS              0x1000
#define TRAMPOLINE_SIZE                 0x1000
#define TRAMPOLINE_END                  TRAMPOLINE_ADDRESS + TRAMPOLINE_SIZE

struct entry_record{
    uint8_t type;
    uint8_t length;
}__attribute__((packed));

//entry type 0 : processor local apic
struct local_processor{
    struct entry_record record;
    uint8_t processor_id;
    uint8_t apicid;
    uint32_t flags; //bit 0 = processor enabled / bit 1 = online capable
}__attribute__((packed));

//entry type 1 : i/o apic
struct ioapic{
    struct entry_record record;
    uint8_t apicid;
    uint8_t reserved;
    uint32_t apic_address;
    uint32_t global_system_interrupt_base;
    uint8_t	max_interrupts;
}__attribute__((packed));

//entry type 2 : interrupt source override
struct interrupt_source_override{
    struct entry_record record;
    uint8_t bus_source;
    uint8_t irq_source;
    uint32_t global_system_interrupt;
    uint16_t flags;
}__attribute__((packed));

//entry type 4 : non-maskable interrupts
struct non_maskableinterrupts{
    struct entry_record record;
    uint8_t acpi_processor_id; //0x_ff means all processors
    uint16_t flags;
    uint8_t lint; //0 or 1
}__attribute__((packed));

//entry type 5 : local apic address override
struct local_apic_address_override{
    struct entry_record record;
    uint8_t reserved;
    uint64_t local_apic_address;
}__attribute__((packed));

enum entry_type{
    entry_type_local_processor = 0,
    entry_type_ioapic = 1,
    entry_type_interrupt_source_override = 2,
    entry_type_nonmaskableinterrupts = 4,
    entry_type_local_apic_address_override = 5,
};

enum local_apic_interrupt{
    local_apic_interrupt_vector			= 0,
    local_apic_interrupt_message_type		= 8,
    local_apic_interrupt_delivery_status	= 12,
    local_apic_interrupt_triger_mode		= 15,
    local_apic_interrupt_mask				= 16,
    local_apic_interrupt_timer_mode		    = 17,
};
enum local_apic_interrupt_register_message_type{
    local_apic_interrupt_register_message_type_fixed		= 0b000,
    local_apic_interrupt_register_message_type_smi		= 0b010,
    local_apic_interrupt_register_message_type_nmi		= 0b100,
    local_apic_interrupt_register_message_type_extint		= 0b111,
};
enum local_apic_interrupt_register_delivery_status{
    local_apic_interrupt_register_message_type_iddle	    = 0,
    local_apic_interrupt_register_message_type_pending	= 1
};
enum local_apic_interrupt_register_remote_irr{
    local_apic_interrupt_register_remote_irr_completed	= 0,
    local_apic_interrupt_register_remote_irr_accepted	    = 1,
};
enum local_apic_interrupt_register_trigger_mode{
    local_apic_interrupt_register_trigger_mode_edge		= 0,
    local_apic_interrupt_register_trigger_mode_level	    = 1,
};
enum local_apic_interrupt_register_mask{
    local_apic_interrupt_register_mask_enable            = 0, 
    local_apic_interrupt_register_mask_disable           = 1,
};
enum local_apic_interrupt_timer_mode{
    local_apic_interrupt_timer_mode_one_shot	= 0,
    local_apic_interrupt_timer_mode_periodic	= 1
};

enum local_apic_destination_mode{
    local_apic_destination_mode_physical_destination = 0,
    local_apic_destination_mode_virtual_destination = 1
};

enum destination_type{
    local_apic_destination_type_base = 0,
    local_apic_destination_type_send_interrupt_self = 1,
    local_apic_destination_type_send_all_processor = 2,
    local_apic_destination_type_send_all_but_not_self = 3
};

struct local_apic_interrupt_register{
    uint8_t	vector:8;
    enum local_apic_interrupt_register_message_type message_type:3;
    enum local_apic_interrupt_register_delivery_status delivery_status:1;
    enum local_apic_interrupt_register_remote_irr remote_irr:1;
    enum local_apic_interrupt_register_trigger_mode trigger_mode:1;
    enum local_apic_interrupt_register_mask	mask:1;
    enum local_apic_interrupt_timer_mode timer_mode:1;
};

enum local_apic_interruptipi{
    local_apic_interruptipi_vector             = 0,
    local_apic_interruptipi_message_type        = 8,
    local_apic_interruptipi_destination_mode    = 11,
    local_apic_interruptipi_destination_type    = 18
};

enum local_apic_delivery_mode{
    local_apic_delivery_mode_fixed		= 0b000,
    local_apic_delivery_mode_smi		= 0b010,
    local_apic_delivery_mode_nmi		= 0b100,
    local_apic_delivery_mode_init		= 0b101,
    local_apic_delivery_mode_start_up	= 0b110,
    local_apic_delivery_mode_reserved	= 0b111,
};

struct local_apic_ipi{
    uint8_t	vector:8;
    enum local_apic_delivery_mode delivery_mode:3;
    enum local_apic_destination_mode destination_mode:1;
    enum destination_type destination_type:2;
};

enum local_apic_register_offset {
    local_apic_register_offset_id					    = 0x020,
    local_apic_register_offset_version				    = 0x030,
    local_apic_register_offset_task_priority			    = 0x080,
    local_apic_register_offset_arbitration_priority	    = 0x090,
    local_apic_register_offset_processor_priority	    = 0x0a0,
    local_apic_register_offset_eoi					    = 0x0b0,
    local_apic_register_offset_remote_read			    = 0x0c0,
    local_apic_register_offset_logical_destination	    = 0x0d0,
    local_apic_register_offset_destination_format	    = 0x0e0,
    local_apic_register_offset_spurious_int_vector	    = 0x0f0,
    local_apic_register_offset_in_service		        = 0x100,
    local_apic_register_offset_trigger_mode		        = 0x180,
    local_apic_register_offset_interruptd_request		= 0x200,
    local_apic_register_offset_error_status		        = 0x280,
    local_apic_register_offset_cmci					    = 0x2f0,
    local_apic_register_offset_interrupt_command_low		= 0x300,
    local_apic_register_offset_interrupt_command_high		= 0x310,
    local_apic_register_offset_lvt_timer				    = 0x320,
    local_apic_register_offset_lvt_thermal_sensor	        = 0x330,
    local_apic_register_offset_lvt_perfommance_monitor	= 0x340,
    local_apic_register_offset_lvtlint0				    = 0x350,
    local_apic_register_offset_lvtlint1				    = 0x360,
    local_apic_register_offset_lvterror				    = 0x370,
    local_apic_register_offset_initial_count			    = 0x380,
    local_apic_register_offset_curent_count			    = 0x390,
    local_apic_register_offset_divide			        = 0x3e0,
};

enum ioapic_register_offset{
    ioapic_id				= 0x00,
    ioapic_version			= 0x01,
    ioapic_arbitration		= 0x02,
    ioapic_redirection_table	= 0x10,
};

enum ioapic_redirection_bits_low {
    ioapic_redirection_bits_low_vector				= 0,
    ioapic_redirection_bits_low_delivery_mode		= 8,
    ioapic_redirection_bits_low_destination_mode	    = 11,
    ioioapic_redirection_bits_low_delivery_status	= 12,
    ioapic_redirection_bits_low_pon_polarity		    = 13,
    ioapic_redirection_bits_low_remote_irr			= 14,
    ioapic_redirection_bits_low_trigger_mode		    = 15,
    ioapic_redirection_bits_low_mask				= 16,
};

enum ioapic_redirection_bits_high {
    ioapic_redirection_bits_high_destination	    = 24
};

enum ioapic_redirection_entry_delivery_mode {
    ioapic_redirection_entry_delivery_mode_fixed			    = 0b000,
    ioapic_redirection_entry_delivery_mode_low_priority		= 0b001,
    ioapic_redirection_entry_delivery_mode_smi				= 0b010,
    ioapic_redirection_entry_delivery_mode_nmi				= 0b100,
    ioapic_redirection_entry_delivery_mode_init				= 0b101,
    ioapic_redirection_entry_delivery_mode_extint			= 0b111,
};
enum ioapic_redirection_entry_destination_mode {
    ioapic_redirection_entry_destination_mode_physical	= 0,
    ioapic_redirection_entry_destination_mode_logical	= 1
};
enum ioapic_redirection_entry_delivery_status {
    ioapic_redirection_entry_delivery_status_iddle		= 0,
    ioapic_redirection_entry_delivery_status_pending	    = 1
};
enum ioapic_redirection_entry_pin_polarity {
    ioapic_redirection_entry_pin_polarity_active_high	    = 0,
    ioapic_redirection_entry_pin_polarity_active_low		= 1
};
enum ioapic_redirection_entry_remote_irr {
    ioapic_redirection_entry_remote_irr_none		= 0,
    ioapic_redirection_entry_remote_irr_inflight	= 1,
};
enum ioapic_redirection_entry_trigger_mode {
    ioapic_redirection_entry_trigger_mode_edge	= 0,
    ioapic_redirection_entry_trigger_mode_level	= 1
};
enum ioapic_redirection_entry_mask {
    ioapic_redirection_entry_mask_enable	= 0,	// masks the interrupt through
    ioapic_redirection_entry_mask_disable	= 1		// masks the so it doesn't go through
};

struct ioapic_redirection_entry {
    uint8_t										    vector:8;
    enum ioapic_redirection_entry_delivery_mode		delivery_mode:3;
    enum ioapic_redirection_entry_destination_mode	destination_mode:1;
    enum ioapic_redirection_entry_delivery_status	delivery_status:1;
    enum ioapic_redirection_entry_pin_polarity		pin_polarity:1;
    enum ioapic_redirection_entry_remote_irr		remote_irr:1;
    enum ioapic_redirection_entry_trigger_mode		trigger_mode:1;
    enum ioapic_redirection_entry_mask				mask:1;
    uint8_t										    destination:8;
}__attribute__((packed));

struct lapic_address{
    void* physical_address;
    void* virtual_address;
}__attribute__((packed));

struct sdt_header{
    uint8_t signature[4];
    uint32_t length;
    uint8_t revision;
    uint8_t checksum;
    uint8_t oemid[6];
    uint8_t oem_table_id[8];
    uint32_t oem_revision;
    uint32_t creator_id;
    uint32_t creator_revision;
}__attribute__((packed));

void apic_init(struct acpi_madt_header* madt);
void io_apic_init(uint8_t io_apic_id);
void io_change_irq_state(uint8_t irq, uint8_t io_apic_id, bool is_enable);
void smp_init(void);
void* get_lapic_address(void);
void enable_apic(uint8_t cpu_id);
void start_lapic_timer(void);
void local_apic_set_timer_count(uint32_t value);
uint32_t local_apic_get_timer_count(void);
void lapic_send_init_ipi(uint8_t cpu_id);
void lapic_send_startup_ipi(uint8_t cpu_id, void* entry);
void local_apic_eoi(uint8_t cpu_id);
void local_apic_enable_spurious_interrupts(void);
uint32_t local_apic_read_register(void* lapic_address, size_t offset);
uint32_t ioapic_read_register(void* apic_ptr , uint8_t offset);
void ioapic_write_register(void* apic_ptr , uint8_t offset, uint32_t value);
void local_apic_write_register(void* lapic_address, size_t offset, uint32_t value);    
uint32_t create_register_value_interrupts(struct local_apic_interrupt_register reg);
uint32_t create_local_apic_ipi_register(struct local_apic_ipi reg);
void io_apic_set_redirection_entry(void* apic_ptr, size_t index, struct ioapic_redirection_entry entry);
void set_command_ipi(uint32_t command_low, uint32_t command_high);
void generate_interruption(uint64_t cpu_id, uint8_t vector);
void generate_interruption_nmi(uint64_t cpu_id);

extern uint8_t processor_count;

#endif // _AMD64_APIC_H