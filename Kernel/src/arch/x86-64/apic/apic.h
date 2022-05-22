#pragma once
#include <arch/arch.h>
#include <kot/types.h>
#include <lib/stdio.h>
#include <drivers/acpi/acpi.h>

namespace APIC{
    #define LOCAL_APIC_ENABLE 0x800
    #define LOCAL_APIC_SPURIOUS_ALL 0x100
    #define LOCAL_APIC_SPURIOUS_ENABLE_APIC 0xff

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
        uint8_t	MaxInterrupts;
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
        LocalAPICInterruptRegisterMessageTypeIddle	    = 0,
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

    enum LocalAPICDestinationMode{
        LocalAPICDestinationModePhysicalDestination = 0,
        LocalAPICDestinationModeVirtualDestination = 1
    };

    enum DestinationType{
        LocalAPICDestinationTypeBase = 0,
        LocalAPICDestinationTypeSendInterruptSelf = 1,
        LocalAPICDestinationTypeSendAllProcessor = 2,
        LocalAPICDestinationTypeSendAllButNotSelf = 3
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

    enum LocalAPICInterruptipi{
        LocalAPICInterruptipiVector             = 0,
        LocalAPICInterruptipiMessageType        = 8,
        LocalAPICInterruptipiDestinationMode    = 11,
        LocalAPICInterruptipiDestinationType    = 18
    };

    enum LocalAPICDeliveryMode{
        LocalAPICDeliveryModeFixed		= 0b000,
        LocalAPICDeliveryModeSMI		= 0b010,
        LocalAPICDeliveryModeNMI		= 0b100,
        LocalAPICDeliveryModeINIT		= 0b101,
        LocalAPICDeliveryModeStartUp	= 0b110,
        LocalAPICDeliveryModeReserved	= 0b111,
    };

    struct LocalAPICIipi{
        uint8_t	vector:8;
        enum LocalAPICDeliveryMode deliveryMode:3;
        enum LocalAPICDestinationMode destinationMode:1;
        enum DestinationType destinationType:2;
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
        LocalAPICRegisterOffsetSpuriousIntVector	    = 0x0f0,
        LocalAPICRegisterOffsetInService		        = 0x100,
        LocalAPICRegisterOffsetTriggerMode		        = 0x180,
        LocalAPICRegisterOffsetInterruptdRequest		= 0x200,
        LocalAPICRegisterOffsetErrorStatus		        = 0x280,
        LocalAPICRegisterOffsetCMCI					    = 0x2f0,
        LocalAPICRegisterOffsetInterruptCommandLow		= 0x300,
        LocalAPICRegisterOffsetInterruptCommandHigh		= 0x310,
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

    enum IOAPICRegisterOffset{
        IOAPICId				= 0x00,
        IOAPICVersion			= 0x01,
        IOAPICArbitration		= 0x02,
        IOAPICRedirectionTable	= 0x10,
    };

    enum IOAPICRedirectionBitsLow {
        IOAPICRedirectionBitsLowVector				= 0,
        IOAPICRedirectionBitsLowDeliveryMode		= 8,
        IOAPICRedirectionBitsLowDestinationMode	    = 11,
        IOIOAPICRedirectionBitsLowDeliveryStatus	= 12,
        IOAPICRedirectionBitsLowPonPolarity		    = 13,
        IOAPICRedirectionBitsLowRemoteIrr			= 14,
        IOAPICRedirectionBitsLowTriggerMode		    = 15,
        IOAPICRedirectionBitsLowMask				= 16,
    };

    enum IOAPICRedirectionBitsHigh {
        IOAPICRedirectionBitsHighDestination	    = 24
    };

    enum IOAPICRedirectionEntryDeliveryMode {
        IOAPICRedirectionEntryDeliveryModeFixed			    = 0b000,
        IOAPICRedirectionEntryDeliveryModeLowPriority		= 0b001,
        IOAPICRedirectionEntryDeliveryModeSMI				= 0b010,
        IOAPICRedirectionEntryDeliveryModeNMI				= 0b100,
        IOAPICRedirectionEntryDeliveryModeINIT				= 0b101,
        IOAPICRedirectionEntryDeliveryModeEXTINT			= 0b111,
    };
    enum IOAPICRedirectionEntryDestinationMode {
        IOAPICRedirectionEntryDestinationModePhysicall	= 0,
        IOAPICRedirectionEntryDestinationModeLogical	= 1
    };
    enum IOAPICRedirectionEntryDeliveryStatus {
        IOAPICRedirectionEntryDeliveryStatusIddle		= 0,
        IOAPICRedirectionEntryDeliveryStatusPending	    = 1
    };
    enum IOAPICRedirectionEntryPinPolarity {
        IOAPICRedirectionEntryPinPolarityActiveHigh	    = 0,
        IOAPICRedirectionEntryPinPolarityActiveLow		= 1
    };
    enum IOAPICRedirectionEntryRemoteIRR {
        IOAPICRedirectionEntryRemoteIRRNone		= 0,
        IOAPICRedirectionEntryRemoteIRRInflight	= 1,
    };
    enum IOAPICRedirectionEntryTriggerMode {
        IOAPICRedirectionEntryTriggerModeEdge	= 0,
        IOAPICRedirectionEntryTriggerModeLevel	= 1
    };
    enum IOAPICRedirectionEntryMask {
        IOAPICRedirectionEntryMaskEnable	= 0,	// Masks the interrupt through
        IOAPICRedirectionEntryMaskDisable	= 1		// Masks the so it doesn't go through
    };

    struct IOAPICRedirectionEntry {
        uint8_t										vector:8;
        enum IOAPICRedirectionEntryDeliveryMode		delivery_mode:3;
        enum IOAPICRedirectionEntryDestinationMode	destination_mode:1;
        enum IOAPICRedirectionEntryDeliveryStatus	delivery_status:1;
        enum IOAPICRedirectionEntryPinPolarity		pin_polarity:1;
        enum IOAPICRedirectionEntryRemoteIRR		remote_irr:1;
        enum IOAPICRedirectionEntryTriggerMode		trigger_mode:1;
        enum IOAPICRedirectionEntryMask				mask:1;
        uint8_t										destination:8;
    };

    struct LapicAddress{
        void* PhysicalAddress;
        void* VirtualAddress;
    } __attribute__((packed));

    void InitializeMADT(ACPI::MADTHeader* madt);
    void IoAPICInit(uint8_t IOApicID);
    void IoChangeIrqState(uint8_t irq, uint8_t IOApicID, bool IsEnable);
    void LoadCores();
    void* GetLAPICAddress();
    void EnableAPIC(uint8_t CoreID);
    void StartLapicTimer();
    void localAPICSetTimerCount(uint32_t value);
    uint32_t localAPICGetTimerCount();
    void lapicSendInitIPI(uint8_t CoreID);
    void lapicSendStartupIPI(uint8_t CoreID, void* entry);
    void localApicEOI(uint8_t CoreID);
    void localApicEnableSpuriousInterrupts();
    uint32_t localAPICReadRegister(size_t offset);
    uint32_t localAPICReadRegister(void* lapicAddress, size_t offset);
    uint32_t ioapicReadRegister(void* apicPtr , uint8_t offset);
    void ioapicWriteRegister(void* apicPtr , uint8_t offset, uint32_t value);
    void localAPICWriteRegister(size_t offset, uint32_t value);    
    void localAPICWriteRegister(void* lapicAddress, size_t offset, uint32_t value);    
    uint32_t CreatRegisterValueInterrupts(LocalAPICInterruptRegister reg);
    uint32_t CreatLocalAPICIipiRegister(LocalAPICIipi reg);
    void IoApicSetRedirectionEntry(void* apicPtr, size_t index, IOAPICRedirectionEntry entry);
    void SetCommandIPI(uint32_t commandLow, uint32_t commandHigh);
    void GenerateInterruption(uint64_t CoreID, uint8_t Vector);
    
    extern LocalProcessor** Processor;
    extern uint8_t ProcessorCount;
}