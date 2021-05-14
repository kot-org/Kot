#include "pci.h"
#include "PciDeviceType.h"
#include "../graphics/BasicRenderer.h"
#include "KernelUtil.h"
#include "acpi/xsdt.h"
#include "acpi/mcfg.h"
#include "acpi/rsdt.h"
#include "drivers/modules.h"
#include "Panic.h"

#include <cstddef>

inline bool pci_device_exists(pci_header_t* h) {
    return h->device_id != 0xffff && h->device_id != 0;
}

static module_t* pci_find_module(uint8_t classcode, uint8_t subclass) {
    for(module_t* module = &_ModulesStart; module != &_ModulesEnd; module++) {
        if(module->module_type == MODULE_TYPE_PCI) {
            pci_device_module_t *pci_module = (pci_device_module_t*)module->module;
            if(classcode == pci_module->classcode && subclass == pci_module->subclass) {
                return module;
            }
        }
    }

    return nullptr;
}

static int pci_try_module(pci_header_t* h) {
    module_t* pci_module = pci_find_module(h->class_code, h->subclass);
    if(!pci_module) {
        return 0;
    }

    return ((pci_device_module_t*)pci_module->module)->device_handler(h);
}

static void pci_init_bus(void* base) {
    for(int i = 0; i < 0x100; i++) {
        KernelPageTableManager()->MapMemory((void *)((uint64_t)base + i * 0x1000), (void *)((uint64_t)base + i * 0x1000), false);
    }

    pci_print_bus(base, 0);

    uint64_t baseAddr = (uint64_t)base;
    for(int i = 0; i < 32; i++, baseAddr += 0x8000) {
        pci_header_t* h = (pci_header_t *)baseAddr;
        if(!pci_device_exists(h)) {
            continue;
        }

        pci_try_module(h);

        if(h->header_type & 0x80) {
            for(int f = 1; f < 8; f++) {
                pci_header_t* subh = (pci_header_t *)(baseAddr + f * 0x1000);
                if(!pci_device_exists(subh)) {
                    continue;
                }

                if(subh->class_code == 0x6 && subh->subclass == 0x4) {
                    pci_to_pci_bridge_t* b = (pci_to_pci_bridge_t *)subh;
                    pci_init_bus((void *)((uint64_t)base + b->secondary_bus * 0x100000));
                } else {
                    pci_try_module(subh);
                }
            }
        }
    }
}

static int pci_init() {
    const void *rsdpAddr = SystemRSDPAddress();
    if(!rsdpAddr) {
        return -1;
    }

    RSDP rsdpObj(rsdpAddr);
    XSDT xsdtObj((void *)rsdpObj.data()->xdst_address);
    RSDT rsdtObj((void *)(uint64_t)rsdpObj.data()->rsdt_address);

    void* mcfg;
    if(xsdtObj.is_valid()) {
        KernelPageTableManager()->MapMemory((void *)xsdtObj.data(), (void *)xsdtObj.data(), 1);
        mcfg = xsdtObj.get(MCFG::signature);
        KernelPageTableManager()->MapMemory(mcfg, mcfg, 1);
    } else {
        KernelPageTableManager()->MapMemory((void *)rsdtObj.data(), (void *)rsdtObj.data(), 1);
        KERNEL_ASSERT(rsdtObj.is_valid());
        mcfg = (void *)(uint64_t)rsdtObj.get(MCFG::signature);
        KernelPageTableManager()->MapMemory(mcfg, mcfg, 1);
    }

    MCFG mcfgObj(mcfg);
    for(int i = 0; i < mcfgObj.count(); i++) {
        mcfg_config_entry_t* entry = mcfgObj.get(i);
        if(entry->start_bus_no == entry->end_bus_no) {
            continue;
        }

        uint64_t baseAddr = entry->base_address;
        pci_init_bus((void *)baseAddr);
    }

    return 0;
}

void pci_print_bus(void *, uint8_t);

inline constexpr uint32_t do_hash(uint8_t cls, uint8_t subclass, uint8_t progif) {
    return ((uint32_t)cls << 16) | ((uint32_t)subclass << 8) | progif;
}

template<typename T>
static const char* binary_search(DeviceTypeEntry<T> list[], T lookup, size_t start,size_t end) {
    if(start == end) {
        return list[start].hash == lookup ? list[start].val : nullptr;
    }

    size_t size = start + (end - start) / 2;
    if(list[size].hash == lookup) {
        return list[size].val;
    }

    if(list[size].hash > lookup) {
        end = size - 1;
    } else {
        start = size + 1;
    }

    return binary_search(list, lookup, start, end);
}

const char* pci_get_classname(pci_header_t* h) {
    if(h->class_code == 0xFF) {
        return "Unassigned";
    }

    return binary_search(ClassList, h->class_code, 0, sizeof(ClassList) / sizeof(DeviceTypeEntry<uint8_t>) - 1);
}

const char* pci_get_subclassname(pci_header_t* h) {
    if(h->subclass == 0x80) {
        return "Other";
    }

    return binary_search(SubclassList, hash(h->class_code, h->subclass), 0, sizeof(SubclassList) / sizeof(DeviceTypeEntry<uint16_t>) - 1);
}

const char* pci_get_interface(pci_header_t* h) {
    return binary_search(InterfaceList, hash(h->class_code, h->subclass, h->prog_interface), 0, sizeof(InterfaceList) / sizeof(DeviceTypeEntry<uint32_t>) - 1);
}

bool pci_get_type(pci_header_t* h, const char** cls, const char ** subclass, const char** progif) {
    *cls = pci_get_classname(h);
    *subclass = pci_get_subclassname(h);
    *progif = pci_get_interface(h);

    return *cls != nullptr;
}

void pci_print_function(uint8_t* base, uint8_t* function, uint8_t functionNum) {
    pci_header_t* h = (pci_header_t *)function;
    GlobalRenderer->Printf("                Function %hhu -> Vendor ID: %x / Device ID: %x / Status: %hu / Type: \\\\",
        functionNum, h->vendor_id, h->device_id, h->status);
    const char* c, *s, *p;
    if(pci_get_type(h, &c, &s, &p)) {
        GlobalRenderer->Printf("%s", c);
        if(s) {
            GlobalRenderer->Printf("\\%s", s);
        }

        if(p) {
            GlobalRenderer->Printf("\\%s", p);
        }
    } else {
        GlobalRenderer->Printf("[TYPE CORRUPT!]");
    }
    
    if(h->class_code == 0x06 && h->subclass == 0x04) {
        pci_print_bus(base, ((pci_to_pci_bridge_t*)h)->secondary_bus);
    } else {
        pci_print_all_bar(h);
    }
}

void pci_print_device(uint8_t* base, uint8_t* device, uint8_t deviceNum) {
    pci_header_t* h = (pci_header_t *)device;
    if(!pci_device_exists(h)) {
        return;
    }

    GlobalRenderer->Printf("              Device %hhu", deviceNum);
    GlobalRenderer->Next();
    pci_print_function(base, device, 0);
    GlobalRenderer->Next();
    if(h->header_type & 0x80) {
        device += 0x1000;
        for(uint8_t i = 1; i < 8; i++, device += 0x1000) {
            h = (pci_header_t *)device;
            if(pci_device_exists(h)) {
                pci_print_function(base, device, i);
                GlobalRenderer->Next();
            }
        }
    }
}

void pci_print_bus(void* base_address, uint8_t num) {
    uint8_t* base = (uint8_t *)base_address;
    uint8_t* bus = base + (num * 0x100000);
    uint8_t* device = bus;

    GlobalRenderer->Printf("            Bus %hhu", num);
    GlobalRenderer->Next();
    for(uint8_t i = 0; i < 32; i++, device += 0x8000) {
        pci_print_device(base, device, i);
    }
}

pci_header_t* pci_get_device(void* cfgArea, uint8_t bus, uint8_t device, uint8_t function) {
    uint8_t* p = (uint8_t *)cfgArea;
    p += bus * 0x100000 + device * 0x8000 + function * 0x1000;
    pci_header_t* pci_header = (pci_header_t *)p;
    if(pci_header->device_id == INVALID_DEVICE) {
        return NULL;
    }

    return pci_header;
}

void pci_print_bar_at(pci_header_t* h, uint32_t* addresses, size_t count) {
    for(size_t i = 0; i < count; i++) {
        uint64_t r1 = addresses[i];
        if(r1 == 0) {
            continue;
        }

        uint64_t width = 0;
        uint32_t realAddr = addresses[i];
        GlobalRenderer->Next();
        if(r1 & 1) {
            bool ioActive = h->command & 0x02;
            if(ioActive) {
                h->command &= ~0x02;
            }

            GlobalRenderer->Printf("                  I/O BAR: 0x%X", (uint64_t)&addresses[i]);

            addresses[i] = 0xffffffff;
            uint32_t nextValue = addresses[i];
            addresses[i] = (uint32_t)r1;
            width = ~(nextValue & 0xFFFFFFFC) + 1;

            if(ioActive) {
                h->command |= 0x02;
            }
        } else {
            bool memActive = h->command & 0x01;
            if(memActive) {
                h->command &= ~0x01;
            }

            GlobalRenderer->Printf("                  RAM BAR: 0x%X", (uint64_t)&addresses[i]);
            if(r1 & 0x4) {
                i++;
                uint64_t r2 = addresses[i];
                addresses[i-1] = addresses[i] = 0xffffffff;
                uint64_t nextValue = addresses[i-1] + ((uint64_t)addresses[i] << 32);
                addresses[i-1] = r1;
                addresses[i] = r2;
                width = ~(nextValue & 0xFFFFFFFFFFFFFFF0) + 1;
            } else {
                addresses[i] = 0xffffffff;
                uint32_t nextValue = addresses[i];
                addresses[i] = (uint32_t)r1;
                width = ~(nextValue & 0xFFFFFFF0) + 1;
            }

            if(memActive) {
                h->command |= 0x01;
            }
        }

        const char* suffix;
        uint64_t smallestWidth = Smallest(width, &suffix);
        GlobalRenderer->Printf(" (%llu%s used by device)", smallestWidth, suffix);
    }
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Waddress-of-packed-member"

void pci_print_all_bar(pci_header_t* h)
{
    uint8_t type = h->header_type & 0x7f;
    if(type == 0) {
        pci_device_t* d = (pci_device_t *)h;
        pci_print_bar_at(h, d->bar, 6);
    } else if(type == 1) {
        pci_to_pci_bridge_t* b = (pci_to_pci_bridge_t *)h;
        pci_print_bar_at(h, b->bar, 2);
    }
}

#pragma GCC diagnostic pop

bool is_match(pci_header_t* header, int cls, int subclass, int prog_if) {
    if(header->class_code != (uint8_t)cls) {
        return false;
    }

    if(subclass != -1 && subclass != header->subclass) {
        return false;
    }

    if(prog_if != -1 && prog_if != header->prog_interface) {
        return false;
    }

    return true;
}

void* pci_find_type_bus(void* base_address, uint8_t num, int cls, int subclass, int prog_if);

void* pci_find_type_device(void* base, uint8_t* device, uint8_t num, int cls, int subclass, int prog_if) {
    pci_header_t* h = (pci_header_t *)device;
    if(!pci_device_exists(h)) {
        return nullptr;
    }

    if(is_match(h, cls, subclass, prog_if)) {
        return h;
    }

    if(h->header_type & 0x80) {
        device += 0x1000;
        for(uint8_t i = 1; i < 8; i++, device += 0x1000) {
            h = (pci_header_t *)device;
            if(is_match(h, cls, subclass, prog_if)) {
                return h;
            }

            if(h->class_code == 0x06 && h->subclass == 0x04) {
                void* result = pci_find_type_bus(base, ((pci_to_pci_bridge_t*)h)->secondary_bus, cls, subclass, prog_if);
                if(result) {
                    return result;
                }
            }
        }
    }

    return nullptr;
}

void* pci_find_type_bus(void* base_address, uint8_t num, int cls, int subclass, int prog_if) {
    uint8_t* base = (uint8_t *)base_address;
    uint8_t* bus = base + (num * 0x100000);
    uint8_t* device = bus;

    for(uint8_t i = 0; i < 32; i++, device += 0x8000) {
        void* result = pci_find_type_device(base, device, i, cls, subclass, prog_if);
        if(result) {
            return result;
        }
    }

    return nullptr;
}

void* pci_find_type(void* cfgArea, int cls, int subclass, int prog_if) {
    return pci_find_type_bus(cfgArea, 0, cls, subclass, prog_if);
}

MODULE_INIT(PCI_module, pci_init);