#pragma once
#include "../../main/kernelInit.h"

namespace HPET{
    void InitialiseHPET(ACPI::HPETHeader* hpet);
    void ChangeMainTimerInterruptState(bool IsEnable);
    void ChangeMainTimerLegacyMappingState(bool IsEnable);
    uint64_t GetTime();

    uint64_t HPETReadRegister(uint64_t offset);
    void HPETWriteRegister(uint64_t offset, uint64_t value);

    struct GeneralCapabilitiesAndIDRegisterStruct{
        uint8_t RevisionID;
        uint8_t NumberOfTimer:4;
        bool IsMainCounter64bit:1;
        uint8_t Reserved:1;
        bool IsLegacyReplacement:1;
        uint16_t VendorId;
        uint32_t CounterPeriod;
    }__attribute__((packed));

    struct TimerCapabilitiesAndConfigurationStruct{
        uint8_t Reserved0:1;
        uint8_t InterruptType:1;
        bool IsInterruptEnabled:1;
        bool EnablePeriodic:1;
        bool IsPeriodicSupported:1;
        bool Is64BitMode:1;
        bool IsSoftwareAccumulator:1; //This field is used to allow software to directly set periodic timer's accumulator
        uint8_t Reserved1:1;
        bool Enable32BitMode:1;
        uint8_t InterruptRoute:4;
        uint8_t EnableFSB:1;
        uint8_t IsFSBSupported:1;
        uint16_t Reserved2;
        uint32_t RouteIRQ;
    }__attribute__((packed));


    enum HPETTimerRegisterOffset{
        GeneralCapabilitiesAndIDRegister = 0x0,
        GeneralConfigurationRegister = 0x10,
        MainCounterValues = 0xF0,
    };

    enum GeneralCapabilitiesAndIDRegisterOffset{
        GeneralCapabilitiesAndIDRegisterCounterPeriod = 32,
    };

    enum GeneralConfigurationRegisterOffset{
        TimerInterrupt = 0,
        LegacyMapping = 1,
    };

    enum HPETTimerRegisterTimerOffset{
        HPETTimerRegisterTimerOffsetSpaceDataStart = 0x100,
        HPETTimerRegisterTimerOffsetSpaceSize = 0x20,
        HPETTimerRegisterTimerOffsetConfigurationCapabilityRegister = 0x0,
        HPETTimerRegisterTimerOffsetComparatorValueRegister = 0x8,
        HPETTimerRegisterTimerOffsetFSBInterruptRouteRegister = 0x10,
    };
}