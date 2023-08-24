#include <port.h>
#include <errno.h>
#include <stdbool.h>
#include <impl/vmm.h>
#include <lib/math.h>
#include <global/pmm.h>

static int sata_stop_cmd(ahci_sata_device_t* device){
    device->port->command_status &= ~HBA_PxCMD_ST;
    device->port->command_status &= ~HBA_PxCMD_FRE;

    while(true){
        if(device->port->command_status & HBA_PxCMD_FR) continue;
        if(device->port->command_status & HBA_PxCMD_CR) continue;

        break;
    }

    return 0;
}

static int sata_start_cmd(ahci_sata_device_t* device){
    while(device->port->command_status & HBA_PxCMD_CR);

    device->port->command_status |= HBA_PxCMD_FRE;
    device->port->command_status |= HBA_PxCMD_ST;

    return 0;
}

static int sata_find_slot(ahci_sata_device_t* device){
    uint32_t slot = device->port->sata_active;
    for(uint8_t i = 0; i < HBA_COMMAND_LIST_MAX_ENTRIES; i++){
        if((slot & 1) == 0){
            return i;
        }
        slot >>= 1;
    }
    return -EIO;
}

static uint64_t sata_get_size(ahci_sata_device_t* device){
    uint64_t size = 0;
    if(device->identify_info->extended_number_of_user_addressable_sectors){
        size = (uint64_t)(device->identify_info->extended_number_of_user_addressable_sectors << 9);
    }else{
        size = (uint64_t)(device->identify_info->total_user_addressable_sectors << 9);
    }
    return size;
}

static int sata_identify(ahci_sata_device_t* device){
    device->port->interrupt_status = (uint32_t)-1; // clear pending interrupt bits

    device->command_header->command_fis_length = sizeof(fis_host_to_device_registers_t) / sizeof(uint32_t); // command fis size;
    device->command_header->atapi = 0;
    device->command_header->write = 0; // read mode
    device->command_header->prdt_length = 1;

    hba_command_table_t* command_table = device->command_address_table[device->main_slot];

    command_table->prdt_entry[0].data_base_address = (uint64_t)device->identify_info_physical;
    command_table->prdt_entry[0].byte_count = sizeof(identify_info_t) - 1; // 512 bytes per sector
    command_table->prdt_entry[0].interrupt_on_completion = 1; 

    fis_host_to_device_registers_t* command_fis = (fis_host_to_device_registers_t*)(&command_table->command_fis);

    command_fis->fis_type = FIS_TYPE_HOST_TO_DEVICE;
    command_fis->command_control = 1; // command
    command_fis->command = ATA_COMMAND_IDENTIFY; // identify command


    uint64_t spin = 0;
    while((device->port->task_file_data & (ATA_DEV_BUSY | ATA_FIS_DRQ)) && spin < ATA_CMD_TIMEOUT){
        spin++;
    }
    if(spin == ATA_CMD_TIMEOUT){
        return EIO;
    }


    device->port->command_issue = 1 << device->main_slot;

    while (true){
        if((device->port->command_issue & (1 << device->main_slot)) == 0) break;
        if(device->port->interrupt_status & HBA_INTERRUPT_STATU_TFE){
            return EIO;
        }
    }

    return 0;
}

static int sata_read(ahci_device_t* ahci_device, uint64_t start, size_t size, void* buffer){
    ahci_sata_device_t* device = (ahci_sata_device_t*)ahci_device->internal_data;

    uint64_t start_alignement = start & 0x1ff;
    uint64_t sector = start >> 9;
    uint64_t sector_count = DIV_ROUNDUP(size + start_alignement, ATA_SECTOR_SIZE);
    uint64_t prdt_length = DIV_ROUNDUP(sector_count, HBA_PRDT_ENTRY_SECTOR_COUNT);

    if(prdt_length > HBA_PRDT_MAX_ENTRIES){
        return EIO;
    }

    uint32_t sector_low = (uint32_t)sector & 0xffffffff;
    uint32_t sector_high = (uint32_t)(sector >> 32) & 0xffffffff;

    device->port->interrupt_status = (uint32_t)-1; // clear pending interrupt bits

    device->command_header->command_fis_length = sizeof(fis_host_to_device_registers_t) / sizeof(uint32_t); // command fis size;
    device->command_header->atapi = 0;
    device->command_header->write = 0; // read mode
    device->command_header->prdt_length = prdt_length;

    hba_command_table_t* command_table = device->command_address_table[device->main_slot];
    fis_host_to_device_registers_t* command_fis = (fis_host_to_device_registers_t*)(&command_table->command_fis);

    // load prdt

    uint64_t sector_count_iteration = sector_count;

    for(uint16_t i = 0; i < prdt_length; i++){
        uint64_t sector_count_to_load = sector_count_iteration;
        if(sector_count_to_load > HBA_PRDT_ENTRY_SECTOR_COUNT){
            sector_count_to_load = HBA_PRDT_ENTRY_SECTOR_COUNT;
        }

        command_table->prdt_entry[i].data_base_address = (uint64_t)vmm_get_physical_address(vmm_get_current_space(), buffer + i * HBA_PRDT_ENTRY_ADDRESS_SIZE);
        command_table->prdt_entry[i].byte_count = (sector_count_to_load << 9) - 1; // 512 bytes per sector
        command_table->prdt_entry[i].interrupt_on_completion = 1; 
        sector_count_iteration -= sector_count_to_load;
	}

    command_fis->fis_type = FIS_TYPE_HOST_TO_DEVICE;
    command_fis->command_control = 1; // command
    command_fis->command = ATA_COMMAND_READ_DMA; // read command

    command_fis->lba0 = (uint8_t)sector_low & 0xff;
    command_fis->lba1 = (uint8_t)(sector_low >> 8) & 0xff;
    command_fis->lba2 = (uint8_t)(sector_low >> 16) & 0xff;
    command_fis->lba3 = (uint8_t)sector_high & 0xff;
    command_fis->lba4 = (uint8_t)(sector_high >> 8) & 0xff;
    command_fis->lba4 = (uint8_t)(sector_high >> 16) & 0xff;

    command_fis->device_register = 1 << 6; // lba mode

    command_fis->count_low = sector_count & 0xff;
    command_fis->count_high = (sector_count >> 8) & 0xff;

    uint64_t spin = 0;
    while((device->port->task_file_data & (ATA_DEV_BUSY | ATA_FIS_DRQ)) && spin < ATA_CMD_TIMEOUT){
        spin++;
    }
    if(spin == ATA_CMD_TIMEOUT){
        return EIO;
    }

    device->port->command_issue = 1 << device->main_slot;

    while(true){
        if((device->port->command_issue & (1 << device->main_slot)) == 0) break;
        if(device->port->interrupt_status & HBA_INTERRUPT_STATU_TFE){
            return EIO;
        }
    }

    return 0;
}

static int sata_write(ahci_device_t* ahci_device, uint64_t start, size_t size, void* buffer){
    ahci_sata_device_t* device = (ahci_sata_device_t*)ahci_device->internal_data;

    uint64_t start_alignement = start & 0x1ff;
    uint64_t sector = start >> 9;
    uint64_t sector_count = DIV_ROUNDUP(size + start_alignement, ATA_SECTOR_SIZE);
    uint64_t prdt_length = DIV_ROUNDUP(sector_count, HBA_PRDT_ENTRY_SECTOR_COUNT);

    if(prdt_length > HBA_PRDT_MAX_ENTRIES){
        return EIO;
    }

    uint32_t sector_low = (uint32_t)sector & 0xffffffff;
    uint32_t sector_high = (uint32_t)(sector >> 32) & 0xffffffff;

    device->port->interrupt_status = (uint32_t)-1; // clear pending interrupt bits

    device->command_header->command_fis_length = sizeof(fis_host_to_device_registers_t) / sizeof(uint32_t); // command fis size;
    device->command_header->atapi = 0;
    device->command_header->write = 1; // write mode
    device->command_header->prdt_length = prdt_length;

    hba_command_table_t* command_table = device->command_address_table[device->main_slot];
    fis_host_to_device_registers_t* command_fis = (fis_host_to_device_registers_t*)(&command_table->command_fis);

    // load prdt

    uint64_t sector_count_iteration = sector_count;

    for(uint16_t i = 0; i < prdt_length; i++){
        uint64_t sector_count_to_load = sector_count_iteration;
        if(sector_count_to_load > HBA_PRDT_ENTRY_SECTOR_COUNT){
            sector_count_to_load = HBA_PRDT_ENTRY_SECTOR_COUNT;
        }

        command_table->prdt_entry[i].data_base_address = (uint64_t)vmm_get_physical_address(kernel_space, (void*)((uintptr_t)buffer + (uintptr_t)i * (uintptr_t)HBA_PRDT_ENTRY_ADDRESS_SIZE));
        command_table->prdt_entry[i].byte_count = (sector_count_to_load << 9) - 1; // 512 bytes per sector
        command_table->prdt_entry[i].interrupt_on_completion = 1; 
        sector_count_iteration -= sector_count_to_load;
	}

    command_fis->fis_type = FIS_TYPE_HOST_TO_DEVICE;
    command_fis->command_control = 1; // command
    command_fis->command = ATA_COMMAND_WRITE_DMA; // write command

    command_fis->lba0 = (uint8_t)sector_low & 0xff;
    command_fis->lba1 = (uint8_t)(sector_low >> 8) & 0xff;
    command_fis->lba2 = (uint8_t)(sector_low >> 16) & 0xff;
    command_fis->lba3 = (uint8_t)sector_high & 0xff;
    command_fis->lba4 = (uint8_t)(sector_high >> 8) & 0xff;
    command_fis->lba4 = (uint8_t)(sector_high >> 16) & 0xff;

    command_fis->device_register = 1 << 6; // lba mode

    command_fis->count_low = sector_count & 0xff;
    command_fis->count_high = (sector_count >> 8) & 0xff;

    uint64_t spin = 0;
    while((device->port->task_file_data & (ATA_DEV_BUSY | ATA_FIS_DRQ)) && spin < ATA_CMD_TIMEOUT){
        spin++;
    }
    if(spin == ATA_CMD_TIMEOUT){
        return EIO;
    }

    device->port->command_issue = 1 << device->main_slot;

    while(true){
        if((device->port->command_issue & (1 << device->main_slot)) == 0) break;
        if(device->port->interrupt_status & HBA_INTERRUPT_STATU_TFE){
            return EIO;
        }
    }

    return 0;
}

ahci_device_t* init_sata_device(hba_port_t* port){
    ahci_sata_device_t* device = malloc(sizeof(ahci_sata_device_t));
    device->ahci_device.internal_data = device;

    device->port = port;
    device->ahci_device.read = sata_read;
    device->ahci_device.write = sata_write;

    sata_stop_cmd(device);
    device->main_slot = sata_find_slot(device);
    
    device->identify_info_physical = pmm_allocate_page();
    device->identify_info = vmm_get_virtual_address(device->identify_info_physical);

    device->port->command_list_base = (uint64_t)pmm_allocate_page();
    device->command_header = vmm_get_virtual_address((void*)device->port->command_list_base);
    memset(device->command_header, 0, 1024);

    device->port->fis_base_address = (uint64_t)pmm_allocate_page();
    void* fis_base_virtual = vmm_get_virtual_address((void*)device->port->fis_base_address);
    memset(fis_base_virtual, 0, 256);

    device->command_header[device->main_slot].command_table_base_address = (uint64_t)pmm_allocate_pages(DIV_ROUNDUP(HBA_COMMAND_TABLE_SIZE, PAGE_SIZE));
    device->command_address_table[device->main_slot] = vmm_get_virtual_address((void*)device->command_header[device->main_slot].command_table_base_address);
    memset(device->command_address_table[device->main_slot], 0, HBA_COMMAND_TABLE_SIZE);

    sata_start_cmd(device);

    sata_identify(device);

    device->ahci_device.lock = (spinlock_t){};

    device->ahci_device.size = sata_get_size(device);

    device->ahci_device.alignement = ATA_SECTOR_SIZE;

    device->ahci_device.block_cache = vmm_get_free_contiguous(HBA_MAX_BLOCK_SIZE);
    device->ahci_device.block_cache_size = HBA_MAX_BLOCK_SIZE;

    return &device->ahci_device;
}