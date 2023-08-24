#include <stdint.h>
#include <stdbool.h>
#include <impl/vmm.h>
#include <impl/time.h>
#include <sys/types.h>

#include <arch/include.h>
#include ARCH_INCLUDE(acpi.h)

#define MILLISECOND_TO_FEMOSECOND(ms)     ms * 1000000000000 // femosecond 10E15 to microsecond 10E3

#define HPET_TIMER_OFFSET_GENERAL_CAPABILITIES_ID 0x0
#define HPET_TIMER_OFFSET_GENERAL_CONFIGURATION 0x10
#define HPET_TIMER_OFFSET_MAIN_COUNTER_VALUES 0xF0

#define HPET_GENERAL_CAPABILITIES_ID_COUNTER_PERIOD 32

#define HPET_GENERAL_CONFIGURATION_TIMER_INTERRUPT 0
#define HPET_GENERAL_CONFIGURATION_LEGACY_MAPPING 1

#define HPET_TIMER_OFFSET_TIMER_SPACE_DATA_START 0x100
#define HPET_TIMER_OFFSET_TIMER_SPACE_SIZE 0x20
#define HPET_TIMER_OFFSET_TIMER_CONFIG_CAPABILITY_REGISTER 0x0
#define HPET_TIMER_OFFSET_TIMER_COMPARATOR_VALUE_REGISTER 0x8
#define HPET_TIMER_OFFSET_TIMER_FSB_INTERRUPT_ROUTE_REGISTER 0x10


static void* hpet_base;
static uint64_t hpet_frequency;


static uint64_t hpet_read_register(uint64_t offset){
    return *((volatile uint64_t*)((void*)((uint64_t)hpet_base + offset)));
}

static void hpet_write_register(uint64_t offset, uint64_t value){
    *((volatile uint64_t*)((void*)((uint64_t)hpet_base + offset))) = value;
}

static void hpet_change_main_timer_interrupt_state(bool is_enable){
    uint64_t general_configuration_register_data = hpet_read_register(HPET_TIMER_OFFSET_GENERAL_CONFIGURATION);
    general_configuration_register_data |= is_enable << HPET_GENERAL_CONFIGURATION_TIMER_INTERRUPT; 
    hpet_write_register(HPET_TIMER_OFFSET_GENERAL_CONFIGURATION, general_configuration_register_data);
} 

static void hpet_change_main_timer_legacy_mapping_state(bool is_enable){
    uint64_t general_configuration_register_data = hpet_read_register(HPET_TIMER_OFFSET_GENERAL_CONFIGURATION);
    general_configuration_register_data |= is_enable << HPET_GENERAL_CONFIGURATION_LEGACY_MAPPING; 
    hpet_write_register(HPET_TIMER_OFFSET_GENERAL_CONFIGURATION, general_configuration_register_data);
}

void hpet_init(struct acpi_hpet_header* hpet){
    hpet_base = (void*)vmm_get_virtual_address((void*)hpet->address.address);

    hpet_frequency = hpet_read_register(HPET_TIMER_OFFSET_GENERAL_CAPABILITIES_ID) >> HPET_GENERAL_CAPABILITIES_ID_COUNTER_PERIOD;
    
    hpet_change_main_timer_interrupt_state(false);
    hpet_write_register(HPET_TIMER_OFFSET_MAIN_COUNTER_VALUES, 0);
    hpet_change_main_timer_interrupt_state(true);
}   


ms_t hpet_get_current_time(void){
    return hpet_read_register(HPET_TIMER_OFFSET_MAIN_COUNTER_VALUES);
}

void hpet_sleep(ms_t ms){
    uint64_t end = hpet_read_register(HPET_TIMER_OFFSET_MAIN_COUNTER_VALUES) + (MILLISECOND_TO_FEMOSECOND(ms)) / hpet_frequency;
    while(hpet_read_register(HPET_TIMER_OFFSET_MAIN_COUNTER_VALUES) <= end){
        __asm__ volatile ("pause" : : : "memory");
    }
}