#include <drivers/acpi/acpi.h>
#include <arch/x86-64/io/io.h>
#include <memory/memory.h>
#include <logs/logs.h>

namespace ACPI{
    void* FindTable(RSDP2* rsdp, char* signature){
        ACPI::SDTHeader* sdt = NULL;
        bool IsXSDT;
        uint64_t entries = 0;
        if(rsdp->XSDTAddress != NULL){
            sdt = (ACPI::SDTHeader*)vmm_GetVirtualAddress((void*)rsdp->XSDTAddress);
            entries = (sdt->Length - sizeof(ACPI::SDTHeader)) / sizeof(uint64_t);
            IsXSDT = true;
        }else if(rsdp->RSDTAddress != NULL){
            sdt = (ACPI::SDTHeader*)vmm_GetVirtualAddress((void*)rsdp->RSDTAddress);
            entries = (sdt->Length - sizeof(ACPI::SDTHeader)) / sizeof(uint32_t);
            IsXSDT = false;
        }else{
            return NULL;
        }
        

        ACPI::SDTHeader* Header = NULL;

        for(uint64_t i = 0; i < entries; i++){
            if(IsXSDT){
                Header = (ACPI::SDTHeader*)(uint64_t)((ACPI::XSDT*)sdt)->SDTPointer[i];
            }else{
                Header = (ACPI::SDTHeader*)(uint32_t)((ACPI::RSDT*)sdt)->SDTPointer[i];
            }
            Header = (ACPI::SDTHeader*)vmm_GetVirtualAddress((void*)Header);

            if(strcmp((char*)Header->Signature, signature, 4)){
                return Header;
            }
        }
        return 0;
    } 
}
