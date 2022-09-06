#pragma once

#define E1000_NUM_RX_DESC 32
#define E1000_NUM_TX_DESC 8

// Transmit Command
#define CMD_EOP                         (1 << 0)    // End of Packet
#define CMD_IFCS                        (1 << 1)    // Insert FCS
#define CMD_IC                          (1 << 2)    // Insert Checksum
#define CMD_RS                          (1 << 3)    // Report Status
#define CMD_RPS                         (1 << 4)    // Report Packet Sent
#define CMD_VLE                         (1 << 6)    // VLAN Packet Enable
#define CMD_IDE                         (1 << 7)    // Interrupt Delay Enable

#include <core/main.h>

struct e1000RxDescriptor {
    volatile uint64_t Addr;
    volatile uint16_t Length;
    volatile uint16_t CheckSum;
    volatile uint8_t Status;
    volatile uint8_t Errors;
    volatile uint16_t Special;
} __attribute__((packed));

struct e1000TxDescriptor {
    volatile uint64_t Addr;
    volatile uint16_t Length;
    volatile uint8_t Cso;
    volatile uint8_t Cmd;
    volatile uint8_t Status;
    volatile uint8_t Css;
    volatile uint16_t Special;
} __attribute__((packed));

class E1000Controller {
    private:
        uintptr_t memoryBase;
        uint32_t ioBase;
        uint16_t deviceID;
        uint8_t barType;

        bool eePromExists;
        void DetectEEProm();
        uint32_t EEPromRead(uint8_t addr);
        
        uint8_t MediaAccCtrl[6];
        bool CheckMAC();
        void InitMAC();

        /* TX and RX */
        uint16_t rxIndex, txIndex;
        uintptr_t ptrPhysical, ptrVirtual;

        void InitRX();
        void InitTX();

    public:
        E1000Controller(srv_pci_bar_info_t* BarInfo, srv_pci_device_info_t* DeviceInfo);

        void writeCmd(uint16_t addr, uint32_t value);
        uint32_t readCmd(uint16_t addr);

        /* Packet */
        void receivePacket();
        void sendPacket(const void* Data, uint16_t Length);

};