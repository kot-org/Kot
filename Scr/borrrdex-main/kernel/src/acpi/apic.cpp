#include "apic.h"
#include "string.h"
#include "KernelUtil.h"
#include "arch/x86_64/pic.h"
#include <type_traits>

using namespace std;

size_t MADT::count() const {
    uint8_t* start = _data->entries;
    uint8_t* current = start;
    size_t len = _data->h.length - sizeof(_data->h);

    size_t total = 0;
    while(current - start < len) {
        int_controller_header_t* h = (int_controller_header_t *)current;
        total++;
        current += h->length;
    }

    return total;
}

int_controller_header_t* MADT::get(size_t index) const {
    uint8_t* start = _data->entries;
    uint8_t* current = start;
    size_t len = _data->h.length - sizeof(_data->h);
    size_t currentIdx = 0;
    while(current - start < len) {
        int_controller_header_t* h = (int_controller_header_t *)current;
        if(currentIdx++ == index) {
            return h;
        }

        current += h->length;
    }

    return nullptr;
}

bool MADT::is_valid() const {
    return _data && strncmp(MADT::signature, _data->h.signature, 4) == 0;
}

LAPIC::LAPIC(void* memoryAddress)
    :_memoryAddress((uint8_t *)memoryAddress)
{

}

uint32_t LAPIC::id() const {
    return *((volatile uint32_t *)(_memoryAddress + lapic::REG_OFFSET_ID));
}

void LAPIC::set_id(uint32_t val) {
    *((volatile uint32_t *)(_memoryAddress + lapic::REG_OFFSET_ID)) = val;
}

uint32_t LAPIC::version() const {
    return *((volatile uint32_t *)(_memoryAddress + lapic::REG_OFFSET_VERSION));
}

uint32_t LAPIC::task_priority() const {
    return *((volatile uint32_t *)(_memoryAddress + lapic::REG_OFFSET_TASK_PRIORITY));
}

void LAPIC::set_task_priority(uint32_t val) {
    *((volatile uint32_t *)(_memoryAddress + lapic::REG_OFFSET_TASK_PRIORITY)) = val;
}

uint32_t LAPIC::arbitration_priority() const {
    return *((volatile uint32_t *)(_memoryAddress + lapic::REG_OFFSET_ARBITRATION_PRIORITY));
}

uint32_t LAPIC::processor_priority() const {
    return *((volatile uint32_t *)(_memoryAddress + lapic::REG_OFFSET_PROCESSOR_PRIORITY));
}

void LAPIC::eoi(uint32_t val) {
    *((volatile uint32_t *)(_memoryAddress + lapic::REG_OFFSET_EOI)) = val;
}

uint32_t LAPIC::remote_read() const {
    return *((volatile uint32_t *)(_memoryAddress + lapic::REG_OFFSET_REMOTE_READ));
}

uint32_t LAPIC::logical_destination() const {
    return *((volatile uint32_t *)(_memoryAddress + lapic::REG_OFFSET_LOGICAL_DESTINATION));
}

void LAPIC::set_logical_destination(uint32_t val) {
    *((volatile uint32_t *)(_memoryAddress + lapic::REG_OFFSET_LOGICAL_DESTINATION)) = val;
}

uint32_t LAPIC::destination_format() const {
    return *((volatile uint32_t *)(_memoryAddress + lapic::REG_OFFSET_DESTINATION_FORMAT));
}

void LAPIC::set_destination_format(uint32_t val) {
    *((volatile uint32_t *)(_memoryAddress + lapic::REG_OFFSET_DESTINATION_FORMAT)) = val;
}

uint32_t LAPIC::spurious_interrupt_vector() const {
    return *((volatile uint32_t *)(_memoryAddress + lapic::REG_OFFSET_SPURIOUS_INT_VECTOR));
}

void LAPIC::set_spurious_interrupt_vector(uint32_t val) {
    *((volatile uint32_t *)(_memoryAddress + lapic::REG_OFFSET_SPURIOUS_INT_VECTOR)) = val;
}

uint32_t LAPIC::error_status() const {
    return *((volatile uint32_t *)(_memoryAddress + lapic::REG_OFFSET_ERROR_STATUS));
}

void LAPIC::interrupt_command(uint32_t* lower, uint32_t* upper) const {
    if(lower) {
        *lower = *((volatile uint32_t *)(_memoryAddress + lapic::REG_OFFSET_INTERRUPT_COMMAND));
    }

    if(upper) {
        *upper = *((volatile uint32_t *)(_memoryAddress + lapic::REG_OFFSET_INTERRUPT_COMMAND + 0x10));
    }
}

void LAPIC::set_interrupt_command(uint32_t lower, uint32_t upper) {
    *((volatile uint32_t *)(_memoryAddress + lapic::REG_OFFSET_INTERRUPT_COMMAND + 0x10)) = upper;
    *((volatile uint32_t *)(_memoryAddress + lapic::REG_OFFSET_INTERRUPT_COMMAND)) = lower;
}

uint32_t LAPIC::lvt_timer() const {
    return *((volatile uint32_t *)(_memoryAddress + lapic::REG_OFFSET_LVT_TIMER));
}

void LAPIC::set_lvt_timer(uint32_t val) {
    *((volatile uint32_t *)(_memoryAddress + lapic::REG_OFFSET_LVT_TIMER)) = val;
}

uint32_t LAPIC::lvt_thermal_sensor() const {
    return *((volatile uint32_t *)(_memoryAddress + lapic::REG_OFFSET_LVT_THERMAL_SENSOR));
}

void LAPIC::set_lvt_thermal_sensor(uint32_t val) {
    *((volatile uint32_t *)(_memoryAddress + lapic::REG_OFFSET_LVT_THERMAL_SENSOR)) = val;
}

uint32_t LAPIC::lvt_perf_monitor() const {
    return *((volatile uint32_t *)(_memoryAddress + lapic::REG_OFFSET_LVT_PERF_MONITOR));
}

void LAPIC::set_lvt_perf_monitor(uint32_t val) {
    *((volatile uint32_t *)(_memoryAddress + lapic::REG_OFFSET_LVT_PERF_MONITOR)) = val;
}

uint32_t LAPIC::lvt_lint0() const {
    return *((volatile uint32_t *)(_memoryAddress + lapic::REG_OFFSET_LVT_LINT0));
}

void LAPIC::set_lvt_lint0(uint32_t val) {
    *((volatile uint32_t *)(_memoryAddress + lapic::REG_OFFSET_LVT_LINT0)) = val;
}

uint32_t LAPIC::lvt_lint1() const {
    return *((volatile uint32_t *)(_memoryAddress + lapic::REG_OFFSET_LVT_LINT1));
}

void LAPIC::set_lvt_lint1(uint32_t val) {
    *((volatile uint32_t *)(_memoryAddress + lapic::REG_OFFSET_LVT_LINT1)) = val;
}

uint32_t LAPIC::lvt_error() const {
    return *((volatile uint32_t *)(_memoryAddress + lapic::REG_OFFSET_LVT_ERROR));
}

void LAPIC::set_lvt_error(uint32_t val) {
    *((volatile uint32_t *)(_memoryAddress + lapic::REG_OFFSET_LVT_ERROR)) = val;
}

uint32_t LAPIC::initial_count() const {
    return *((volatile uint32_t *)(_memoryAddress + lapic::REG_OFFSET_INITIAL_COUNT));
}

void LAPIC::set_initial_count(uint32_t val) {
    *((volatile uint32_t *)(_memoryAddress + lapic::REG_OFFSET_INITIAL_COUNT)) = val;
}

uint32_t LAPIC::current_count() const {
    return *((volatile uint32_t *)(_memoryAddress + lapic::REG_OFFSET_CURRENT_COUNT));
}

uint32_t LAPIC::divide_config() const {
    return *((volatile uint32_t *)(_memoryAddress + lapic::REG_OFFSET_DIVIDE_CONFIG));
}

void LAPIC::set_divide_config(uint32_t val) {
    *((volatile uint32_t *)(_memoryAddress + lapic::REG_OFFSET_DIVIDE_CONFIG)) = val;
}

template<typename T, typename = typename enable_if<is_integral<T>::value, T>::type>
static void replace_bits(T* val, T newVal, T offset, T mask) {
    T tmp = *val;
    tmp &= ~(mask << offset);
    tmp |= (newVal << offset);
    *val = tmp;
}

template<typename T, typename = typename enable_if<is_integral<T>::value, T>::type>
static void replace_bits(volatile T* val, T newVal, T offset, T mask) {
    T tmp = *val;
    tmp &= ~(mask << offset);
    tmp |= (newVal << offset);
    *val = tmp;
}

uint8_t IOAPICRedirectionEntry::destination_field() const {
    uint64_t tmp = (_val >> ioapic::IOREDTBL_DEST_FIELD_OFFSET);
    return destination_mode() == Physical
        ? tmp & ioapic::IOREDTBL_DEST_FIELD_PHYS_MASK
        : tmp & ioapic::IOREDTBL_DEST_FIELD_LOG_MASK;
}

IOAPICRedirectionEntry& IOAPICRedirectionEntry::set_destination_field(uint8_t val) {
    uint8_t mask = destination_mode() == Physical
        ? ioapic::IOREDTBL_DEST_FIELD_PHYS_MASK
        : ioapic::IOREDTBL_DEST_FIELD_LOG_MASK;

    replace_bits<uint64_t>(&_val, val, ioapic::IOREDTBL_DEST_FIELD_OFFSET, mask);
    return *this;
}

bool IOAPICRedirectionEntry::interrupt_masked() const {
    return _val & ioapic::IOREDTBL_INT_MASK_FLAG;
}

IOAPICRedirectionEntry& IOAPICRedirectionEntry::set_interrupt_masked(bool masked) {
    if(masked) {
        _val |= ioapic::IOREDTBL_INT_MASK_FLAG;
    } else {
        _val &= ~ioapic::IOREDTBL_INT_MASK_FLAG;
    }

    return *this;
}

IOAPICTriggerMode IOAPICRedirectionEntry::trigger_mode() const {
    return _val & ioapic::IOREDTBL_TRIG_MODE_FLAG ? Level : Edge;
}

IOAPICRedirectionEntry& IOAPICRedirectionEntry::set_trigger_mode(IOAPICTriggerMode mode) {
    if(mode == Level) {
        _val |= ioapic::IOREDTBL_TRIG_MODE_FLAG;
    } else {
        _val &= ~ioapic::IOREDTBL_TRIG_MODE_FLAG;
    }

    return *this;
}

bool IOAPICRedirectionEntry::remote_irr() const {
    return _val & ioapic::IOREDTBL_REMOTE_IRR_FLAG;
}

IOAPICPolarity IOAPICRedirectionEntry::interrupt_pin_polarity() const {
    return _val & ioapic::IOREDTBL_INTPOL_FLAG ? ActiveLow : ActiveHigh;
}

IOAPICRedirectionEntry& IOAPICRedirectionEntry::set_interrupt_pin_polarity(IOAPICPolarity polarity) {
    if(polarity == ActiveLow) {
        _val |= ioapic::IOREDTBL_INTPOL_FLAG;
    } else {
        _val &= ~ioapic::IOREDTBL_INTPOL_FLAG;
    }

    return *this;
}

bool IOAPICRedirectionEntry::is_delivery_pending() const {
    return _val & ioapic::IOREDTBL_DELIVER_STAT_FLAG;
}

IOAPICDestinationMode IOAPICRedirectionEntry::destination_mode() const {
    return _val & ioapic::IOREDTBL_DESTMOD_FLAG ? Logical : Physical;
}

IOAPICRedirectionEntry& IOAPICRedirectionEntry::set_destination_mode(IOAPICDestinationMode mode) {
    if(mode == Logical) {
        _val |= ioapic::IOREDTBL_DESTMOD_FLAG;
    } else {
        _val &= ~ioapic::IOREDTBL_DESTMOD_FLAG;
    }

    return *this;
}

IOAPICDeliveryMode IOAPICRedirectionEntry::delivery_mode() const {
    uint8_t raw = (_val >> ioapic::IOREDTBL_DELIVER_MODE_OFFSET) & ioapic::IOREDTBL_DELIVER_MODE_MASK;
    return (IOAPICDeliveryMode)raw;
}

IOAPICRedirectionEntry& IOAPICRedirectionEntry::set_delivery_mode(IOAPICDeliveryMode mode) {
    uint8_t raw = (uint8_t)mode;
    replace_bits<uint64_t>(&_val, raw, ioapic::IOREDTBL_DELIVER_MODE_OFFSET, ioapic::IOREDTBL_DELIVER_MODE_MASK);
    return *this;
}

uint8_t IOAPICRedirectionEntry::interrupt_vector() const {
    return _val & ioapic::IOREDTBL_INTVEC_MASK;
}

IOAPICRedirectionEntry& IOAPICRedirectionEntry::set_interrupt_vector(uint8_t vector) {
    replace_bits<uint64_t>(&_val, vector, 0, ioapic::IOREDTBL_INTVEC_MASK);
    return *this;
} 

uint8_t IOAPIC::selected_register() const {
    uint32_t raw = *((volatile uint32_t *)(_memoryAddress + ioapic::IOREGSEL_OFFSET));
    return raw & ioapic::IOREGSEL_REG_ADDR_MASK;
}

void IOAPIC::select_register(uint8_t reg) {
    *((volatile uint32_t *)(_memoryAddress + ioapic::IOREGSEL_OFFSET)) = (uint32_t)reg;
}

uint32_t IOAPIC::read_current() const {
    return *((volatile uint32_t *)(_memoryAddress + ioapic::IOWIN_OFFSET));
}

void IOAPIC::write_current(uint32_t val) {
    *((volatile uint32_t *)(_memoryAddress + ioapic::IOWIN_OFFSET)) = val;
}

uint8_t IOAPIC::ioapic_id() {
    select_register(ioapic::REG_OFFSET_IOAPICID);
    uint32_t raw = *((volatile uint32_t *)(_memoryAddress + ioapic::IOWIN_OFFSET));
    return (raw >> ioapic::IOAPICID_ID_OFFSET) & ioapic::IOAPICID_ID_MASK;
}

void IOAPIC::set_ioapid_id(uint8_t id) {
    select_register(ioapic::REG_OFFSET_IOAPICID);
    volatile uint32_t* addr = ((volatile uint32_t *)(_memoryAddress + ioapic::IOWIN_OFFSET));
    replace_bits<uint32_t>(addr, id, ioapic::IOAPICID_ID_OFFSET, ioapic::IOAPICID_ID_MASK);
}

uint8_t IOAPIC::max_redirection_entry() {
    select_register(ioapic::REG_OFFSET_IOAPICVER);
    uint32_t raw = *((volatile uint32_t *)(_memoryAddress + ioapic::IOWIN_OFFSET));
    return (raw >> ioapic::IOAPICVER_MAX_REDIR_OFFSET) & ioapic::IOAPICVER_MAX_REDIR_MASK;
}

uint8_t IOAPIC::apic_version() {
    select_register(ioapic::REG_OFFSET_IOAPICVER);
    uint32_t raw = *((volatile uint32_t *)(_memoryAddress + ioapic::IOWIN_OFFSET));
    return raw & ioapic::IOAPICVER_VERSION_MASK;
}

uint8_t IOAPIC::ioapic_arbitration_id() {
    select_register(ioapic::REG_OFFSET_IOAPICARB);
    uint32_t raw = *((volatile uint32_t *)(_memoryAddress + ioapic::IOWIN_OFFSET));
    return (raw >> ioapic::IOAPICARB_ID_OFFSET) & ioapic::IOAPICARB_ID_MASK;
}

void IOAPIC::set_ioapic_arbitration_id(uint8_t id) {
    select_register(ioapic::REG_OFFSET_IOAPICID);
    volatile uint32_t* addr = ((volatile uint32_t *)(_memoryAddress + ioapic::IOWIN_OFFSET));
    replace_bits<uint32_t>(addr, id, ioapic::IOAPICARB_ID_OFFSET, ioapic::IOAPICARB_ID_MASK);
}

IOAPICRedirectionEntry IOAPIC::redirection_entry(size_t index) {
    select_register(ioapic::REG_OFFSET_IOREBTBL_BASE + 2 * index + 1);
    uint64_t tmp = *((volatile uint32_t *)(_memoryAddress + ioapic::IOWIN_OFFSET));
    tmp <<= 32;
    select_register(ioapic::REG_OFFSET_IOREBTBL_BASE + 2 * index);
    tmp |= *((volatile uint32_t *)(_memoryAddress + ioapic::IOWIN_OFFSET));
    return IOAPICRedirectionEntry(tmp);
}

void IOAPIC::set_redirection_entry(const IOAPICRedirectionEntry& entry, size_t index) {
    uint64_t raw = (uint64_t)entry;
    if(!entry.interrupt_masked()) {
        // Since this requires two writes, it is easy to imagine a situation in which
        // a half written redirection entry gets used.  So for now, disable the entry.
        raw |= ioapic::IOREDTBL_INT_MASK_FLAG;
    }

    select_register(ioapic::REG_OFFSET_IOREBTBL_BASE + 2 * index);
    *((volatile uint32_t *)(_memoryAddress + ioapic::IOWIN_OFFSET)) = raw & 0xffffffff;

    select_register(ioapic::REG_OFFSET_IOREBTBL_BASE + 2 * index + 1);
    *((volatile uint32_t *)(_memoryAddress + ioapic::IOWIN_OFFSET)) = raw >> 32;

    if(!entry.interrupt_masked()) {
        // Now that the upper half is written, rewrite the lower half
        // with an unmasked value
        raw &= ~ioapic::IOREDTBL_INT_MASK_FLAG;
        select_register(ioapic::REG_OFFSET_IOREBTBL_BASE + 2 * index);
        *((volatile uint32_t *)(_memoryAddress + ioapic::IOWIN_OFFSET)) = raw & 0xffffffff;
    }
}

void apic_init(madt_t* m) {
    MADT madt(m);
    uint64_t lapic = madt.data()->lic_address;
    uint64_t ioapic = 0;
    int count = 0;
    
    int_source_override_t* interrupt_overrides[16] = { 
        nullptr, nullptr, nullptr, nullptr, 
        nullptr, nullptr, nullptr, nullptr, 
        nullptr, nullptr, nullptr, nullptr, 
        nullptr, nullptr, nullptr, nullptr
    };

    for(int i = 0; i < madt.count(); i++) {
        auto* entry = madt.get(i);
        if(entry->type == madt::TYPE_LOCAL_APIC_ADDR_OVERRIDE) {
            lapic = ((local_apic_addr_ovr_t *)entry)->local_apic_addr;
        } else if(entry->type == madt::TYPE_INTERRUPT_SRC_OVERRIDE) {
            auto* source = (int_source_override_t *)entry;
            interrupt_overrides[source->source] = source;
        } else if(entry->type == madt::TYPE_IO_APIC) {
            ioapic = ((io_apic_t *)entry)->apic_address;
        }
    }

    uint64_t lapicPage = lapic & ~0xfffULL;
    KernelPageTableManager()->MapMemory((void *)lapicPage, (void *)lapicPage, false);
    uint64_t ioapicPage = ioapic & ~0xfffULL;
    KernelPageTableManager()->MapMemory((void *)ioapicPage, (void *)ioapicPage, false);
    pic_override((void *)lapic);
    
    uint8_t vectors[16] = {
        0xEC, 0xE4, 0, 0x94,
        0x8C, 0x84, 0x7C, 0x74,
        0xD4, 0xCC, 0xC4, 0xBC,
        0xB4, 0xAC, 0xA4, 0x9C
    };

    uint16_t bmp;
    Bitmap bitmap(2, (void *)&bmp);
    bitmap.Clear();
    
    IOAPIC ioapicObj((void *)ioapic);
    for(int i = 0; i < 16; i++) {
        if(bitmap[i]) {
            continue;
        }

        IOAPICPolarity polarity = ActiveHigh;
        IOAPICTriggerMode trigger = Edge;
        uint8_t index = i;
        const auto* ovrd = interrupt_overrides[i];
        if(ovrd) {
            index = ovrd->global_interrupt;
            polarity = (ovrd->flags & 0x3) == 0x3 ? ActiveLow : ActiveHigh;
            trigger = (ovrd->flags & 0xC) == 0xC ? Level : Edge;
            bitmap.Set(ovrd->global_interrupt, true);
        }

        IOAPICRedirectionEntry e = ioapicObj.redirection_entry(index);
        e.set_interrupt_masked(false)
            .set_interrupt_vector(vectors[i])
            .set_delivery_mode(Fixed)
            .set_destination_mode(Physical)
            .set_interrupt_pin_polarity(polarity)
            .set_trigger_mode(trigger);
        ioapicObj.set_redirection_entry(e, index);
    }
}