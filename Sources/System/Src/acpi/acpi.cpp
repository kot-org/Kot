#include <acpi/acpi.h>

RSDP2* rsdp = NULL;
kot_srv_system_sdtheader_t* sdt = NULL;
kot_srv_system_sdtheader_t** headers = NULL;
uintptr_t* headersphysical = NULL;
uint64_t entries = 0;

uintptr_t ParseRSDP(uintptr_t rsdpPhysical){
    rsdp = (RSDP2*)kot_MapPhysical(rsdpPhysical, sizeof(RSDP2));
    bool IsXSDT = (rsdp->Revision >= 1);
    if(IsXSDT){
        sdt = (kot_srv_system_sdtheader_t*)kot_MapPhysical((uintptr_t)rsdp->XSDTAddress, sizeof(kot_srv_system_sdtheader_t));
        entries = (sdt->Length - sizeof(kot_srv_system_sdtheader_t)) / sizeof(uint64_t);
        sdt = (kot_srv_system_sdtheader_t*)kot_MapPhysical((uintptr_t)rsdp->XSDTAddress, sizeof(uint64_t) * entries + sizeof(kot_srv_system_sdtheader_t));
    }else{
        sdt = (kot_srv_system_sdtheader_t*)kot_MapPhysical((uintptr_t)rsdp->RSDTAddress, sizeof(kot_srv_system_sdtheader_t));
        entries = (sdt->Length - sizeof(kot_srv_system_sdtheader_t)) / sizeof(uint32_t);
        sdt = (kot_srv_system_sdtheader_t*)kot_MapPhysical((uintptr_t)rsdp->RSDTAddress, sizeof(uint32_t) * entries + sizeof(kot_srv_system_sdtheader_t));
    }        

    headers = (kot_srv_system_sdtheader_t**)malloc(sizeof(kot_srv_system_sdtheader_t*) * entries);
    headersphysical = (uintptr_t*)malloc(sizeof(uintptr_t) * entries);

    for(uint64_t i = 0; i < entries; i++){
        if(IsXSDT){
            headers[i] = (kot_srv_system_sdtheader_t*)(uint64_t)((XSDT*)sdt)->SDTPointer[i];
        }else{
            headers[i] = (kot_srv_system_sdtheader_t*)(uint32_t)((RSDT*)sdt)->SDTPointer[i];
        }
        headersphysical[i] = (uintptr_t)headers[i];
        headers[i] = (kot_srv_system_sdtheader_t*)kot_MapPhysical((uintptr_t)headers[i], sizeof(kot_srv_system_sdtheader_t));
    }
    return NULL;
} 

uint64_t FindTableIndex(char* signature){
    for(uint64_t i = 0; i < entries; i++){
        if(strncmp((char*)headers[i]->Signature, signature, 4)){
            return i;
        }
    }
    return NULL;
}

uintptr_t GetTablePhysicalAddress(uint64_t index){
    if(index >= entries){
        return NULL;
    } 
    return headersphysical[index];
}

size64_t GetTableSize(uint64_t index){
    if(index >= entries){
        return NULL;
    } 
    return headers[index]->Length;
}
