#include "hpet.h"

namespace HPET{
    void* HPETAddress;
    uint64_t HPETClock;
    void InitialiseHPET(ACPI::HPETHeader* hpet){
        if(hpet == NULL) return;
        HPETAddress = globalPageTableManager.MapMemory((void*)hpet->Address.Address, 1);

        HPETClock = HPETReadRegister(GeneralCapabilitiesAndIDRegister) >> GeneralCapabilitiesAndIDRegisterCounterPeriod;
        
        ChangeMainTimerInterruptState(false);
        HPETWriteRegister(MainCounterValues, 0);
        ChangeMainTimerInterruptState(true);
        ChangeMainTimerLegacyMappingState(true);
    }   


    void ChangeMainTimerInterruptState(bool IsEnable){
        uint64_t GeneralConfigurationRegisterData = HPETReadRegister(GeneralConfigurationRegister);
        GeneralConfigurationRegisterData |= IsEnable << TimerInterrupt; 
        HPETWriteRegister(GeneralConfigurationRegister, GeneralConfigurationRegisterData);
    } 

    void ChangeMainTimerLegacyMappingState(bool IsEnable){
        uint64_t GeneralConfigurationRegisterData = HPETReadRegister(GeneralConfigurationRegister);
        GeneralConfigurationRegisterData |= IsEnable << LegacyMapping; 
        HPETWriteRegister(GeneralConfigurationRegister, GeneralConfigurationRegisterData);
    }

    uint64_t GetTime(){
        return ((float)HPETReadRegister(MainCounterValues) / HPETClock) * 100;
    }

    uint64_t HPETReadRegister(uint64_t offset){
	    return *((volatile uint64_t*)((void*)((uint64_t)HPETAddress + offset)));
    }

    void HPETWriteRegister(uint64_t offset, uint64_t value){
        *((volatile uint64_t*)((void*)((uint64_t)HPETAddress + offset))) = value;
    }

    void HPETSleep(uint64_t ms){

    }
}