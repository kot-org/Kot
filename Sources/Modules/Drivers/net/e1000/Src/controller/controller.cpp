#include <controller/controller.h>

#include <utils/mmio.h>

#include <kot/arch.h>

#include <kot++/printf.h>

using namespace std;

e1000RxDescriptor* rxDescBuff[E1000_NUM_RX_DESC]; // Receive Descriptor Buffers
e1000TxDescriptor* txDescBuff[E1000_NUM_TX_DESC]; // Transmit Descriptor Buffers

void E1000Controller::writeCmd(uint16_t reg, uint32_t value) {
    if(barType == PCI_BAR_TYPE_IO) {
        IoWrite32(ioBase, reg);
        IoWrite32(ioBase + 4, value);
    } else {
        MMIOWrite32(memoryBase, reg, value);
    }
}

uint32_t E1000Controller::readCmd(uint16_t reg) {
    if(barType == PCI_BAR_TYPE_IO) {
        IoWrite32(ioBase, reg);
        return IoRead32(ioBase + 4);
    } else {
        return MMIORead32(memoryBase, reg);
    }
}

void E1000Controller::DetectEEProm() {
    uint32_t value = 0;

    writeCmd(REG_EEPROM, 0x1);

    for(int i = 0; i < 1000 && !eePromExists; i++) {
        value = readCmd(REG_EEPROM);
        
        if(value & 0x10)
            eePromExists = true;
        else
            eePromExists = false;
    }
}

uint32_t E1000Controller::EEPromRead(uint8_t addr) {
    uint32_t tmp = 0;

    if(eePromExists) {
        writeCmd(REG_EEPROM, (1) | (uint32_t) addr << 8);
        while(!((tmp = readCmd(REG_EEPROM)) & (1 << 4)));
    } else {
        writeCmd(REG_EEPROM, (1) | ((uint32_t) addr << 2));
        while(!((tmp = readCmd(REG_EEPROM)) & (1 << 1)));
    }

    return (tmp >> 16) & 0xFFFF;
}

bool E1000Controller::CheckMAC() {
    if(!eePromExists) {
        if(MMIORead32(memoryBase, REG_MAC) == 0){
            return false;
        }
    }
    return true;
}

void E1000Controller::InitMAC() {
    if(eePromExists) {
        uint32_t tmp;

        tmp = EEPromRead(0);
        MediaAccCtrl[0] = tmp & 0xFF;
        MediaAccCtrl[1] = tmp >> 8;

        tmp = EEPromRead(1);
        MediaAccCtrl[2] = tmp & 0xFF;
        MediaAccCtrl[3] = tmp >> 8;

        tmp = EEPromRead(2);
        MediaAccCtrl[4] = tmp & 0xFF;
        MediaAccCtrl[5] = tmp >> 8;
    } else {
        if(CheckMAC()) {            
            if(MMIORead32(memoryBase, REG_MAC) != 0) {
                for(int i = 0; i < 6; i++) {
                    MediaAccCtrl[i] = MMIORead8(memoryBase, REG_MAC + i); 
                }
            }
        }
    }
    Printlog("[NET/E1000] MAC initialized");
}

/* Packet */
void E1000Controller::InitRX() {
    uintptr_t ptr = GetPhysical(&ptrPhysical, sizeof(e1000RxDescriptor) * E1000_NUM_RX_DESC + 16);
    e1000RxDescriptor* descBuff = (e1000RxDescriptor*) ptr;
    
    for(int i = 0; i < E1000_NUM_RX_DESC; i++) {
        rxDescBuff[i] = (e1000RxDescriptor*)((uint8_t*) descBuff + i * 16);
        ptrVirtual = GetPhysical((uintptr_t*) rxDescBuff[i]->Addr, 8192 + 16);
    }

    writeCmd(REG_RXDESC_LOW, ((uint64_t) ptrPhysical));
    writeCmd(REG_RXDESC_HIGH, ((uint64_t) ptrPhysical >> 32));

    writeCmd(REG_RXDESC_LEN, E1000_NUM_RX_DESC * 16);

    writeCmd(REG_RXDESC_HEAD, 0);
    writeCmd(REG_RXDESC_TAIL, E1000_NUM_RX_DESC - 1);

    writeCmd(REG_RCTRL, 0x602801E);

    rxIndex = 0;

    Printlog("[NET/E1000] RX initialized");
}

void E1000Controller::InitTX() {
    uintptr_t ptr = GetPhysical(&ptrPhysical, sizeof(e1000TxDescriptor) * E1000_NUM_TX_DESC + 16);
    e1000TxDescriptor* descBuff = (e1000TxDescriptor*) ptr;

    for(int i = 0; i < E1000_NUM_TX_DESC; i++) {
        txDescBuff[i] = (e1000TxDescriptor*)((uint8_t*) descBuff + i * 16);
        txDescBuff[i]->Status = TSTA_DD;
    }

    writeCmd(REG_TXDESC_LOW, ((uint64_t) ptrPhysical & 0xFFFFFFFF));
    writeCmd(REG_TXDESC_HIGH, ((uint64_t) ptrPhysical >> 32));

    writeCmd(REG_TXDESC_LEN, E1000_NUM_TX_DESC * 16);

    if(deviceID == E1000_DEV)               // Qemu, Bochs, and VirtualBox emmulated NICs
        writeCmd(REG_TCTRL, 0x10400FA);
    else if(deviceID == E1000_I217 || deviceID == E1000_82577LM)    // Intel I217 and Intel 82577LM
        writeCmd(REG_TCTRL, 0x3003F0FA);

    writeCmd(REG_TIPG, 0x0060200A);

    txIndex = 0;

    Printlog("[NET/E1000] TX initialized");
}

void E1000Controller::receivePacket() {

}

void E1000Controller::sendPacket(const void* Data, uint16_t Length) {
    txDescBuff[txIndex]->Addr = (uint64_t) Data;
    txDescBuff[txIndex]->Length = Length;
    txDescBuff[txIndex]->Cmd = CMD_EOP | CMD_IFCS | CMD_RS;

    uint16_t oldTxIndex = txIndex;

    txIndex = (txIndex + 1) % E1000_NUM_TX_DESC;
    writeCmd(REG_TXDESC_TAIL, txIndex);

    while(!(txDescBuff[oldTxIndex]->Status & 0xFF))
    Printlog("Packet send");
}

/* Todo: E1000 Handler */

/* Init */
E1000Controller::E1000Controller(srv_pci_bar_info_t* BarInfo, srv_pci_device_info_t* DeviceInfo) {
    if(BarInfo->Type == PCI_BAR_TYPE_IO) {
        ioBase = (uint64_t) BarInfo->Address;
    } else {
        memoryBase = MapPhysical(BarInfo->Address, BarInfo->Size);
    }
    barType = BarInfo->Type;
    deviceID = DeviceInfo->deviceID;

    DetectEEProm();
    InitMAC();
    InitRX();
    InitTX();

    std::printf("[NET/E1000] DeviceID: %x EEProm: %d BarType: %d \n \t\t\t MAC0: %x MAC1: %x MAC2: %x MAC3: %x MAC4: %x MAC5: %x", deviceID, eePromExists, barType, MediaAccCtrl[0], MediaAccCtrl[1], MediaAccCtrl[2], MediaAccCtrl[3], MediaAccCtrl[4], MediaAccCtrl[5]);

    // sendPacket(malloc(0x1000), 0x1000);
}