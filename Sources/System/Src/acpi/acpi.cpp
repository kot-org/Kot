#include <acpi/acpi.h>

RSDP2* rsdp = NULL;
SDTHeader* sdt = NULL;
SDTHeader** header = NULL;
uintptr_t* headerphysical = NULL;
uint64_t entries = 0;

uintptr_t ParseRSDP(uintptr_t rsdpPhysical){
    rsdp = (RSDP2*)MapPhysical(rsdpPhysical, sizeof(RSDP2));
    bool IsXSDT = (rsdp->Revision >= 1);
    if(IsXSDT){
        sdt = (SDTHeader*)MapPhysical((uintptr_t)rsdp->XSDTAddress, sizeof(SDTHeader));
        entries = (sdt->Length - sizeof(SDTHeader)) / sizeof(uint64_t);
        sdt = (SDTHeader*)MapPhysical((uintptr_t)rsdp->XSDTAddress, sizeof(uint64_t) * entries + sizeof(SDTHeader));
    }else{
        sdt = (SDTHeader*)MapPhysical((uintptr_t)rsdp->RSDTAddress, sizeof(SDTHeader));
        entries = (sdt->Length - sizeof(SDTHeader)) / sizeof(uint32_t);
        sdt = (SDTHeader*)MapPhysical((uintptr_t)rsdp->RSDTAddress, sizeof(uint32_t) * entries + sizeof(SDTHeader));
    }        

    header = (SDTHeader**)malloc(sizeof(SDTHeader*) * entries);
    headerphysical = (uintptr_t*)malloc(sizeof(uintptr_t) * entries);

    for(uint64_t i = 0; i < entries; i++){
        if(IsXSDT){
            header[i] = (SDTHeader*)(uint64_t)((XSDT*)sdt)->SDTPointer[i];
        }else{
            header[i] = (SDTHeader*)(uint32_t)((RSDT*)sdt)->SDTPointer[i];
        }
        headerphysical[i] = (uintptr_t)header[i];
        header[i] = (SDTHeader*)MapPhysical((uintptr_t)header[i], sizeof(SDTHeader));
    }
    return NULL;
} 

uintptr_t FindTable(char* signature){
    for(uint64_t i = 0; i < entries; i++){
        if(strncmp((char*)header[i]->Signature, signature, 4)){
            return headerphysical[i];
        }
    }
    return NULL;
}
