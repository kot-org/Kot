#include <drivers/hpet/hpet.h>

namespace HPET{
    uintptr_t HPETAddress;
    Timer timer;

    void InitialiseHPET(ACPI::HPETHeader* hpet){
        HPETAddress = (uintptr_t)vmm_Map((uintptr_t)hpet->Address.Address);

        timer.Counter = (uint64_t*)((uint64_t)HPETAddress + MainCounterValues);
        timer.Frequency = HPETReadRegister(GeneralCapabilitiesAndIDRegister) >> GeneralCapabilitiesAndIDRegisterCounterPeriod;
        
        ChangeMainTimerInterruptState(false);
        HPETWriteRegister(MainCounterValues, 0);
        ChangeMainTimerInterruptState(true);
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
        return HPETReadRegister(MainCounterValues);
    }

    uint64_t HPETReadRegister(uint64_t offset){
	    return *((volatile uint64_t*)((uintptr_t)((uint64_t)HPETAddress + offset)));
    }

    void HPETWriteRegister(uint64_t offset, uint64_t value){
        *((volatile uint64_t*)((uintptr_t)((uint64_t)HPETAddress + offset))) = value;
    }

    void HPETSleep(uint64_t ms){
        uint64_t end = HPETReadRegister(MainCounterValues) + (ms * FEMOSECOND_IN_MILLISECOND) / timer.Frequency;
        while(HPETReadRegister(MainCounterValues) <= end){
            __asm__ __volatile__ ("pause" : : : "memory");
        }
    }
}