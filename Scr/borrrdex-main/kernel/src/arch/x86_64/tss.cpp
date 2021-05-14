#include "tss.h"
#include "config.h"
#include "string.h"
#include "gdt/gdt.h"

extern "C" void __load_tss(uint16_t tss);

static tss_t s_tss_descriptors[CONFIG_MAX_CPUS];

void tss_init() {
    memset(s_tss_descriptors, 0, sizeof(tss_t) * CONFIG_MAX_CPUS);
}

void tss_install(int num_cpu, uint64_t stack) {
    uint64_t tss_base = (uint64_t)&s_tss_descriptors[num_cpu];
    memset((void *)tss_base, 0, sizeof(tss_t));

    gdt_install_tss(tss_base, sizeof(tss_t));

    s_tss_descriptors[num_cpu].rsp[0] = stack;
    s_tss_descriptors[num_cpu].io_map = 0xFFFF;

    __load_tss((uint16_t)((6 + num_cpu) * sizeof(gdt_desc_t)));
}

void tss_setstack(int num_cpu, uint64_t stack) {
    s_tss_descriptors[num_cpu].rsp[0] = stack;
}

extern "C" tss_t* tss_get(int num_cpu) {
    return &s_tss_descriptors[num_cpu];
}