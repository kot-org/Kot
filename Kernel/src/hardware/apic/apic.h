#pragma once
#include <stdint.h>
#include <stddef.h>
#include "../../lib/stdio.h"
#include "../acpi/acpi.h"
#include "../../IO/msr/msr.h"
#include "../../IO/IO.h"
#include "../../graphics.h"
#include "../../scheduling/pit/pit.h"
#include "../../paging/pageTableManager.h"

namespace APIC{
    #define MAX_PROCESSORS   256
    #define IO_APIC     256


    struct EntryRecord{
        uint8_t Type;
        uint8_t Length;
    } __attribute__((packed));

    //Entry Type 0 : Processor Local APIC
    struct LocalProcessor{
        EntryRecord Record;
        uint8_t ProcessorID;
        uint8_t APICID;
        uint32_t Flags; //bit 0 = Processor Enabled / bit 1 = Online Capable
    } __attribute__((packed));

    //Entry Type 1 : I/O APIC
    struct IOAPIC{
        EntryRecord Record;
        uint8_t APICID;
        uint8_t Reserved;
        uint32_t APICAddress;
        uint32_t GlobalSystemInterruptBase;
    } __attribute__((packed));

    //Entry Type 2 : Interrupt Source Override
    struct InterruptSourceOverride{
        EntryRecord Record;
        uint8_t BusSource;
        uint8_t IRQSource;
        uint32_t GlobalSystemInterrupt;
        uint16_t Flags;
    } __attribute__((packed));

    //Entry Type 4 : Non-maskable interrupts
    struct NonMaskableinterrupts{
        EntryRecord Record;
        uint8_t ACPIProcessorID; //0xFF means all processors
        uint16_t Flags;
        uint8_t LINT; //0 or 1
    } __attribute__((packed));

    //Entry Type 5 : Local APIC Address Override
    struct LocalAPICAddressOverride{
        EntryRecord Record;
        uint8_t Reserved;
        uint64_t LocalAPICAddress;
    } __attribute__((packed));

    enum EntryType{
        EntryTypeLocalProcessor = 0,
        EntryTypeIOAPIC = 1,
        EntryTypeInterruptSourceOverride = 2,
        EntryTypeNonmaskableinterrupts = 4,
        EntryTypeLocalAPICAddressOverride = 5,
    };

    enum LocalAPICInterrupt{
        LocalAPICInterruptVector			= 0,
        LocalAPICInterruptMessageType		= 8,
        LocalAPICInterruptDeliveryStatus	= 12,
        LocalAPICInterruptTrigerMode		= 15,
        LocalAPICInterruptMask				= 16,
        LocalAPICInterruptTimerMode		    = 17,
    };
    enum LocalAPICInterruptRegisterMessageType{
        LocalAPICInterruptRegisterMessageTypeFixed		= 0b000,
        LocalAPICInterruptRegisterMessageTypeSMI		= 0b010,
        LocalAPICInterruptRegisterMessageTypeNMI		= 0b100,
        LocalAPICInterruptRegisterMessageTypeExtint		= 0b111,
    };
    enum LocalAPICInterruptRegisterDeliveryStatus{
        LocalAPICInterruptRegisterMessageTypedle	    = 0,
        LocalAPICInterruptRegisterMessageTypePending	= 1
    };
    enum LocalAPICInterruptRegisterRemoteIRR{
        LocalAPICInterruptRegisterRemoteIRRCompleted	= 0,
        LocalAPICInterruptRegisterRemoteIRRAccepted	    = 1,
    };
    enum LocalAPICInterruptRegisterTriggerMode{
        LocalAPICInterruptRegisterTriggerModeEdge		= 0,
        LocalAPICInterruptRegisterTriggerModeLevel	    = 1,
    };
    enum LocalAPICInterruptRegisterMask{
        LocalAPICInterruptRegisterMaskEnable            = 0, 
        LocalAPICInterruptRegisterMaskDisable           = 1,
    };
    enum LocalAPICInterruptTimerMode{
        LocalAPICInterruptTimerModeOneShot	= 0,
        LocalAPICInterruptTimerModePeriodic	= 1
    };

    struct LocalAPICInterruptRegister{
        uint8_t	vector:8;
        enum LocalAPICInterruptRegisterMessageType messageType:3;
        enum LocalAPICInterruptRegisterDeliveryStatus deliveryStatus:1;
        enum LocalAPICInterruptRegisterRemoteIRR remoteIrr:1;
        enum LocalAPICInterruptRegisterTriggerMode triggerMode:1;
        enum LocalAPICInterruptRegisterMask	mask:1;
        enum LocalAPICInterruptTimerMode timerMode:1;
    };

    enum LocalAPICRegisterOffset {
        LocalAPICRegisterOffsetID					    = 0x020,
        LocalAPICRegisterOffsetVersion				    = 0x030,
        LocalAPICRegisterOffsetTaskPriority			    = 0x080,
        LocalAPICRegisterOffsetArbitrationPriority	    = 0x090,
        LocalAPICRegisterOffsetProcessorPriority	    = 0x0a0,
        LocalAPICRegisterOffsetEOI					    = 0x0b0,
        LocalAPICRegisterOffsetRemoteRead			    = 0x0c0,
        LocalAPICRegisterOffsetLogicalDestination	    = 0x0d0,
        LocalAPICRegisterOffsetDestinationFormat	    = 0x0e0,
        LocalAPICRegisterOffsetSpuriouseIntVector	    = 0x0f0,
        LocalAPICRegisterOffsetInService		        = 0x100,
        LocalAPICRegisterOffsetTriggerMode		        = 0x180,
        LocalAPICRegisterOffsetInterruptdRequest		= 0x200,
        LocalAPICRegisterOffsetErrorStatus		        = 0x280,
        LocalAPICRegisterOffsetCMCI					    = 0x2f0,
        LocalAPICRegisterOffsetInterruptCommand		    = 0x300,
        LocalAPICRegisterOffsetLVTTimer				    = 0x320,
        LocalAPICRegisterOffsetLVTThermalSensor	        = 0x330,
        LocalAPICRegisterOffsetLVTPerfommanceMonitor	= 0x340,
        LocalAPICRegisterOffsetLVTLINT0				    = 0x350,
        LocalAPICRegisterOffsetLVTLINT1				    = 0x360,
        LocalAPICRegisterOffsetLVTERROR				    = 0x370,
        LocalAPICRegisterOffsetInitialCount			    = 0x380,
        LocalAPICRegisterOffsetCurentCount			    = 0x390,
        LocalAPICRegisterOffsetDivide			        = 0x3e0,
    };

    void InitializeMADT(ACPI::MADTHeader* madt);
    void* GetLAPICAddress();
    void EnableAPIC();
    void StartLapicTimer();
    uint32_t localAPICReadRegister(size_t offset);
    uint32_t localAPICReadRegister(void* lapicAddress, size_t offset);
    void localAPICWriteRegister(size_t offset, uint32_t value);    
    void localAPICWriteRegister(void* lapicAddress, size_t offset, uint32_t value);    
    void localAPICSetTimerCount(uint32_t value);
    void localApicEOI();
    uint32_t CreatRegisterValueInterrupts(LocalAPICInterruptRegister reg);

    extern LocalProcessor* Processor[MAX_PROCESSORS];
    extern size_t ProcessorCount;
}