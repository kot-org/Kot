#include <multiboot/vmm/vmm.h>

pagetable_t vmm_PageTable;
uint64_t vmm_HHDMAdress = 0;
uint64_t vmm_BitmapAddress = 0;

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

bool vmm_GetFlags(pagetable_t table, uintptr_t Address, vmm_flag flags){
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

void vmm_SetFlags(pagetable_t table, uintptr_t Address, vmm_flag flags, bool enabled){
    vmm_index indexer = vmm_Index((uint64_t)Address);
    uint64_t PDE;

    vmm_page_table* PML4VirtualAddress = (vmm_page_table*)vmm_GetVirtualAddress(table);
    PDE = PML4VirtualAddress->entries[indexer.PDP_i];
    uintptr_t PDP;
    vmm_page_table* PDPVirtualAddress;

    if (!vmm_GetFlag(&PDE, vmm_flag::vmm_Present)){
        return;
    }else{
        PDP = (uintptr_t)((uint64_t)vmm_GetAddress(&PDE) << 12);
        PDPVirtualAddress = (vmm_page_table*)vmm_GetVirtualAddress(PDP);    
    }

    PDE = PDPVirtualAddress->entries[indexer.PD_i];

    uintptr_t PD;
    vmm_page_table* PDVirtualAddress;
    if (!vmm_GetFlag(&PDE, vmm_flag::vmm_Present)){
        return;
    }else{
        PD = (uintptr_t)((uint64_t)vmm_GetAddress(&PDE) << 12);
        PDVirtualAddress = (vmm_page_table*)vmm_GetVirtualAddress(PD);  
    }

    PDE = PDVirtualAddress->entries[indexer.PT_i];

    uintptr_t PT;
    vmm_page_table* PTVirtualAddress;
    if (!vmm_GetFlag(&PDE, vmm_flag::vmm_Present)){
        return;
    }else{
        PT = (uintptr_t)((uint64_t)vmm_GetAddress(&PDE) << 12);
        PTVirtualAddress = (vmm_page_table*)vmm_GetVirtualAddress(PT);  
    }

    PDE = PTVirtualAddress->entries[indexer.P_i];
    vmm_SetFlag(&PDE, flags, enabled);   
    PTVirtualAddress->entries[indexer.P_i] = PDE;
}

uint64_t vmm_Map(uintptr_t physicalAddress){
    uint64_t virtualAddress = vmm_GetVirtualAddress(physicalAddress);
    vmm_Map(vmm_PageTable, (uintptr_t)virtualAddress, physicalAddress);
    return virtualAddress;
}

void vmm_Map(uintptr_t Address, uintptr_t physicalAddress){
    vmm_Map(vmm_PageTable, Address, physicalAddress);
}

void vmm_Map(pagetable_t table, uintptr_t Address, uintptr_t physicalAddress){
    vmm_Map(table, Address, physicalAddress, false, true);
}
void vmm_Map(pagetable_t table, uintptr_t Address, uintptr_t physicalAddress, bool user){
    vmm_Map(table, Address, physicalAddress, user, true);
}

void vmm_Map(pagetable_t table, uintptr_t Address, uintptr_t physicalAddress, bool user, bool readWrite){
    vmm_Map(table, Address, physicalAddress, user, readWrite, true);
}

void vmm_Map(pagetable_t table, uintptr_t Address, uintptr_t physicalAddress, bool user, bool readWrite, bool physicalStorage){
    vmm_index indexer = vmm_Index((uint64_t)Address);
    uint64_t PDE;

    vmm_page_table* PML4VirtualAddress = (vmm_page_table*)vmm_GetVirtualAddress(table);
    PDE = PML4VirtualAddress->entries[indexer.PDP_i];
    uintptr_t PDP;
    vmm_page_table* PDPVirtualAddress;

    if(!vmm_GetFlag(&PDE, vmm_flag::vmm_Present)){
        PDP = (uintptr_t)Pmm_RequestPage();
        PDPVirtualAddress = (vmm_page_table*)vmm_GetVirtualAddress(PDP);  
        memset(PDPVirtualAddress, 0, PAGE_SIZE);
        vmm_SetAddress(&PDE, (uint64_t)PDP >> 12);
        vmm_SetFlag(&PDE, vmm_flag::vmm_Present, true);
        vmm_SetFlag(&PDE, vmm_flag::vmm_ReadWrite, true);
    }else{
        PDP = (uintptr_t)((uint64_t)vmm_GetAddress(&PDE) << 12);
        PDPVirtualAddress = (vmm_page_table*)vmm_GetVirtualAddress(PDP);    
    }

    vmm_SetIfNotFlag(&PDE, vmm_flag::vmm_User, user);
    PML4VirtualAddress->entries[indexer.PDP_i] = PDE;

    
    PDE = PDPVirtualAddress->entries[indexer.PD_i];

    uintptr_t PD;
    vmm_page_table* PDVirtualAddress;
    if(!vmm_GetFlag(&PDE, vmm_flag::vmm_Present)){
        PD = (uintptr_t)Pmm_RequestPage();
        PDVirtualAddress = (vmm_page_table*)vmm_GetVirtualAddress(PD); 
        memset(PDVirtualAddress, 0, PAGE_SIZE);
        vmm_SetAddress(&PDE, (uint64_t)PD >> 12);
        vmm_SetFlag(&PDE, vmm_flag::vmm_Present, true);
        vmm_SetFlag(&PDE, vmm_flag::vmm_ReadWrite, true);
    }else{
        PD = (uintptr_t)((uint64_t)vmm_GetAddress(&PDE) << 12);
        PDVirtualAddress = (vmm_page_table*)vmm_GetVirtualAddress(PD);  
    }
    
    vmm_SetIfNotFlag(&PDE, vmm_flag::vmm_User, user);
    PDPVirtualAddress->entries[indexer.PD_i] = PDE;


    PDE = PDVirtualAddress->entries[indexer.PT_i];
    uintptr_t PT;
    vmm_page_table* PTVirtualAddress;
    if(!vmm_GetFlag(&PDE, vmm_flag::vmm_Present)){
        PT = (uintptr_t)Pmm_RequestPage();
        PTVirtualAddress = (vmm_page_table*)vmm_GetVirtualAddress(PT);
        memset(PTVirtualAddress, 0, PAGE_SIZE);
        vmm_SetAddress(&PDE, (uint64_t)PT >> 12);
        vmm_SetFlag(&PDE, vmm_flag::vmm_Present, true);
        vmm_SetFlag(&PDE, vmm_flag::vmm_ReadWrite, true);
    }else{
        PT = (uintptr_t)((uint64_t)vmm_GetAddress(&PDE) << 12);
        PTVirtualAddress = (vmm_page_table*)vmm_GetVirtualAddress(PT);  
    }
    
    vmm_SetIfNotFlag(&PDE, vmm_flag::vmm_User, user);
    PDVirtualAddress->entries[indexer.PT_i] = PDE;


    PDE = PTVirtualAddress->entries[indexer.P_i];
    vmm_SetAddress(&PDE, (uint64_t)physicalAddress >> 12);
    vmm_SetFlag(&PDE, vmm_flag::vmm_Present, true);
    vmm_SetFlag(&PDE, vmm_flag::vmm_ReadWrite, readWrite);
    vmm_SetFlag(&PDE, vmm_flag::vmm_PhysicalStorage, physicalStorage);
    vmm_SetIfNotFlag(&PDE, vmm_flag::vmm_User, user);
    PTVirtualAddress->entries[indexer.P_i] = PDE;
}

void vmm_Unmap(uintptr_t Address){
    vmm_Unmap(vmm_PageTable, Address);
}

void vmm_Unmap(pagetable_t table, uintptr_t Address){
    vmm_index indexer = vmm_Index((uint64_t)Address);
    uint64_t PDE;

    vmm_page_table* PML4VirtualAddress = (vmm_page_table*)vmm_GetVirtualAddress(table);
    PDE = PML4VirtualAddress->entries[indexer.PDP_i];
    uintptr_t PDP;
    vmm_page_table* PDPVirtualAddress;

    if (!vmm_GetFlag(&PDE, vmm_flag::vmm_Present)){
        return;
    }else{
        PDP = (uintptr_t)((uint64_t)vmm_GetAddress(&PDE) << 12);
        PDPVirtualAddress = (vmm_page_table*)vmm_GetVirtualAddress(PDP);       
    }
    
    PML4VirtualAddress->entries[indexer.PDP_i] = PDE;

    PDE = PDPVirtualAddress->entries[indexer.PD_i];

    uintptr_t PD;
    vmm_page_table* PDVirtualAddress;
    if (!vmm_GetFlag(&PDE, vmm_flag::vmm_Present)){
        return;
    }else{
        PD = (uintptr_t)((uint64_t)vmm_GetAddress(&PDE) << 12);
        PDVirtualAddress = (vmm_page_table*)vmm_GetVirtualAddress(PD);     
    }

    PDPVirtualAddress->entries[indexer.PD_i] = PDE;

    PDE = PDVirtualAddress->entries[indexer.PT_i];

    uintptr_t PT;
    vmm_page_table* PTVirtualAddress;
    if (!vmm_GetFlag(&PDE, vmm_flag::vmm_Present)){
        return;
    }else{
        PT = (uintptr_t)((uint64_t)vmm_GetAddress(&PDE) << 12);
        PTVirtualAddress = (vmm_page_table*)vmm_GetVirtualAddress(PT);    
    }

    PTVirtualAddress->entries[indexer.P_i] = NULL;  
}

uintptr_t vmm_GetPhysical(pagetable_t table, uintptr_t Address){
    vmm_index indexer = vmm_Index((uint64_t)Address);
    uint64_t PDE;

    vmm_page_table* PML4VirtualAddress = (vmm_page_table*)vmm_GetVirtualAddress(table);
    PDE = PML4VirtualAddress->entries[indexer.PDP_i];
    uintptr_t PDP;
    vmm_page_table* PDPVirtualAddress;

    if(!vmm_GetFlag(&PDE, vmm_flag::vmm_Present)){
        return NULL;
    }else{
        PDP = (uintptr_t)((uint64_t)vmm_GetAddress(&PDE) << 12);
        PDPVirtualAddress = (vmm_page_table*)vmm_GetVirtualAddress(PDP);       
    }
    
    PML4VirtualAddress->entries[indexer.PDP_i] = PDE;

    PDE = PDPVirtualAddress->entries[indexer.PD_i];

    uintptr_t PD;
    vmm_page_table* PDVirtualAddress;
    if (!vmm_GetFlag(&PDE, vmm_flag::vmm_Present)){
        return NULL;
    }else{
        PD = (uintptr_t)((uint64_t)vmm_GetAddress(&PDE) << 12);
        PDVirtualAddress = (vmm_page_table*)vmm_GetVirtualAddress(PD);     
    }

    PDPVirtualAddress->entries[indexer.PD_i] = PDE;

    PDE = PDVirtualAddress->entries[indexer.PT_i];

    uintptr_t PT;
    vmm_page_table* PTVirtualAddress;
    if (!vmm_GetFlag(&PDE, vmm_flag::vmm_Present)){
        return NULL;
    }else{
        PT = (uintptr_t)((uint64_t)vmm_GetAddress(&PDE) << 12);
        PTVirtualAddress = (vmm_page_table*)vmm_GetVirtualAddress(PT);    
    }

    PDE = PTVirtualAddress->entries[indexer.P_i];
    return (uintptr_t)((uint64_t)vmm_GetAddress(&PDE) * PAGE_SIZE + (uint64_t)Address % PAGE_SIZE);
}

void vmm_CopyPageTable(pagetable_t tableSource, pagetable_t tableDestination, uint64_t from, uint64_t to){
    vmm_page_table* PML4VirtualAddressDestination = (vmm_page_table*)vmm_GetVirtualAddress(tableDestination);
    vmm_page_table* PML4VirtualAddressSource = (vmm_page_table*)vmm_GetVirtualAddress(tableSource);
    for(uint16_t i = from; i < to; i++){
        PML4VirtualAddressDestination->entries[i] = PML4VirtualAddressSource->entries[i];
    }
}

void vmm_Fill(pagetable_t table, uint64_t from, uint64_t to, bool user){
    vmm_page_table* PML4VirtualAddress = (vmm_page_table*)vmm_GetVirtualAddress(table);
    for(uint16_t i = from; i < to; i++){
        uint64_t PDE = PML4VirtualAddress->entries[i];
        vmm_page_table* PDP;
        vmm_page_table* PDPVirtualAddress;
        if(!vmm_GetFlag(&PDE, vmm_flag::vmm_Present)){
            PDP = (vmm_page_table*)Pmm_RequestPage();
            PDPVirtualAddress = (vmm_page_table*)vmm_GetVirtualAddress(PDP);
            memset(PDPVirtualAddress, 0, PAGE_SIZE);
            vmm_SetAddress(&PDE, (uint64_t)PDP >> 12);
            vmm_SetFlag(&PDE, vmm_flag::vmm_Present, true);
            vmm_SetFlag(&PDE, vmm_flag::vmm_ReadWrite, true);
            vmm_SetFlag(&PDE, vmm_flag::vmm_User, user);
            PML4VirtualAddress->entries[i] = PDE;
        }
    }
}

void vmm_Init(struct multiboot_tag_mmap* Map){
    vmm_PageTable = Pmm_RequestPage();
    memset(vmm_PageTable, 0, PAGE_SIZE);

    vmm_HHDMAdress = PML4_HIGHER_HALF_ADDRESS;
    vmm_BitmapAddress = vmm_HHDMAdress;

    /* allocate bitmap into higher half address*/
    uint64_t MemoryPageNumber = DivideRoundUp(Pmm_GetMemorySize(Map), PAGE_SIZE);
    uint64_t BitmapSize = DivideRoundUp(MemoryPageNumber, BYTE_SIZE);
    uint64_t BitmapPageNumber = DivideRoundUp(BitmapSize, BYTE_SIZE);

    uint64_t BitmapVirtualAddressEnd = vmm_BitmapAddress;
    for(uint64_t y = 0; y < BitmapPageNumber; y++){
        uint64_t physicalAddress = (uint64_t)Pmm_RequestPage();
        vmm_Map(vmm_PageTable, (uintptr_t)BitmapVirtualAddressEnd, (uintptr_t)physicalAddress, false, true, true);
        BitmapVirtualAddressEnd += PAGE_SIZE;
    }
    vmm_HHDMAdress = BitmapVirtualAddressEnd;

    /* map all the memory */

    for (uint64_t i = 0; i < Map->entry_size; i++){
        uint64_t PageNumber = DivideRoundUp(Map->entries[i].len, PAGE_SIZE);
        for(uint64_t y = 0; y < PageNumber; y++){
            uint64_t physicalAddress = Map->entries[i].addr + y * PAGE_SIZE;
            uint64_t virtualAddress = physicalAddress + vmm_HHDMAdress;
            vmm_Map(vmm_PageTable, (uintptr_t)virtualAddress, (uintptr_t)physicalAddress, false, true, Map->entries[i].type == MULTIBOOT_MEMORY_AVAILABLE);
        }
    }

    vmm_Fill(vmm_PageTable, VMM_LOWERHALF, VMM_HIGHERALF, false);
}

void Vmm_ExtractsInfo(){
    // TODO
}