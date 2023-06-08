#include <multiboot/vmm/vmm.h>

pagetable_t vmm_PageTable;
uint64_t vmm_HHDMAdress = 0;
uint64_t vmm_BitmapAddress = 0;
size64_t vmm_BitmapSize = 0;

bool vmm_GetFlag(uint64_t* entry, uint8_t flag){
    return *entry & (1 << flag); 
}

void vmm_SetFlag(uint64_t* entry, uint8_t flag, bool enabled){
    uint64_t bitSelector = (uint64_t)(1 << flag);
    if (enabled){
        *entry |= bitSelector;
    }else{
        *entry &= ~bitSelector;
    }
}

void vmm_SetIfNotFlag(uint64_t* entry, uint8_t flag, bool enabled){
    uint64_t bitSelector = (uint64_t)(1 << flag);
    *entry |= bitSelector;
}

void vmm_CopyFlag(uint64_t* entryDst, uint64_t entrySrc){
    *entryDst |= (entrySrc & 0xfff);
}

uint64_t vmm_GetAddress(uint64_t* entry){
    return (*entry & 0x000ffffffffff000) >> 12;
}

void vmm_SetAddress(uint64_t* entry, uint64_t address){
    address &= 0x000000ffffffffff;  /* Get the 39 bits address */
    *entry &= 0xfff0000000000fff;   /* Clear address bit */
    *entry |= (address << 12);      /* Set address value */
}

struct vmm_index vmm_Index(uint64_t virtualAddress){
    struct vmm_index index;
    virtualAddress >>= 12;
    index.P_i = virtualAddress & 0x1ff;
    virtualAddress >>= 9;
    index.PT_i = virtualAddress & 0x1ff;
    virtualAddress >>= 9;
    index.PD_i = virtualAddress & 0x1ff;
    virtualAddress >>= 9;
    index.PDP_i = virtualAddress & 0x1ff;
    return index;
}

uint64_t vmm_MapAddress(uint64_t PDP_i, uint64_t PD_i, uint64_t PT_i, uint64_t P_i){
    uint64_t virtualAddress = 0;
    virtualAddress |= P_i << 12;
    virtualAddress |= PT_i << 21;
    virtualAddress |= PD_i << 30;
    virtualAddress |= PDP_i << 39;
    return virtualAddress;   
}

bool vmm_GetFlags(pagetable_t table, uint64_t Address, vmm_flag flags){
    vmm_index indexer = vmm_Index((uint64_t)Address);
    uint64_t PDE;

    vmm_page_table* PML4VirtualAddress = (vmm_page_table*)vmm_GetVirtualAddress(table);
    PDE = PML4VirtualAddress->entries[indexer.PDP_i];
    vmm_page_table* PDP;
    vmm_page_table* PDPVirtualAddress;

    if (!vmm_GetFlag(&PDE, vmm_flag::vmm_Present)){
        return false;
    }else{
        PDP = (vmm_page_table*)(vmm_GetAddress(&PDE) << 12);
        PDPVirtualAddress = (vmm_page_table*)(vmm_GetVirtualAddress(PDP)); 
    }

    PDE = PDPVirtualAddress->entries[indexer.PD_i];

    vmm_page_table* PD;
    vmm_page_table* PDVirtualAddress;
    if (!vmm_GetFlag(&PDE, vmm_flag::vmm_Present)){
        return false;
    }else{
        PD = (vmm_page_table*)(vmm_GetAddress(&PDE) << 12);
        PDVirtualAddress = (vmm_page_table*)vmm_GetVirtualAddress(PD);  
    }

    PDPVirtualAddress->entries[indexer.PD_i] = PDE;

    PDE = PDVirtualAddress->entries[indexer.PT_i];

    vmm_page_table* PT;
    vmm_page_table* PTVirtualAddress;
    if (!vmm_GetFlag(&PDE, vmm_flag::vmm_Present)){
        return false;
    }else{
        PT = (vmm_page_table*)(vmm_GetAddress(&PDE) << 12);
        PTVirtualAddress = (vmm_page_table*)vmm_GetVirtualAddress(PT) ;
    }

    PDVirtualAddress->entries[indexer.PT_i] = PDE;

    PDE = PTVirtualAddress->entries[indexer.P_i];
    return vmm_GetFlag(&PDE, flags);   
}

void vmm_SetFlags(pagetable_t table, uint64_t Address, vmm_flag flags, bool enabled){
    vmm_index indexer = vmm_Index((uint64_t)Address);
    uint64_t PDE;

    vmm_page_table* PML4VirtualAddress = (vmm_page_table*)vmm_GetVirtualAddress(table);
    PDE = PML4VirtualAddress->entries[indexer.PDP_i];
    uint64_t PDP;
    vmm_page_table* PDPVirtualAddress;

    if (!vmm_GetFlag(&PDE, vmm_flag::vmm_Present)){
        return;
    }else{
        PDP = (uint64_t)((uint64_t)vmm_GetAddress(&PDE) << 12);
        PDPVirtualAddress = (vmm_page_table*)vmm_GetVirtualAddress(PDP);    
    }

    PDE = PDPVirtualAddress->entries[indexer.PD_i];

    uint64_t PD;
    vmm_page_table* PDVirtualAddress;
    if (!vmm_GetFlag(&PDE, vmm_flag::vmm_Present)){
        return;
    }else{
        PD = (uint64_t)((uint64_t)vmm_GetAddress(&PDE) << 12);
        PDVirtualAddress = (vmm_page_table*)vmm_GetVirtualAddress(PD);  
    }

    PDE = PDVirtualAddress->entries[indexer.PT_i];

    uint64_t PT;
    vmm_page_table* PTVirtualAddress;
    if (!vmm_GetFlag(&PDE, vmm_flag::vmm_Present)){
        return;
    }else{
        PT = (uint64_t)((uint64_t)vmm_GetAddress(&PDE) << 12);
        PTVirtualAddress = (vmm_page_table*)vmm_GetVirtualAddress(PT);  
    }

    PDE = PTVirtualAddress->entries[indexer.P_i];
    vmm_SetFlag(&PDE, flags, enabled);   
    PTVirtualAddress->entries[indexer.P_i] = PDE;
}

uint64_t vmm_Map(uint64_t physicalAddress){
    uint64_t virtualAddress = vmm_GetVirtualAddress(physicalAddress);
    vmm_Map(vmm_PageTable, (uint64_t)virtualAddress, physicalAddress);
    return virtualAddress;
}

void vmm_Map(uint64_t Address, uint64_t physicalAddress){
    vmm_Map(vmm_PageTable, Address, physicalAddress);
}

void vmm_Map(pagetable_t table, uint64_t Address, uint64_t physicalAddress){
    vmm_Map(table, Address, physicalAddress, false, true);
}
void vmm_Map(pagetable_t table, uint64_t Address, uint64_t physicalAddress, bool user){
    vmm_Map(table, Address, physicalAddress, user, true);
}

void vmm_Map(pagetable_t table, uint64_t Address, uint64_t physicalAddress, bool user, bool readWrite){
    vmm_Map(table, Address, physicalAddress, user, readWrite, true);
}

void vmm_Map(pagetable_t table, uint64_t Address, uint64_t physicalAddress, bool user, bool readWrite, bool isPureMemory){
    vmm_index indexer = vmm_Index((uint64_t)Address);
    uint64_t PDE;

    vmm_page_table* PML4VirtualAddress = (vmm_page_table*)vmm_GetVirtualAddress(table);
    PDE = PML4VirtualAddress->entries[indexer.PDP_i];
    uint64_t PDP;
    vmm_page_table* PDPVirtualAddress;

    if(!vmm_GetFlag(&PDE, vmm_flag::vmm_Present)){
        PDP = (uint64_t)Pmm_RequestPage();
        PDPVirtualAddress = (vmm_page_table*)vmm_GetVirtualAddress(PDP);  
        memset(PDPVirtualAddress, 0, PAGE_SIZE);
        vmm_SetAddress(&PDE, (uint64_t)PDP >> 12);
        vmm_SetFlag(&PDE, vmm_flag::vmm_Present, true);
        vmm_SetFlag(&PDE, vmm_flag::vmm_ReadWrite, true);
    }else{
        PDP = (uint64_t)((uint64_t)vmm_GetAddress(&PDE) << 12);
        PDPVirtualAddress = (vmm_page_table*)vmm_GetVirtualAddress(PDP);    
    }

    vmm_SetIfNotFlag(&PDE, vmm_flag::vmm_User, user);
    PML4VirtualAddress->entries[indexer.PDP_i] = PDE;

    
    PDE = PDPVirtualAddress->entries[indexer.PD_i];

    uint64_t PD;
    vmm_page_table* PDVirtualAddress;
    if(!vmm_GetFlag(&PDE, vmm_flag::vmm_Present)){
        PD = (uint64_t)Pmm_RequestPage();
        PDVirtualAddress = (vmm_page_table*)vmm_GetVirtualAddress(PD); 
        memset(PDVirtualAddress, 0, PAGE_SIZE);
        vmm_SetAddress(&PDE, (uint64_t)PD >> 12);
        vmm_SetFlag(&PDE, vmm_flag::vmm_Present, true);
        vmm_SetFlag(&PDE, vmm_flag::vmm_ReadWrite, true);
    }else{
        PD = (uint64_t)((uint64_t)vmm_GetAddress(&PDE) << 12);
        PDVirtualAddress = (vmm_page_table*)vmm_GetVirtualAddress(PD);  
    }
    
    vmm_SetIfNotFlag(&PDE, vmm_flag::vmm_User, user);
    PDPVirtualAddress->entries[indexer.PD_i] = PDE;


    PDE = PDVirtualAddress->entries[indexer.PT_i];
    uint64_t PT;
    vmm_page_table* PTVirtualAddress;
    if(!vmm_GetFlag(&PDE, vmm_flag::vmm_Present)){
        PT = (uint64_t)Pmm_RequestPage();
        PTVirtualAddress = (vmm_page_table*)vmm_GetVirtualAddress(PT);
        memset(PTVirtualAddress, 0, PAGE_SIZE);
        vmm_SetAddress(&PDE, (uint64_t)PT >> 12);
        vmm_SetFlag(&PDE, vmm_flag::vmm_Present, true);
        vmm_SetFlag(&PDE, vmm_flag::vmm_ReadWrite, true);
    }else{
        PT = (uint64_t)((uint64_t)vmm_GetAddress(&PDE) << 12);
        PTVirtualAddress = (vmm_page_table*)vmm_GetVirtualAddress(PT);  
    }
    
    vmm_SetIfNotFlag(&PDE, vmm_flag::vmm_User, user);
    PDVirtualAddress->entries[indexer.PT_i] = PDE;


    PDE = PTVirtualAddress->entries[indexer.P_i];
    vmm_SetAddress(&PDE, (uint64_t)physicalAddress >> 12);
    vmm_SetFlag(&PDE, vmm_flag::vmm_Present, true);
    vmm_SetFlag(&PDE, vmm_flag::vmm_ReadWrite, readWrite);
    vmm_SetFlag(&PDE, vmm_flag::vmm_IsPureMemory, isPureMemory);
    vmm_SetFlag(&PDE, vmm_flag::vmm_User, user);
    PTVirtualAddress->entries[indexer.P_i] = PDE;
}

void vmm_Unmap(uint64_t Address){
    vmm_Unmap(vmm_PageTable, Address);
}

void vmm_Unmap(pagetable_t table, uint64_t Address){
    vmm_index indexer = vmm_Index((uint64_t)Address);
    uint64_t PDE;

    vmm_page_table* PML4VirtualAddress = (vmm_page_table*)vmm_GetVirtualAddress(table);
    PDE = PML4VirtualAddress->entries[indexer.PDP_i];
    uint64_t PDP;
    vmm_page_table* PDPVirtualAddress;

    if (!vmm_GetFlag(&PDE, vmm_flag::vmm_Present)){
        return;
    }else{
        PDP = (uint64_t)((uint64_t)vmm_GetAddress(&PDE) << 12);
        PDPVirtualAddress = (vmm_page_table*)vmm_GetVirtualAddress(PDP);       
    }
    
    PML4VirtualAddress->entries[indexer.PDP_i] = PDE;

    PDE = PDPVirtualAddress->entries[indexer.PD_i];

    uint64_t PD;
    vmm_page_table* PDVirtualAddress;
    if (!vmm_GetFlag(&PDE, vmm_flag::vmm_Present)){
        return;
    }else{
        PD = (uint64_t)((uint64_t)vmm_GetAddress(&PDE) << 12);
        PDVirtualAddress = (vmm_page_table*)vmm_GetVirtualAddress(PD);     
    }

    PDPVirtualAddress->entries[indexer.PD_i] = PDE;

    PDE = PDVirtualAddress->entries[indexer.PT_i];

    uint64_t PT;
    vmm_page_table* PTVirtualAddress;
    if (!vmm_GetFlag(&PDE, vmm_flag::vmm_Present)){
        return;
    }else{
        PT = (uint64_t)((uint64_t)vmm_GetAddress(&PDE) << 12);
        PTVirtualAddress = (vmm_page_table*)vmm_GetVirtualAddress(PT);    
    }

    PTVirtualAddress->entries[indexer.P_i] = NULL;  
}

uint64_t vmm_GetPhysical(pagetable_t table, uint64_t Address){
    vmm_index indexer = vmm_Index((uint64_t)Address);
    uint64_t PDE;

    vmm_page_table* PML4VirtualAddress = (vmm_page_table*)vmm_GetVirtualAddress(table);
    PDE = PML4VirtualAddress->entries[indexer.PDP_i];
    uint64_t PDP;
    vmm_page_table* PDPVirtualAddress;

    if(!vmm_GetFlag(&PDE, vmm_flag::vmm_Present)){
        return NULL;
    }else{
        PDP = (uint64_t)((uint64_t)vmm_GetAddress(&PDE) << 12);
        PDPVirtualAddress = (vmm_page_table*)vmm_GetVirtualAddress(PDP);       
    }
    
    PML4VirtualAddress->entries[indexer.PDP_i] = PDE;

    PDE = PDPVirtualAddress->entries[indexer.PD_i];

    uint64_t PD;
    vmm_page_table* PDVirtualAddress;
    if (!vmm_GetFlag(&PDE, vmm_flag::vmm_Present)){
        return NULL;
    }else{
        PD = (uint64_t)((uint64_t)vmm_GetAddress(&PDE) << 12);
        PDVirtualAddress = (vmm_page_table*)vmm_GetVirtualAddress(PD);     
    }

    PDPVirtualAddress->entries[indexer.PD_i] = PDE;

    PDE = PDVirtualAddress->entries[indexer.PT_i];

    uint64_t PT;
    vmm_page_table* PTVirtualAddress;
    if (!vmm_GetFlag(&PDE, vmm_flag::vmm_Present)){
        return NULL;
    }else{
        PT = (uint64_t)((uint64_t)vmm_GetAddress(&PDE) << 12);
        PTVirtualAddress = (vmm_page_table*)vmm_GetVirtualAddress(PT);    
    }

    PDE = PTVirtualAddress->entries[indexer.P_i];
    return (uint64_t)((uint64_t)vmm_GetAddress(&PDE) * PAGE_SIZE + (uint64_t)Address % PAGE_SIZE);
}

void vmm_Init(struct multiboot_tag_mmap* Map, uint64_t* Stack){
    vmm_PageTable = (pagetable_t)Pmm_RequestPage();
    memset((void*)vmm_PageTable, 0, PAGE_SIZE);

    vmm_HHDMAdress = PML4_HIGHER_HALF_ADDRESS;

    /* allocate stack into higher half address*/
    uint64_t StackSize = STACK_SIZE;
    uint64_t StackVirtualAddressEnd = vmm_HHDMAdress;

    for(uint64_t i = 0; i < StackSize; i += PAGE_SIZE){
        uint64_t PhysicalAddress = (uint64_t)Pmm_RequestPage();
        vmm_Map(vmm_PageTable, (uint64_t)StackVirtualAddressEnd, (uint64_t)PhysicalAddress, false, true, true);
        StackVirtualAddressEnd += PAGE_SIZE;
    }
    *Stack = StackVirtualAddressEnd;
    vmm_HHDMAdress = StackVirtualAddressEnd;

    /* allocate bitmap into higher half address */
    uint64_t MemoryPageNumber = Pmm_PageNumber;

    vmm_BitmapAddress = vmm_HHDMAdress;
    uint64_t BitmapVirtualAddressEnd = vmm_BitmapAddress;

    for(uint64_t i = 0; i < MemoryPageNumber; i += PAGE_SIZE * BYTE_SIZE){
        uint64_t PhysicalAddress = (uint64_t)Pmm_RequestPage();
        vmm_Map(vmm_PageTable, BitmapVirtualAddressEnd, PhysicalAddress, false, true, true);
        BitmapVirtualAddressEnd += PAGE_SIZE;
        vmm_BitmapSize += PAGE_SIZE;
    }
    vmm_HHDMAdress = BitmapVirtualAddressEnd;

    /* allocate bitmap into higher half address */
    uint64_t NumberOfEntry = (Pmm_Map->size - sizeof(multiboot_tag_mmap)) / Pmm_Map->entry_size;
    for(uint64_t i = 0; i < NumberOfEntry; i++){
        for(uint64_t y = 0; y < Map->entries[i].len; y += PAGE_SIZE){
            uint64_t physicalAddress = Map->entries[i].addr + y;
            uint64_t virtualAddress = physicalAddress + vmm_HHDMAdress;
            vmm_Map(vmm_PageTable, virtualAddress, physicalAddress, false, true, true);
        }
    }
    for(uint64_t y = (uint64_t)&Pmm_Ukl_Start; y < (uint64_t)&Pmm_Ukl_End; y += PAGE_SIZE){
        vmm_Map(vmm_PageTable, y, y, false, true, true);
    }
}

void Vmm_ExtractsInfo(struct ukl_boot_structure_t* BootData){
    BootData->memory_info.HHDM = vmm_HHDMAdress;
    BootData->memory_info.bitmap_address = vmm_BitmapAddress;
    BootData->memory_info.bitmap_size = vmm_BitmapSize;
    BootData->memory_info.page_table = (uint64_t)vmm_PageTable;
}