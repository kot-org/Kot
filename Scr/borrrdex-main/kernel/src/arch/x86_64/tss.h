#pragma once

#include <stdint.h>

typedef struct tss {
    uint32_t prev_tss;
    uint64_t rsp[3];
    uint64_t reserved0;
    uint64_t ist[7];
    uint64_t reserved1;
    uint16_t reserved2;
    uint16_t io_map;
} __attribute__((packed)) tss_t;

void tss_init();
void tss_install(int num_cpu, uint64_t cpu_stack);

void tss_setstack(int num_cpu, uint64_t stack);

extern "C" tss_t* tss_get(int num_cpu);