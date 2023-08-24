#include <lib/log.h>
#include <lib/lock.h>
#include <impl/vmm.h>
#include <impl/time.h>
#include <global/pmm.h>
#include <global/heap.h>

#include <arch/include.h>
#include ARCH_INCLUDE(io.h)
#include ARCH_INCLUDE(asm.h)
#include ARCH_INCLUDE(smp.h)
#include ARCH_INCLUDE(cpu.h)
#include ARCH_INCLUDE(apic.h)
#include ARCH_INCLUDE(interrupts.h)
#include ARCH_INCLUDE(impl/arch.h)

//processors
struct local_processor** processors;
uint8_t processor_count;

struct lapic_address** lapic_address;

//ioapic
struct ioapic** ioapics;
uint64_t ioapic_count;

//iso
struct interrupt_source_override** isos;
uint64_t iso_count;

void apic_init(struct acpi_madt_header* madt){
    processor_count = 0;
    iso_count = 0;
    uint64_t max_apicid = 1;

    uint64_t entries = (madt->header.length - sizeof(struct acpi_madt_header));

    for(uint64_t i = 0; i < entries;){
        struct entry_record* entry_record = (struct entry_record*)((uint64_t)madt + sizeof(struct acpi_madt_header) + i);
        i += entry_record->length;

        switch(entry_record->type){
            case entry_type_local_processor: {
                struct local_processor* processor = (struct local_processor*)entry_record;
                if(processor->apicid > max_apicid){
                    max_apicid = processor->apicid;
                }  
                processor_count++;      
                break;                    
            }
            case entry_type_ioapic:{
                ioapic_count++;
                break;
            }                    
            case entry_type_interrupt_source_override:{
                iso_count++;
                break;
            }                    
            case entry_type_nonmaskableinterrupts:{
                break;
            }                    
            case entry_type_local_apic_address_override:{
                break;  
            }                                      
        }
    }
    processors = (struct local_processor**)malloc(sizeof(struct local_processor*) * processor_count);
    lapic_address = (struct lapic_address**)malloc(sizeof(struct lapic_address*) * (max_apicid + 1));

    ioapics = (struct ioapic**)malloc(sizeof(struct ioapic*) * ioapic_count);

    isos = (struct interrupt_source_override**)malloc(sizeof(struct interrupt_source_override*) * iso_count);

    uint8_t processor_count_tmp = 0;
    uint64_t iso_count_tmp = 0;
    uint8_t ioapic_tmp = 0;

    for(uint64_t i = 0; i < entries;){            
        struct entry_record* entry_record = (struct entry_record*)((uint64_t)madt + sizeof(struct acpi_madt_header) + i);
        i += entry_record->length;

        switch(entry_record->type){
            case entry_type_local_processor: {
                struct local_processor* entry_processor = (struct local_processor*)entry_record;
                processors[processor_count_tmp] = entry_processor;  
                lapic_address[entry_processor->apicid] = (struct lapic_address*)malloc(sizeof(struct lapic_address));
                processor_count_tmp++;
                break;
            }
            case entry_type_ioapic:{
                struct ioapic* entry_ioapic = (struct ioapic*)entry_record;
                ioapics[ioapic_tmp] = entry_ioapic;
                ioapic_tmp++;
                break;
            }                    
            case entry_type_interrupt_source_override:{
                struct interrupt_source_override* entry_iso = (struct interrupt_source_override*)entry_record;
                isos[iso_count_tmp] = entry_iso;
                iso_count_tmp++;
                break;
            }                    
            case entry_type_nonmaskableinterrupts:{
                struct non_maskableinterrupts* nmi = (struct non_maskableinterrupts*)entry_record;
                break;
            }                    
            case entry_type_local_apic_address_override:{
                struct local_apic_address_override* local_apic_override = (struct local_apic_address_override*)entry_record;
                break;  
            }                                      
        }
    }

    for(uint64_t i = 0; i < ioapic_count; i++){
        io_apic_init(i);
    }
}  

void io_apic_init(uint8_t io_apic_id){
    // disable pic
    io_write8(0xa1, 0xff);
    io_write8(0x21, 0xff);
    
    enable_apic(cpu_get_apicid());

    // configure first ioapic
    struct ioapic* ioapic = ioapics[io_apic_id];
    void* ioapic_address_virtual = vmm_get_virtual_address((void*)(uint64_t)ioapic->apic_address);
    uint8_t max_interrupts = ((ioapic_read_register((void*)ioapic_address_virtual, ioapic_version) >> 16) & 0xff) + 1;
    ioapic->max_interrupts = max_interrupts;

    // set up the entries
    uint32_t base = ioapic->global_system_interrupt_base;

    for (size_t i = 0; i < max_interrupts; i++){
            uint8_t irq_number = i + 0x20;
            io_apic_set_redirection_entry((void*)ioapic_address_virtual, i - base, (struct ioapic_redirection_entry){
                .vector = irq_number,
                .delivery_mode = ioapic_redirection_entry_delivery_mode_fixed,
                .destination_mode = ioapic_redirection_entry_destination_mode_physical,
                .delivery_status = ioapic_redirection_entry_delivery_status_iddle,
                .pin_polarity = ioapic_redirection_entry_pin_polarity_active_high,
                .remote_irr = ioapic_redirection_entry_remote_irr_none,
                .trigger_mode = ioapic_redirection_entry_trigger_mode_edge,
                .mask = ioapic_redirection_entry_mask_disable,
                .destination = 0,
            });
    }

    for(size_t i = 0; i < iso_count; i++) {
        struct interrupt_source_override* iso = isos[i];
        uint8_t irq_number = iso->irq_source + 0x20;
        io_apic_set_redirection_entry((void*)ioapic_address_virtual, iso->irq_source, (struct ioapic_redirection_entry){
            .vector = irq_number,
            .delivery_mode = ioapic_redirection_entry_delivery_mode_fixed,
            .destination_mode = ioapic_redirection_entry_destination_mode_physical,
            .delivery_status = ioapic_redirection_entry_delivery_status_iddle,
            .pin_polarity = (iso->flags & 0x03) == 0x03 ? ioapic_redirection_entry_pin_polarity_active_low : ioapic_redirection_entry_pin_polarity_active_high,
            .remote_irr = ioapic_redirection_entry_remote_irr_none,
            .trigger_mode = (iso->flags & 0x0c) == 0x0c ? ioapic_redirection_entry_trigger_mode_level : ioapic_redirection_entry_trigger_mode_edge,
            .mask = ioapic_redirection_entry_mask_disable,
            .destination = 0,
        });
    } 
} 

void io_change_irq_state(uint8_t irq, uint8_t io_apic_id, bool is_enable){
    struct ioapic* ioapic = ioapics[io_apic_id];
    void* ioapic_address_virtual = vmm_get_virtual_address((void*)(uint64_t)ioapic->apic_address);
    uint32_t base = ioapic->global_system_interrupt_base;
    size_t index = irq - base;
    
    volatile uint32_t low = 0;

    low = ioapic_read_register(ioapic_address_virtual, ioapic_redirection_table + 2 * index);
    
    if(!is_enable){
        low |= 1 << ioapic_redirection_bits_low_mask;
    }else{
        low &= ~(1 << ioapic_redirection_bits_low_mask);
    }

    ioapic_write_register(ioapic_address_virtual, ioapic_redirection_table + 2 * index, low);
}

/* this function need apic to be init */
void smp_init(void){
    void* trampoline_virtual_address = (void*)vmm_get_virtual_address((void*)0x1000);

    memcpy((void*)trampoline_virtual_address, (void*)&trampoline_entry, PAGE_SIZE);

    struct trampoline_data* data = (struct trampoline_data*)(((uint64_t)&data_trampoline - (uint64_t)&trampoline_entry) + (uint64_t)trampoline_virtual_address);

    
    //tmp trampoline map
    vmm_map(kernel_space, (memory_range_t){ (void*)TRAMPOLINE_ADDRESS, PAGE_SIZE }, (memory_range_t){ (void*)TRAMPOLINE_ADDRESS, PAGE_SIZE }, MEMORY_FLAG_READABLE | MEMORY_FLAG_WRITABLE | MEMORY_FLAG_EXECUTABLE);

    for(int i = 0; i < processor_count; i++){ 
        if(processors[i]->apicid == cpu_get_apicid()) continue; 

        data->paging = kernel_space;
        data->main_entry = &trampoline_main; 
        data->stack = (void*)((uintptr_t)malloc(KERNEL_STACK_SIZE) + KERNEL_STACK_SIZE);
        data_trampoline.main_entry = data->main_entry;
        data_trampoline.stack = data->stack;
        data_trampoline.stack_scheduler = (void*)((uintptr_t)malloc(KERNEL_STACK_SIZE) + KERNEL_STACK_SIZE);

        lapic_send_init_ipi(processors[i]->apicid);

        data_trampoline.status = 0;
        
        log_info("wait processor %d...\n", i);

        // send startup ipi twice 
        lapic_send_startup_ipi(processors[i]->apicid, (void*)TRAMPOLINE_ADDRESS);

        while (data_trampoline.status != 0xef){
            __asm__ __volatile__ ("pause" : : : "memory");
        } 
        log_info("processor %d respond with success\n", i);
    }
    data_trampoline.status = 0xff;
    vmm_unmap(kernel_space, (memory_range_t){ (void*)TRAMPOLINE_ADDRESS, PAGE_SIZE });

}  

void* get_lapic_address(void){
    return lapic_address[cpu_get_apicid()]->virtual_address;
}

void enable_apic(uint8_t cpu_id){
    lapic_address[cpu_id]->physical_address = (void*)(rdmsr(0x1b) & 0xfffff000);
    lapic_address[cpu_id]->virtual_address = vmm_get_virtual_address(lapic_address[cpu_id]->physical_address);

    // reset registers recommanded by intel : dfr, ldr and tpr
    local_apic_write_register(get_lapic_address(), local_apic_register_offset_destination_format, 0xffffffff);
    local_apic_write_register(get_lapic_address(), local_apic_register_offset_logical_destination, (local_apic_read_register(get_lapic_address(), local_apic_register_offset_logical_destination) & ~((0xff << 24)) | (cpu_id << 24)));
    local_apic_write_register(get_lapic_address(), local_apic_register_offset_spurious_int_vector, local_apic_read_register(get_lapic_address(), local_apic_register_offset_spurious_int_vector) | (LOCAL_APIC_SPURIOUS_ALL | LOCAL_APIC_SPURIOUS_ENABLE_APIC));
    local_apic_write_register(get_lapic_address(), local_apic_register_offset_task_priority, 0);
    wrmsr(0x1b, ((uint64_t)lapic_address[cpu_id]->physical_address | LOCAL_APIC_ENABLE) & ~((1 << 10)));
}

void start_lapic_timer(void){
    // setup local apic timer
    local_apic_write_register(get_lapic_address(), local_apic_register_offset_divide, 4);        
    local_apic_write_register(get_lapic_address(), local_apic_register_offset_initial_count, 0xffffffff);

    kernel_sleep_ms(10);
    
    uint32_t tick10ms = 0xffffffff - local_apic_read_register(get_lapic_address(), local_apic_register_offset_curent_count);

    struct local_apic_interrupt_register timer_registers;

    /* don't forget to define all the struct because it can be corrupt by the stack */
    timer_registers.vector = INT_SCHEDULE_APIC_TIMER;
    timer_registers.message_type = local_apic_interrupt_register_message_type_fixed;
    timer_registers.delivery_status = local_apic_interrupt_register_message_type_iddle;
    timer_registers.remote_irr = local_apic_interrupt_register_remote_irr_completed;
    timer_registers.trigger_mode = local_apic_interrupt_register_trigger_mode_edge;
    timer_registers.mask = local_apic_interrupt_register_mask_enable;
    timer_registers.timer_mode = local_apic_interrupt_timer_mode_periodic;
    
    uint32_t timer = local_apic_read_register(get_lapic_address(), local_apic_register_offset_lvt_timer);
    local_apic_write_register(get_lapic_address(), local_apic_register_offset_lvt_timer, create_register_value_interrupts(timer_registers) | (timer & 0xfffcef00));    
    local_apic_write_register(get_lapic_address(), local_apic_register_offset_initial_count, (tick10ms / 10)); 
}

void local_apic_set_timer_count(uint32_t value){
    local_apic_write_register(get_lapic_address(), local_apic_register_offset_initial_count, value);
}

uint32_t local_apic_get_timer_count(void){
    return local_apic_read_register(get_lapic_address(), local_apic_register_offset_curent_count);
}

void lapic_send_init_ipi(uint8_t cpu_id){
    struct local_apic_ipi register_interrupt;
    register_interrupt.vector = 0;
    register_interrupt.delivery_mode = local_apic_delivery_mode_init;
    register_interrupt.destination_mode = local_apic_destination_mode_physical_destination;
    register_interrupt.destination_type = local_apic_destination_type_base;
    uint32_t command_low = create_local_apic_ipi_register(register_interrupt);
    uint32_t command_high = cpu_id << 24;
    set_command_ipi(command_low, command_high);
}

void lapic_send_startup_ipi(uint8_t cpu_id, void* entry){
    struct local_apic_ipi register_interrupt;
    register_interrupt.vector = (uint8_t)(((uint64_t)entry / PAGE_SIZE) & 0xff);
    register_interrupt.delivery_mode = local_apic_delivery_mode_start_up;
    register_interrupt.destination_mode = local_apic_destination_mode_physical_destination;
    register_interrupt.destination_type = local_apic_destination_type_base;
    uint32_t command_low = create_local_apic_ipi_register(register_interrupt);
    uint32_t command_high = cpu_id << 24;
    set_command_ipi(command_low, command_high);
}

void local_apic_eoi(uint8_t cpu_id){        
    local_apic_write_register(lapic_address[cpu_id]->virtual_address, local_apic_register_offset_eoi, 0);
}

void local_apic_enable_spurious_interrupts(void){
    local_apic_write_register(get_lapic_address(), local_apic_register_offset_spurious_int_vector, local_apic_read_register(get_lapic_address(), local_apic_register_offset_spurious_int_vector) | 0x100);
}

/* apic */

uint32_t local_apic_read_register(void* lapic_address, size_t offset){
    return *((volatile uint32_t*)((void*)((uint64_t)lapic_address + offset)));
}

uint32_t ioapic_read_register(void* apic_ptr , uint8_t offset){
    *(volatile uint32_t*)(apic_ptr) = offset;
    return *(volatile uint32_t*)((uint64_t)apic_ptr + 0x10);
}

void ioapic_write_register(void* apic_ptr , uint8_t offset, uint32_t value){
    *(volatile uint32_t*)(apic_ptr) = offset;
    *(volatile uint32_t*)((uint64_t)apic_ptr + 0x10) = value;
}

void local_apic_write_register(void* lapic_address, size_t offset, uint32_t value){
    *((volatile uint32_t*)((void*)((uint64_t)lapic_address + offset))) = value;
}

uint32_t create_register_value_interrupts(struct local_apic_interrupt_register reg){
    return (
        (reg.vector << local_apic_interrupt_vector) |
        (reg.message_type << local_apic_interrupt_message_type) |
        (reg.delivery_status << local_apic_interrupt_delivery_status) |
        (reg.trigger_mode << local_apic_interrupt_triger_mode) |
        (reg.mask << local_apic_interrupt_mask) |
        (reg.timer_mode << local_apic_interrupt_timer_mode)
    );
}

void io_apic_set_redirection_entry(void* apic_ptr, size_t index, struct ioapic_redirection_entry entry){
    volatile uint32_t low = (
        (entry.vector << ioapic_redirection_bits_low_vector) |
        (entry.delivery_mode << ioapic_redirection_bits_low_delivery_mode) |
        (entry.destination_mode << ioapic_redirection_bits_low_destination_mode) |
        (entry.delivery_status << ioioapic_redirection_bits_low_delivery_status) |
        (entry.pin_polarity << ioapic_redirection_bits_low_pon_polarity) |
        (entry.remote_irr << ioapic_redirection_bits_low_remote_irr) |
        (entry.trigger_mode << ioapic_redirection_bits_low_trigger_mode) |
        (entry.mask << ioapic_redirection_bits_low_mask)
    );
    volatile uint32_t high = (
        (entry.destination << ioapic_redirection_bits_high_destination)
    );
    
    ioapic_write_register(apic_ptr, ioapic_redirection_table + 2 * index, low);
    ioapic_write_register(apic_ptr, ioapic_redirection_table + 2 * index + 1, high);
}

void set_command_ipi(uint32_t command_low, uint32_t command_high){
    void* lapic_address = get_lapic_address();
    local_apic_write_register(lapic_address, local_apic_register_offset_error_status, 0);
    local_apic_write_register(lapic_address, local_apic_register_offset_interrupt_command_high, command_high);
    local_apic_write_register(lapic_address, local_apic_register_offset_interrupt_command_low, command_low);
    do { __asm__ __volatile__ ("pause" : : : "memory"); }while(local_apic_read_register(lapic_address, local_apic_register_offset_interrupt_command_low) & (1 << 12));
}

void generate_interruption(uint64_t cpu_id, uint8_t vector){
    struct local_apic_ipi register_interrupt;
    register_interrupt.vector = vector;
    register_interrupt.delivery_mode = local_apic_delivery_mode_fixed;
    register_interrupt.destination_mode = local_apic_destination_mode_physical_destination;
    register_interrupt.destination_type = local_apic_destination_type_base;
    uint32_t command_low = create_local_apic_ipi_register(register_interrupt);
    uint32_t command_high = cpu_id << 24;
    
    set_command_ipi(command_low, command_high);
}

void generate_interruption_nmi(uint64_t cpu_id){
    struct local_apic_ipi register_interrupt;
    register_interrupt.vector = 0x0;
    register_interrupt.delivery_mode = local_apic_delivery_mode_nmi;
    register_interrupt.destination_mode = local_apic_destination_mode_physical_destination;
    register_interrupt.destination_type = local_apic_destination_type_base;
    uint32_t command_low = create_local_apic_ipi_register(register_interrupt);
    uint32_t command_high = cpu_id << 24;
    
    set_command_ipi(command_low, command_high);
}

uint32_t create_local_apic_ipi_register(struct local_apic_ipi reg){
    return (
        (reg.vector << local_apic_interruptipi_vector) |
        (reg.delivery_mode << local_apic_interruptipi_message_type) |
        (reg.destination_mode << local_apic_interruptipi_destination_mode) |
        (reg.destination_type << local_apic_interruptipi_destination_type)
    );
}