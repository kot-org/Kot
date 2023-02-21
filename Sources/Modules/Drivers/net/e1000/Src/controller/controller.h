#pragma once

#include <kot/sys.h>
#include <kot/bits.h>

#include <kot/uisd/srvs/pci.h>

extern process_t Proc;

#define E1000_DEV      0x100E  // Device ID for the e1000 Qemu, Bochs, and VirtualBox emmulated NICs
#define E1000_I217     0x153A  // Device ID for Intel I217
#define E1000_82577LM  0x10EA  // Device ID for Intel 82577LM

typedef struct {
    uint16_t DeviceID;
    uint8_t NumRXDesc, NumTXDesc;
} ChipInfo_t;

// control
#define REG_CTRL                0x0000
#define CTRL_RESET_MASK         (1 << 26)
#define CTRL_ASDE_MASK          (1 << 5) // Auto-speed detection enable
#define CTRL_SLU_MASK           (1 << 6) // Set link up

// status
#define REG_STATUS              0x0008
#define STATUS_SPEED_MASK       (1 << 6) 


// packet
#define PACKET_SIZE     2048

// Receive
typedef struct {
    uint64_t BufferAddress;
    uint16_t Length;
    uint16_t Checksum; // for 82544GC/EI -> Reserved
    uint8_t Status;
    uint8_t Errors;
    uint16_t Special; // for 82544GC/EI -> Reserved
} RXDescriptor;

// Receive base address
#define RCV_BA_LOW    0x5400 
#define RCV_BA_HIGH   0x5404
#define RBAH_AV_MASK     (1 << 31) // bit "Address valid" sur le registre RAH (Receive address high)

// Receive descriptor (RCVD)
#define RCVD_ADDR_LOW       0x2800
#define RCVD_ADDR_HIGH      0x2804
#define RCVD_LENGTH         0x2808
#define RCVD_HEAD           0x2810
#define RCVD_TAIL           0x2818

// Receive control
#define REG_RCV_CTRL        0x0100
#define RCV_EN_MASK         (1 << 1)
#define RCV_BAM_MASK        (1 << 15)
#define RCV_BSIZE_MASK      (1 << 16)

// Transmit
typedef struct {
    uint64_t BufferAddress;
    uint16_t Length;
    uint8_t Cso; // Checksum Offset 
    uint8_t Cmd; // (bit 0 = EOP, bit 1 = IFCS)
    uint8_t Status; // (bit 0 = DD, bit 1 = EC, bit 2 = LC, bit 3 = TU)
    uint8_t Css;
    uint16_t Special;
} TXDescriptor;

// Transmit descriptor (TSTD)
#define TSTD_ADDR_LOW       0x3800
#define TSTD_ADDR_HIGH      0x3804
#define TSTD_LENGTH         0x3808
#define TSTD_HEAD           0x3810
#define TSTD_TAIL           0x3818

// Transmit control
#define REG_TST_CTRL    0x0400
#define TST_EN_MASK     (1 << 1) // Transmit enable
#define TST_PSP_MASK    (1 << 3) // Pad Short Packets

// Status
#define TX_STATUS_DD    (1 << 0) // Descriptor done
#define TX_STATUS_EC    (1 << 1) // Excess Collisions
#define TX_STATUS_LC    (1 << 2) // Late Collision
// bit 3 (Transmit Underrun) for 82544GC/EI ?? 

// Command
#define TX_CMD_EOP      (1 << 0) // End Of Packet
#define TX_CMD_IFCS     (1 << 1) // Insert FCS (Frame Check Sequence) -> Enables the receiver to verify whether the packet was corrupted during transmission
#define TX_CMD_IC       (1 << 2) // Insert Checksum
#define TX_CMD_RS       (1 << 3) // Report Status
// bit 4 (Report Packet Sent) for 82544GC/EI ?? 
#define TX_CMD_DEXT     (1 << 5) // Extension
#define TX_CMD_VLE      (1 << 6) // VLAN Packet Enable
#define TX_CMD_IDE      (1 << 7) // Interrupt Delay Enable


class E1000 {
    private:
        uint8_t BarType;

        uintptr_t MemoryBase;
        uint64_t IoBase;

        uint8_t MACAddr[5];

        ChipInfo_t* ChipInfo;

        void WriteRegister(uint16_t Reg, uint32_t Value);
        uint32_t ReadRegister(uint16_t Reg);

        // packet
        void InitTX();
        TXDescriptor* TXDesc;
        uint8_t* TXPacketBuffer;
        void InitRX();
        RXDescriptor* RXDesc;
        uint8_t* RXPacketBuffer;

    public:
        E1000(srv_pci_device_info_t* DeviceInfo, srv_pci_bar_info_t* BarInfo);
        ~E1000();

        void SendPacket(uint8_t* Data, uint32_t Size);
        void ReceivePacket();

        uint16_t GetSpeed();
};

ChipInfo_t* GetChipInfo(uint16_t DeviceID);