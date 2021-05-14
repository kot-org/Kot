#include "init.h"
#include "KernelUtil.h"
#include "acpi/rsdp.h"
#include "acpi/xsdt.h"
#include "acpi/apic.h"
#include "arch/x86_64/cpuid.h"
#include "string.h"
#include "drivers/x86_64/pit.h"
#include "arch/x86_64/gdt/gdt.h"
#include "arch/x86_64/interrupt/idt.h"
#include "KernelUtil.h"

static int count_cpus(uint8_t lapic_buffer[], uint64_t& ioapic, uint64_t& lapic) {
    RSDP rsdp(SystemRSDPAddress());
    if(!rsdp.is_valid()) {
        return -1;
    }

    XSDT xsdt((const void *)rsdp.data()->xdst_address);
    if(!xsdt.is_valid()) {
        return -2;
    }

    MADT madt(xsdt.get(MADT::signature));
    if(!madt.is_valid()) {
        return -3;
    }

    lapic = madt.data()->lic_address;
    int count = 0;
    for(int i = 0; i < madt.count(); i++) {
        auto* entry = madt.get(i);
        if(entry->type == madt::TYPE_PROCESSOR_LOCAL_APIC) {
            lapic_buffer[count++] = ((proc_local_apic_t *)entry)->apic_id;
        } else if(entry->type == madt::TYPE_LOCAL_APIC_ADDR_OVERRIDE) {
            lapic = ((local_apic_addr_ovr_t *)entry)->local_apic_addr;
        } else if(entry->type == madt::TYPE_IO_APIC) {
            ioapic = ((io_apic_t *)entry)->apic_address;
        }
    }

    return count;
}

extern "C" void ap_startup() {
    while(true) {
        asm volatile("hlt");
    }
}

extern "C" void ap_trampoline();

volatile uint8_t aprunning = 0;
uint8_t bspdone = 0;
uint64_t stack_top = 0x70000000;
uint64_t pml4_address, idt_addr, gdt_addr;

int bsp_init() {
    idt_addr = (uint64_t)idt_address();
    gdt_addr = (uint64_t)gdt_address();
    pml4_address = (uint64_t)KernelPageTableManager()->PML4Address();
    uint8_t buffer[64];
    uint64_t ioapic, lapic;
    int proc_count = count_cpus(buffer, ioapic, lapic);
    if(proc_count <= 0) {
        return proc_count - 1;
    }

    uint64_t lapicPage = lapic & ~0xfffULL;
    KernelPageTableManager()->MapMemory((void *)lapicPage, (void *)lapicPage, false);
    
    uint64_t rax = 1, rbx, rcx, rdx;
    _cpuid(&rax, &rbx, &rcx, &rdx);
    uint8_t bspid = (rbx >> 24) & 0xFF;

    LAPIC lapicObj((void *)lapic);
    uint32_t siv = lapicObj.spurious_interrupt_vector();

    memcpy((void *)0x8000, (void *)&ap_trampoline, 4096);

    for(int i = 0; i < proc_count; i++) {
        if(buffer[i] == bspid) {
            continue;
        }

        uint64_t stack_mapping = stack_top - 2 * (i * 32 * 1024);
        for(int i = 0; i < 8; i++) {
            KernelPageTableManager()->MapMemory((void *)(stack_mapping), (void*)(stack_mapping), false);
            stack_mapping += 4096;
        }

        
        uint8_t start_running = aprunning;
        uint32_t cmdLower, cmdUpper;
        lapicObj.interrupt_command(&cmdLower, &cmdUpper);
        cmdUpper = (cmdUpper & 0x00ffffff) | (buffer[i] << 24);
        cmdLower = (cmdLower & 0xfff32000) | 0x00c500;
        lapicObj.set_interrupt_command(cmdLower, cmdUpper);

        do {
            asm volatile("pause" : : : "memory");
            lapicObj.interrupt_command(&cmdLower, nullptr);
        } while(cmdLower & (1 << 12));

        lapicObj.interrupt_command(&cmdLower, &cmdUpper);
        cmdUpper = (cmdUpper & 0x00ffffff) | (buffer[i] << 24);
        cmdLower = (cmdLower & 0xfff32000) | 0x008500;
        lapicObj.set_interrupt_command(cmdLower, cmdUpper);

        do {
            asm volatile("pause" : : : "memory");
            lapicObj.interrupt_command(&cmdLower, nullptr);
        } while(cmdLower & (1 << 12));

        pit_sleepms(10);

        for(int j = 0; j < 2; j++) {
            lapicObj.interrupt_command(&cmdLower, &cmdUpper);
            cmdUpper = (cmdUpper & 0x00ffffff) | (buffer[i] << 24);
            cmdLower = (cmdLower & 0xfff3f800) | 0x000608;
            lapicObj.set_interrupt_command(cmdLower, cmdUpper);
            pit_sleepms(1);
            do {
                asm volatile("pause" : : : "memory");
                lapicObj.interrupt_command(&cmdLower, nullptr);
            } while(cmdLower & (1 << 12));
        }
    }

    bspdone = 1;
    return 0;
}