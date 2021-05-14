#define AHCI_DEV_NULL 0
//#define AHCI_DEV_SATA 1
//#define AHCI_DEV_SATAPI 4
//#define AHCI_DEV_SEMB 2
//#define AHCI_DEV_PM 3
#define HBA_PORT_DET_PRESENT 3
#define HBA_PORT_IPM_ACTIVE 1
#define SATA_SIG_ATA    0x00000101      // SATA drive
#define SATA_SIG_ATAPI  0xEB140101      // SATAPI drive
#define SATA_SIG_SEMB   0xC33C0101      // Enclosure management bridge
#define SATA_SIG_PM     0x96690101      // Port multiplier
#define CMD_FIS_DEV_LBA (1U << 6)
#define MAX_CMD_SLOT_CNT 32
#define MAX_PORT_CNT     32
#define AHCI_BASE 0x400000  //ADDED
#include <stdio.h>
#include <sys/kprintf.h>
#include <unistd.h>
#include <sys/ahci.h>
#include <sys/ahcinew.h>
#include <sys/ahcicomm.h>

uint32_t inportl(uint16_t portid)
{
	uint32_t ret;
	__asm__ volatile("inl %%dx, %%eax":"=a"(ret):"d"(portid));
	return ret;
}

void outportl(uint16_t portid, uint32_t value)
{
	__asm__ volatile("outl %%eax, %%dx": :"d" (portid), "a" (value));
}

uint16_t pciConfigReadWord (uint8_t bus, uint8_t slot,
                             uint8_t func, uint8_t offset)
 {
    uint32_t address;
    uint32_t lbus  = (uint32_t)bus;
    uint32_t lslot = (uint32_t)slot;
    uint32_t lfunc = (uint32_t)func;
    uint16_t tmp = 0;
    address = (uint32_t)((lbus << 16) | (lslot << 11) |
              (lfunc << 8) | (offset & 0xfc) | ((uint32_t)0x80000000)); 
    outportl (0xCF8, address);
    tmp = (uint16_t)((inportl (0xCFC) >> ((offset & 2) * 8)) & 0xffff);
    return (tmp);
}


uint64_t abarAddress(unsigned short bus, unsigned short slot,unsigned short func, unsigned short offset)
{
        uint32_t address;
        address = (uint32_t)((bus << 16) | (slot << 11)|  (func << 8) | (offset & 0xfc) | ((uint32_t)0x80000000));
        outportl(0xCF8, address);
        outportl(0xCFC, 0x3EBF1000);
        uint32_t tmp = (uint32_t)(inportl (0xCFC));
        return tmp;
}



static int check_type(hba_port_t *port)
{
        uint32_t ssts = port->ssts;
 
        uint8_t ipm = (ssts >> 8) & 0x0F;
        uint8_t det = ssts & 0x0F;
 
        if (det != HBA_PORT_DET_PRESENT)        // Check drive status
                return AHCI_DEV_NULL;
        if (ipm != HBA_PORT_IPM_ACTIVE)
                return AHCI_DEV_NULL;
 
        switch (port->sig)
        {
        case SATA_SIG_ATAPI:
                return AHCI_DEV_SATAPI;
        case SATA_SIG_SEMB:
                return AHCI_DEV_SEMB;
        case SATA_SIG_PM:
                return AHCI_DEV_PM;
        default:
                return AHCI_DEV_SATA;
        }
}

void probe_port(unsigned short bus, unsigned short slot,unsigned short func, unsigned short offset)
{
	hba_mem_t *abar = (hba_mem_t *)abarAddress(bus,slot,func,offset);
        uint32_t pi = abar->pi;
	int i = 0;
	int sata_drive = -1;	

        while (i<32)
	{
		if (pi & 1)
		{
			int dt = check_type(&abar->ports[i]);
			if (dt == AHCI_DEV_SATA)
			{
				kprintf("SATA drive found at port %d\n", i);
				sata_drive =i;
			}
			else if (dt == AHCI_DEV_SATAPI)
			{
				kprintf("SATAPI drive found at port %d\n", i);
			}
			else if (dt == AHCI_DEV_SEMB)
			{
				kprintf("SEMB drive found at port %d\n", i);
			}
			else if (dt == AHCI_DEV_PM)
			{
				kprintf("PM drive found at port %d\n", i);
			}
			else
			{
				kprintf("No drive found at port %d\n", i);
			}
		}
 
		pi >>= 1;
		i ++;

	}
	uint32_t * buffer = (uint32_t *)0x300000;
        uint8_t *p = (uint8_t *)0x300000;
        int sectors =0;
        for(int point =0 ; point <= 99 ; point++)
	{
        p = (uint8_t *)0x300000;
        buffer = (uint32_t *)0x300000;
        for(int j =0;j<512;j++)
         {           
         
         *p = point;
         p = p+1;           
         
         }
         for(int n =0; n<8;n++)
         { 
         ahci_write(&abar->ports[sata_drive],(uint32_t)sectors,(uint32_t)0,(uint32_t) 1,(uint32_t *)buffer);
         sectors++;
         }
	
	}

	//kprintf(" write finished !!\n");

	uint32_t * readbuffer = (uint32_t *)0x600000 ;
        for(int point =0 ; point < 800 ; point++)
        {
{
        ahci_read(&abar->ports[sata_drive],(uint32_t)point,(uint32_t)0,(uint32_t) 1,(uint32_t *)readbuffer);
         //uint8_t *v = (uint8_t *)readbuffer;      
         //kprintf("%d", (int)*v);	
 
}     
        // kprintf(" read finished !!\n");
}

} 
uint16_t getVendorId(uint8_t bus,uint8_t slot,uint8_t func)
{
int vendorId,deviceId,subclassId,subclass,classCode;

vendorId = pciConfigReadWord(bus,slot,func,0);
if(vendorId != 0xFFFF) 
{

deviceId  = (int)pciConfigReadWord(bus,slot,func,2);
subclassId = (int)pciConfigReadWord(bus,slot,func,0x0A);
subclass = subclassId & 0xFF;
classCode = (subclassId >> 8) & 0xFF;
if((classCode ==1)&&(subclass ==6))
{
probe_port(bus,slot,func,0x24);
kprintf("Bus:%x\n",bus);
kprintf("Slot:%x\n",slot);
kprintf("Function:%x\n",func);
kprintf("Vendor ID: %x\n",vendorId);
kprintf("Device ID: %x\n",deviceId);
kprintf("subclass: %x\n",subclass);
kprintf("classcode: %x\n",classCode);
}
}

return vendorId;
}

void checkDevice(uint8_t bus, uint8_t device) 
{
     uint8_t function = 0;
     uint16_t vendorID =0;
     vendorID = getVendorId(bus, device, function);
     if(vendorID == 0xFFFF) return;  
}

void pci_probe(void) 
{
     uint8_t bus;
     uint8_t device;
 
     for(bus = 0; bus < 255; bus++) {
         for(device = 0; device < 32; device++) {
             checkDevice(bus, device);
         }
     }
 }












