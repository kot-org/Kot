#include <drivers/acpi/acpi.h>
#include <arch/x86-64/io/io.h>
#include <memory/memory.h>
#include <logs/logs.h>

namespace ACPI{
    void* FindTable(SDTHeader* sdtHeader, char* signature){
        int entries = (sdtHeader->Length - sizeof(ACPI::SDTHeader)) / 8;
        for(int t = 0; t < entries; t++){
            ACPI::SDTHeader* newSDTHeader = (ACPI::SDTHeader*)*(uint64_t*)((uint64_t)sdtHeader + sizeof(ACPI::SDTHeader) + (t * 8));
            newSDTHeader = (ACPI::SDTHeader*)globalPageTableManager[CPU::GetCoreID()].GetVirtualAddress(newSDTHeader);
            for(int i = 0; i < 4; i++){
                if(newSDTHeader->Signature[i] != signature[i]){
                    break;
                }  
                if(i == 3){
                    return newSDTHeader;   
                }          
            }
        }
        return 0;
    } 
}
