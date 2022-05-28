#include <drivers/acpi/acpi.h>
#include <arch/x86-64/io/io.h>
#include <memory/memory.h>
#include <logs/logs.h>

namespace ACPI{
    uintptr_t FindTable(RSDP2* rsdp, char* signature){
        ACPI::SDTHeader* sdt = NULL;
        bool IsXSDT = (rsdp->Revision >= 1);
        uint64_t entries = 0;
        if(IsXSDT){
            sdt = (ACPI::SDTHeader*)vmm_GetVirtualAddress((uintptr_t)rsdp->XSDTAddress);
            entries = (sdt->Length - sizeof(ACPI::SDTHeader)) / sizeof(uint64_t);
        }else{
            sdt = (ACPI::SDTHeader*)vmm_GetVirtualAddress((uintptr_t)rsdp->RSDTAddress);
            entries = (sdt->Length - sizeof(ACPI::SDTHeader)) / sizeof(uint32_t);
        }
        

        ACPI::SDTHeader* Header = NULL;

        for(uint64_t i = 0; i < entries; i++){
            if(IsXSDT){
                Header = (ACPI::SDTHeader*)(uint64_t)((ACPI::XSDT*)sdt)->SDTPointer[i];
            }else{
                Header = (ACPI::SDTHeader*)(uint32_t)((ACPI::RSDT*)sdt)->SDTPointer[i];
            }
            Header = (ACPI::SDTHeader*)vmm_GetVirtualAddress((uintptr_t)Header);

            if(strcmp((char*)Header->Signature, signature, 4)){
                return Header;
            }
        }
        return 0;
    } 
}
