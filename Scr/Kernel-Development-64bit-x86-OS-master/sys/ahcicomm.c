//
//  ahcicomm.c
//  AHCI
//

#include <sys/ahci.h>
#include <sys/kprintf.h>
#include <sys/string.h>
#define PAGE_TRANS_READ_WRITE 0x2ULL
#define PAGE_TRANS_USER_SUPERVISOR 0x4ULL

#define ATA_CMD_READ_DMA 0xC8
#define ATA_CMD_READ_DMA_EX 0x25
#define ATA_CMD_WRITE_DMA_EX 0x35
#define ATA_DEV_BUSY 0x80
#define ATA_DEV_DRQ 0x08

#define HBA_PORT_CMD_CR 0x8000
#define HBA_PORT_IS_TFES 0x40000000

#define	KERNBASE	0xffffffff80000000

#define LOuint8_t(w) ((uint8_t)(w))
#define HIuint8_t(w) ((uint8_t)(((uint16_t)(w) >> 8) & 0xFF))



/*void* memset(void *str,int val,uint64_t size){
    unsigned char* ptr = str;
    while(size--)
        *ptr++ = (unsigned char) val;
    return str;
}*/

// Find a free command list slot
int find_cmdslot(hba_port_t *port)
{
    // If not set in SACT and CI, the slot is free
    uint32_t slots = (port->sact | port->ci);
    
    int cmdslots = 32;
    for (int i=0; i<cmdslots; i++)
    {
        if ((slots&1) == 0)
            return i;
        slots >>= 1;
    }
    kprintf("No free slots");    
    return -1;
}



int ahci_read(hba_port_t *port, uint32_t startl, uint32_t starth, uint32_t count, uint64_t *buf)
{
    port->is_rwc = (uint32_t) -1;		// Clear pending interrupt bits
    int spin = 0; // Spin lock timeout counter
    int slot = find_cmdslot(port);
    if (slot == -1)
        return 0;
    
   // uint64_t addr = 0;
   // addr = (((addr | port->clbu) << 32) | port->clb);
    hba_cmd_header_t *cmdheader = (hba_cmd_header_t*)port->clb;

    cmdheader += slot;
    cmdheader->cfl = sizeof(fis_reg_h2d_t)/sizeof(uint32_t);	// Command FIS size
    cmdheader->w = 0;		// Read from device
    cmdheader->c = 1;
    cmdheader->prdtl = (uint16_t)((count-1)>>4) + 1;	// PRDT entries count
    hba_cmd_tbl_t *cmdtbl = (hba_cmd_tbl_t*)(cmdheader->ctba);
    
    
    memset(cmdtbl, 0, sizeof(hba_cmd_tbl_t) +
           (cmdheader->prdtl-1)*sizeof(hba_prdt_entry_t));
    int i=0;
    for (i=0; i<cmdheader->prdtl-1; i++)
    {
        cmdtbl->prdt_entry[i].dba = (uint64_t)buf;
        cmdtbl->prdt_entry[i].dbc = 8*1024;	// 8K uint8_ts
        cmdtbl->prdt_entry[i].i = 1;
        buf += 4*1024;	// 4K uint16_ts
        count -= 16;	// 16 sectors
    }
    // Last entry
    cmdtbl->prdt_entry[i].dba = (uint64_t)buf;
    cmdtbl->prdt_entry[i].dbc = count<<9;	// 512 uint8_ts per sector
    cmdtbl->prdt_entry[i].i = 1;
    
    // Setup command
    fis_reg_h2d_t *cmdfis = (fis_reg_h2d_t *)(&cmdtbl->cfis);
    
    cmdfis->fis_type = FIS_TYPE_REG_H2D;
    cmdfis->c = 1;	// Command
    cmdfis->command = ATA_CMD_READ_DMA_EX;
    
    cmdfis->lba0 = (uint8_t)startl;
    cmdfis->lba1 = (uint8_t)(startl>>8);
    cmdfis->lba2 = (uint8_t)(startl>>16);
    cmdfis->device = 1<<6;	// LBA mode
    
    cmdfis->lba3 = (uint8_t)(startl>>24);
    cmdfis->lba4 = (uint8_t)starth;
    cmdfis->lba5 = (uint8_t)(starth>>8);
    
    cmdfis->countl = LOuint8_t(count);
    cmdfis->counth = HIuint8_t(count);
    
    // The below loop waits until the port is no longer busy before issuing a new command
    while ((port->tfd & (ATA_DEV_BUSY | ATA_DEV_DRQ)) && spin < 1000000)
    {
        spin++;
    }
    if (spin == 1000000)
    {
      //  trace_ahci("Port is hung\n");
        return 0;
    }
    
    port->ci = 1<<slot;	// Issue command
    
    // Wait for completion
    while (1)
    {
        // In some longer duration reads, it may be helpful to spin on the DPS bit
        // in the PxIS port field as well (1 << 5)
        if ((port->ci & (1<<slot)) == 0)
            break;
        if (port->is_rwc & HBA_PxIS_TFES)	// Task file error
        {
            //trace_ahci("Read disk error\n");
            return 0;
        }
    }
    
    // Check again
    if (port->is_rwc & HBA_PxIS_TFES)
    {
        //trace_ahci("Read disk error\n");
        return 0;
    }

    //kprintf(" Value read from buffer :%d", *buf);	    
    return 1;
}




int ahci_write(hba_port_t *port, uint32_t startl, uint32_t starth, uint32_t count, uint64_t* buffer ){
    //kprintf("Port number %d", port->is_rwc);
    port->is_rwc = (uint32_t)-1; // clearing pending interrupt bits
    int spin = 0;   // Spin lock timeout counter
    int slot = find_cmdslot(port);
    if (slot == -1)
        return 0;
     hba_cmd_header_t *cmdheader = (hba_cmd_header_t*)port->clb;
    

    
    cmdheader += slot;
    cmdheader->cfl = sizeof(fis_reg_h2d_t)/sizeof(uint32_t);	// Command FIS size
    cmdheader->w = 1;    // write to device
    //cmdheader->c = 1;
    cmdheader->prdtl = (uint16_t)((count-1)>>4) + 1;	// PRDT entries count
    
    hba_cmd_tbl_t *cmdtbl = (hba_cmd_tbl_t*)(cmdheader->ctba);	
   
    
    memset(cmdtbl, 0, sizeof(hba_cmd_tbl_t) + (cmdheader->prdtl-1)*sizeof(hba_prdt_entry_t));
    
    // 8K uint8_ts (16 sectors) per PRDT
    int i;
    for (i=0; i<cmdheader->prdtl-1; i++)
    {
        
	cmdtbl->prdt_entry[i].dba = (uint64_t)buffer;
        cmdtbl->prdt_entry[i].dbc = 8*1024;// -1;	// 8K uint8_ts
        cmdtbl->prdt_entry[i].i = 1;
        buffer += 4*1024;	// 4K uint16_ts
        count -= 16;	// 16 sectors
    }
    // Last entry
    cmdtbl->prdt_entry[i].dba = (uint64_t)buffer;
    cmdtbl->prdt_entry[i].dbc =  ((count<<9) );//-1);	// 512 iuint8_ts per sector
    cmdtbl->prdt_entry[i].i = 0;
    

    //kprintf("dba address %d",cmdtbl->prdt_entry[i].dba);
    // Setup command
    fis_reg_h2d_t *cmdfis = (fis_reg_h2d_t*)(&cmdtbl->cfis);
    
    cmdfis->fis_type = FIS_TYPE_REG_H2D;
    cmdfis->c = 1;	// Command
    cmdfis->command = ATA_CMD_WRITE_DMA_EX;
    
    cmdfis->lba0 = (uint8_t)startl;
    cmdfis->lba1 = (uint8_t)(startl>>8);
    cmdfis->lba2 = (uint8_t)(startl>>16);
    cmdfis->device = 1<<6;	// LBA mode
    
    cmdfis->lba3 = (uint8_t)(startl>>24);
    cmdfis->lba4 = (uint8_t)starth;
    cmdfis->lba5 = (uint8_t)(starth>>8);
    
    cmdfis->countl = LOuint8_t(count);
    cmdfis->counth = HIuint8_t(count);
    
    
    // The below loop waits until the port is no longer busy before issuing a new command
    while ((port->tfd & (ATA_DEV_BUSY | ATA_DEV_DRQ)) && spin < 1000000)
    {
        spin++;
    }
    if (spin == 1000000)
    {
        kprintf("Port is hung\n");
        return 0;
    }
    
    port->ci = 1<<slot;	// Issue command
    
    // Wait for completion
    while (1)
    {
        // In some longer duration reads, it may be helpful to spin on the DPS bit
        // in the PxIS port field as well (1 << 5)
        if ((port->ci & (1<<slot)) == 0)
            break;
        if (port->is_rwc & HBA_PORT_IS_TFES )	// Task file error
        {
            kprintf("Write disk error\n");
            return 0;
        }
        
    }
    
    // Check again
    if (port->is_rwc & HBA_PORT_IS_TFES )
    {
        kprintf("Write disk error\n");
        return 0;
    }
    
//    kprintf("Written to disk\n");
    return 1;
    
}





