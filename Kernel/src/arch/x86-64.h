#pragma once

#include <arch/x86-64/asm.h>
#include <arch/x86-64/io/io.h>
#include <arch/x86-64/smp/smp.h>
#include <arch/x86-64/cpu/cpu.h>
#include <arch/x86-64/gdt/gdt.h>
#include <arch/x86-64/tss/tss.h>
#include <arch/x86-64/vmm/vmm.h>
#include <arch/x86-64/apic/apic.h>
#include <arch/x86-64/simd/simd.h>
#include <arch/x86-64/io/msr/msr.h>
#include <arch/x86-64/interrupts/idt.h>
#include <arch/x86-64/io/serial/serial.h>
#include <arch/x86-64/interrupts/interrupts.h>
#include <arch/x86-64/userspace/userspace.h>