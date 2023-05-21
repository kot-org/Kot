#pragma once

#include <kot/sys.h>
#include <string.h>
#include <kot++/printf.h>
#include <kot/uisd/srvs/time.h>
#include <kot/uisd/srvs/system.h>

#define SPACE_MEMORY    0x0
#define SPACE_IO        0x1

#define REGISTER_SIZE   0x400

#define FEMOSECOND_IN_MILLISECOND     1000000000000 // femosecond 10E15 to millisecond 10E3

struct GenericAddressStructure_t{
    uint8_t AddressSpace;       // 0 - system memory, 1 - system I/O
    uint8_t BitWidth;
    uint8_t BitOffset;
    uint8_t AccessSize;
    uint64_t Address;
}__attribute__((packed));

struct HPETHeader_t{
    srv_system_sdtheader_t Header;

    uint8_t RevisionID;
    uint8_t ComparatorCount:5;
    uint8_t CounterSize:1;
    uint8_t Reserved:1;
    uint8_t LegacyReplacement:1;
    uint16_t VendorID;     
        
    GenericAddressStructure_t Address;

    uint8_t HPETNumber;
    uint16_t MinimumTick;
    uint8_t PageProtection;
}__attribute__((packed));

struct GeneralCapabilitiesAndIDRegisterStruct_t{
    uint8_t RevisionID;
    uint8_t NumberOfTimer:4;
    bool IsMainCounter64bit:1;
    uint8_t Reserved:1;
    bool IsLegacyReplacement:1;
    uint16_t VendorId;
    uint32_t TickPeriod;
}__attribute__((packed));

struct TimerCapabilitiesAndConfigurationStruct_t{
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


enum Registers{
    RegisterGeneralCapabilitiesAndIDRegister = 0x0,
    RegisterGeneralConfigurationRegister = 0x10,
    RegisterMainCounterValues = 0xF0,
};

enum GeneralCapabilitiesAndIDRegisterOffset{
    GeneralCapabilitiesAndIDRegisterCounterPeriod = 32,
};

enum GeneralConfigurationRegisterOffset{
    TimerInterrupt = 0,
    LegacyMapping = 1,
};

enum RegisterTimerOffset{
    RegisterTimerOffsetConfigurationCapabilityRegister = 0x0,
    RegisterTimerOffsetComparatorValueRegister = 0x8,
    RegisterTimerOffsetFSBInterruptRouteRegister = 0x10,
    RegisterTimerOffsetSpaceSize = 0x20,
    RegisterTimerOffsetSpaceDataStart = 0x100,
};

struct HPET_t{
    HPETHeader_t* Header;
    uintptr_t RegistersAddress;
    uint64_t MainCounterAddress;

    uint64_t TickPeriod; // TickPeriod in femosecond

    uint64_t ReadRegister(uint64_t offset);
    void WriteRegister(uint64_t offset, uint64_t value);
    void ChangeMainTimerInterruptState(bool IsEnable);
    void ChangeMainTimerLegacyMappingState(bool IsEnable);
    uint64_t GetMainCounterTick();
    void Sleep(uint64_t duration);
};