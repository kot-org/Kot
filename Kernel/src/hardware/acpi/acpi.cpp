#include "acpi.h"
#include "../../logs/logs.h"

namespace ACPI{
    void* FindTable(SDTHeader* sdtHeader, char* signature){
        int entries = (sdtHeader->Length - sizeof(ACPI::SDTHeader)) / 8;
        for(int t = 0; t < entries; t++){
            ACPI::SDTHeader* newSDTHeader = (ACPI::SDTHeader*)*(uint64_t*)((uint64_t)sdtHeader + sizeof(ACPI::SDTHeader) + (t * 8));
            newSDTHeader = (ACPI::SDTHeader*)globalPageTableManager.GetVirtualAddress(newSDTHeader);
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

    /* FADT */
    FADTHeader* FadtHeader;
    SLPInfo* SLP;
    bool CanShutdown = false;

    void InitializeFADT(FADTHeader* fadt){
        FadtHeader = fadt;
        IoWrite8(fadt->SMI_CommandPort, fadt->AcpiEnable);

        /*while(IoRead16(fadt->PM1aControlBlock) & 1 == 0);

        char* S5Addr = (char*) fadt->Dsdt + 36;
        int dsdtLength = fadt->Dsdt + 1 - 36;

        while(dsdtLength > 0) {
            if(memcmp(S5Addr, "_S5_", 4) == 0){
                break;
            }
                
            dsdtLength--;
            S5Addr++;
        }
        
        if(dsdtLength > 0) {
            S5Addr += 5;
            S5Addr += ((*S5Addr & 0xC0) >> 6) +2;
            if(*S5Addr == 0x0A){
                S5Addr++;
            }
                
            SLP->TYPa = *(S5Addr) << 10;
            S5Addr++;
            if(*S5Addr == 0x0A){
                S5Addr++;
            }
                
            SLP->TYPb = *(S5Addr) << 10;
            SLP->EN = 1 << 13;
            CanShutdown = true;
        }*/
    }

    void Restart(){
        //IO
        IoWrite16(FadtHeader->ResetReg.Address, FadtHeader->ResetValue);
        //nmio
        *((volatile uint8_t *)((uintptr_t)FadtHeader->ResetReg.Address)) = FadtHeader->ResetValue;   
    }

    void Shutdown(){
        if(CanShutdown){
            IoWrite16(FadtHeader->PM1aControlBlock, SLP->TYPa | SLP->EN);
            IoWrite16(FadtHeader->PM1bControlBlock, SLP->TYPb | SLP->EN); 
        }
    }
}
