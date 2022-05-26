#include <arch/x86-64/vmm/vmm.h>

pagetable_t vmm_PageTable;
uint64_t vmm_HHDMAdress = 0;

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

bool vmm_GetFlags(pagetable_t table, void* Address, vmm_flag flags){
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
    
    PML4VirtualAddress->entries[indexer.PDP_i] = PDE;

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

void vmm_SetFlags(pagetable_t table, void* Address, vmm_flag flags, bool enabled){
    vmm_index indexer = vmm_Index((uint64_t)Address);
    uint64_t PDE;

    vmm_page_table* PML4VirtualAddress = (vmm_page_table*)vmm_GetVirtualAddress(table);
    PDE = PML4VirtualAddress->entries[indexer.PDP_i];
    void* PDP;
    vmm_page_table* PDPVirtualAddress;

    if (!vmm_GetFlag(&PDE, vmm_flag::vmm_Present)){
        return;
    }else{
        PDP = (void*)((uint64_t)vmm_GetAddress(&PDE) << 12);
        PDPVirtualAddress = (vmm_page_table*)vmm_GetVirtualAddress(PDP);    
    }

    PDE = PDPVirtualAddress->entries[indexer.PD_i];

    void* PD;
    vmm_page_table* PDVirtualAddress;
    if (!vmm_GetFlag(&PDE, vmm_flag::vmm_Present)){
        return;
    }else{
        PD = (void*)((uint64_t)vmm_GetAddress(&PDE) << 12);
        PDVirtualAddress = (vmm_page_table*)vmm_GetVirtualAddress(PD);  
    }

    PDE = PDVirtualAddress->entries[indexer.PT_i];

    void* PT;
    vmm_page_table* PTVirtualAddress;
    if (!vmm_GetFlag(&PDE, vmm_flag::vmm_Present)){
        return;
    }else{
        PT = (void*)((uint64_t)vmm_GetAddress(&PDE) << 12);
        PTVirtualAddress = (vmm_page_table*)vmm_GetVirtualAddress(PT);  
    }

    PDE = PTVirtualAddress->entries[indexer.P_i];
    vmm_SetFlag(&PDE, flags, enabled);   
    PTVirtualAddress->entries[indexer.P_i] = PDE;
}

uint64_t vmm_Map(void* physicalAddress){
    uint64_t virtualAddress = vmm_GetVirtualAddress(physicalAddress);
    vmm_Map(vmm_PageTable, (void*)virtualAddress, physicalAddress);
    return virtualAddress;
}

void vmm_Map(void* Address, void* physicalAddress){
    vmm_Map(vmm_PageTable, Address, physicalAddress);
}

void vmm_Map(pagetable_t table, void* Address, void* physicalAddress){
    vmm_Map(table, Address, physicalAddress, false, true);
}
void vmm_Map(pagetable_t table, void* Address, void* physicalAddress, bool user){
    vmm_Map(table, Address, physicalAddress, user, true);
}

void vmm_Map(pagetable_t table, void* Address, void* physicalAddress, bool user, bool readWrite){
    vmm_index indexer = vmm_Index((uint64_t)Address);
    uint64_t PDE;

    vmm_page_table* PML4VirtualAddress = (vmm_page_table*)vmm_GetVirtualAddress(table);
    PDE = PML4VirtualAddress->entries[indexer.PDP_i];
    void* PDP;
    vmm_page_table* PDPVirtualAddress;

    if(!vmm_GetFlag(&PDE, vmm_flag::vmm_Present)){
        PDP = (void*)Pmm_RequestPage();
        PDPVirtualAddress = (vmm_page_table*)vmm_GetVirtualAddress(PDP);  
        memset(PDPVirtualAddress, 0, PAGE_SIZE);
        vmm_SetAddress(&PDE, (uint64_t)PDP >> 12);
        vmm_SetFlag(&PDE, vmm_flag::vmm_Present, true);
        vmm_SetFlag(&PDE, vmm_flag::vmm_ReadWrite, true);
    }else{
        PDP = (void*)((uint64_t)vmm_GetAddress(&PDE) << 12);
        PDPVirtualAddress = (vmm_page_table*)vmm_GetVirtualAddress(PDP);    
    }

    vmm_SetIfNotFlag(&PDE, vmm_flag::vmm_User, user);
    PML4VirtualAddress->entries[indexer.PDP_i] = PDE;

    
    PDE = PDPVirtualAddress->entries[indexer.PD_i];

    void* PD;
    vmm_page_table* PDVirtualAddress;
    if(!vmm_GetFlag(&PDE, vmm_flag::vmm_Present)){
        PD = (void*)Pmm_RequestPage();
        PDVirtualAddress = (vmm_page_table*)vmm_GetVirtualAddress(PD); 
        memset(PDVirtualAddress, 0, PAGE_SIZE);
        vmm_SetAddress(&PDE, (uint64_t)PD >> 12);
        vmm_SetFlag(&PDE, vmm_flag::vmm_Present, true);
        vmm_SetFlag(&PDE, vmm_flag::vmm_ReadWrite, true);
    }else{
        PD = (void*)((uint64_t)vmm_GetAddress(&PDE) << 12);
        PDVirtualAddress = (vmm_page_table*)vmm_GetVirtualAddress(PD);  
    }
    
    vmm_SetIfNotFlag(&PDE, vmm_flag::vmm_User, user);
    PDPVirtualAddress->entries[indexer.PD_i] = PDE;


    PDE = PDVirtualAddress->entries[indexer.PT_i];
    void* PT;
    vmm_page_table* PTVirtualAddress;
    if(!vmm_GetFlag(&PDE, vmm_flag::vmm_Present)){
        PT = (void*)Pmm_RequestPage();
        PTVirtualAddress = (vmm_page_table*)vmm_GetVirtualAddress(PT);
        memset(PTVirtualAddress, 0, PAGE_SIZE);
        vmm_SetAddress(&PDE, (uint64_t)PT >> 12);
        vmm_SetFlag(&PDE, vmm_flag::vmm_Present, true);
        vmm_SetFlag(&PDE, vmm_flag::vmm_ReadWrite, true);
    }else{
        PT = (void*)((uint64_t)vmm_GetAddress(&PDE) << 12);
        PTVirtualAddress = (vmm_page_table*)vmm_GetVirtualAddress(PT);  
    }
    
    vmm_SetIfNotFlag(&PDE, vmm_flag::vmm_User, user);
    PDVirtualAddress->entries[indexer.PT_i] = PDE;


    PDE = PTVirtualAddress->entries[indexer.P_i];
    vmm_SetAddress(&PDE, (uint64_t)physicalAddress >> 12);
    vmm_SetFlag(&PDE, vmm_flag::vmm_Present, true);
    vmm_SetFlag(&PDE, vmm_flag::vmm_ReadWrite, readWrite);
    vmm_SetIfNotFlag(&PDE, vmm_flag::vmm_User, user);
    PTVirtualAddress->entries[indexer.P_i] = PDE;
}

void vmm_Map(pagetable_t table, void* Address, void* physicalAddress, bool user, bool readWrite, bool physicalStorage){
    vmm_index indexer = vmm_Index((uint64_t)Address);
    uint64_t PDE;

    vmm_page_table* PML4VirtualAddress = (vmm_page_table*)vmm_GetVirtualAddress(table);
    PDE = PML4VirtualAddress->entries[indexer.PDP_i];
    void* PDP;
    vmm_page_table* PDPVirtualAddress;

    if(!vmm_GetFlag(&PDE, vmm_flag::vmm_Present)){
        PDP = (void*)Pmm_RequestPage();
        PDPVirtualAddress = (vmm_page_table*)vmm_GetVirtualAddress(PDP);  
        memset(PDPVirtualAddress, 0, PAGE_SIZE);
        vmm_SetAddress(&PDE, (uint64_t)PDP >> 12);
        vmm_SetFlag(&PDE, vmm_flag::vmm_Present, true);
        vmm_SetFlag(&PDE, vmm_flag::vmm_ReadWrite, true);
    }else{
        PDP = (void*)((uint64_t)vmm_GetAddress(&PDE) << 12);
        PDPVirtualAddress = (vmm_page_table*)vmm_GetVirtualAddress(PDP);    
    }

    vmm_SetIfNotFlag(&PDE, vmm_flag::vmm_User, user);
    PML4VirtualAddress->entries[indexer.PDP_i] = PDE;

    
    PDE = PDPVirtualAddress->entries[indexer.PD_i];

    void* PD;
    vmm_page_table* PDVirtualAddress;
    if(!vmm_GetFlag(&PDE, vmm_flag::vmm_Present)){
        PD = (void*)Pmm_RequestPage();
        PDVirtualAddress = (vmm_page_table*)vmm_GetVirtualAddress(PD); 
        memset(PDVirtualAddress, 0, PAGE_SIZE);
        vmm_SetAddress(&PDE, (uint64_t)PD >> 12);
        vmm_SetFlag(&PDE, vmm_flag::vmm_Present, true);
        vmm_SetFlag(&PDE, vmm_flag::vmm_ReadWrite, true);
    }else{
        PD = (void*)((uint64_t)vmm_GetAddress(&PDE) << 12);
        PDVirtualAddress = (vmm_page_table*)vmm_GetVirtualAddress(PD);  
    }
    
    vmm_SetIfNotFlag(&PDE, vmm_flag::vmm_User, user);
    PDPVirtualAddress->entries[indexer.PD_i] = PDE;


    PDE = PDVirtualAddress->entries[indexer.PT_i];
    void* PT;
    vmm_page_table* PTVirtualAddress;
    if(!vmm_GetFlag(&PDE, vmm_flag::vmm_Present)){
        PT = (void*)Pmm_RequestPage();
        PTVirtualAddress = (vmm_page_table*)vmm_GetVirtualAddress(PT);
        memset(PTVirtualAddress, 0, PAGE_SIZE);
        vmm_SetAddress(&PDE, (uint64_t)PT >> 12);
        vmm_SetFlag(&PDE, vmm_flag::vmm_Present, true);
        vmm_SetFlag(&PDE, vmm_flag::vmm_ReadWrite, true);
    }else{
        PT = (void*)((uint64_t)vmm_GetAddress(&PDE) << 12);
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

void vmm_Unmap(void* Address){
    vmm_Unmap(vmm_PageTable, Address);
}

void vmm_Unmap(pagetable_t table, void* Address){
        vmm_index indexer = vmm_Index((uint64_t)Address);
    uint64_t PDE;

    vmm_page_table* PML4VirtualAddress = (vmm_page_table*)vmm_GetVirtualAddress(table);
    PDE = PML4VirtualAddress->entries[indexer.PDP_i];
    void* PDP;
    vmm_page_table* PDPVirtualAddress;

    if (!vmm_GetFlag(&PDE, vmm_flag::vmm_Present)){
        return;
    }else{
        PDP = (void*)((uint64_t)vmm_GetAddress(&PDE) << 12);
        PDPVirtualAddress = (vmm_page_table*)vmm_GetVirtualAddress(PDP);       
    }
    
    PML4VirtualAddress->entries[indexer.PDP_i] = PDE;

    PDE = PDPVirtualAddress->entries[indexer.PD_i];

    void* PD;
    vmm_page_table* PDVirtualAddress;
    if (!vmm_GetFlag(&PDE, vmm_flag::vmm_Present)){
        return;
    }else{
        PD = (void*)((uint64_t)vmm_GetAddress(&PDE) << 12);
        PDVirtualAddress = (vmm_page_table*)vmm_GetVirtualAddress(PD);     
    }

    PDPVirtualAddress->entries[indexer.PD_i] = PDE;

    PDE = PDVirtualAddress->entries[indexer.PT_i];

    void* PT;
    vmm_page_table* PTVirtualAddress;
    if (!vmm_GetFlag(&PDE, vmm_flag::vmm_Present)){
        return;
    }else{
        PT = (void*)((uint64_t)vmm_GetAddress(&PDE) << 12);
        PTVirtualAddress = (vmm_page_table*)vmm_GetVirtualAddress(PT);    
    }

    PTVirtualAddress->entries[indexer.P_i] = 0;  
}

void* vmm_GetPhysical(pagetable_t table, void* Address){
    vmm_index indexer = vmm_Index((uint64_t)Address);
    uint64_t PDE;

    vmm_page_table* PML4VirtualAddress = (vmm_page_table*)vmm_GetVirtualAddress(table);
    PDE = PML4VirtualAddress->entries[indexer.PDP_i];
    void* PDP;
    vmm_page_table* PDPVirtualAddress;

    if(!vmm_GetFlag(&PDE, vmm_flag::vmm_Present)){
        return NULL;
    }else{
        PDP = (void*)((uint64_t)vmm_GetAddress(&PDE) << 12);
        PDPVirtualAddress = (vmm_page_table*)vmm_GetVirtualAddress(PDP);       
    }
    
    PML4VirtualAddress->entries[indexer.PDP_i] = PDE;

    PDE = PDPVirtualAddress->entries[indexer.PD_i];

    void* PD;
    vmm_page_table* PDVirtualAddress;
    if (!vmm_GetFlag(&PDE, vmm_flag::vmm_Present)){
        return NULL;
    }else{
        PD = (void*)((uint64_t)vmm_GetAddress(&PDE) << 12);
        PDVirtualAddress = (vmm_page_table*)vmm_GetVirtualAddress(PD);     
    }

    PDPVirtualAddress->entries[indexer.PD_i] = PDE;

    PDE = PDVirtualAddress->entries[indexer.PT_i];

    void* PT;
    vmm_page_table* PTVirtualAddress;
    if (!vmm_GetFlag(&PDE, vmm_flag::vmm_Present)){
        return NULL;
    }else{
        PT = (void*)((uint64_t)vmm_GetAddress(&PDE) << 12);
        PTVirtualAddress = (vmm_page_table*)vmm_GetVirtualAddress(PT);    
    }

    PDE = PTVirtualAddress->entries[indexer.P_i];
    return (void*)((uint64_t)vmm_GetAddress(&PDE) * PAGE_SIZE + (uint64_t)Address % PAGE_SIZE);
}

void vmm_CopyPageTable(pagetable_t tableSource, pagetable_t tableDestination, uint64_t from, uint64_t to){
    vmm_page_table* PML4VirtualAddressDestination = (vmm_page_table*)vmm_GetVirtualAddress(tableDestination);
    vmm_page_table* PML4VirtualAddressSource = (vmm_page_table*)vmm_GetVirtualAddress(tableSource);
    for(int i = from; i < to; i++){
        PML4VirtualAddressDestination->entries[i] = PML4VirtualAddressSource->entries[i];
    }
}

void vmm_Fill(pagetable_t table, uint64_t from, uint64_t to, bool user){
    vmm_page_table* PML4VirtualAddress = (vmm_page_table*)vmm_GetVirtualAddress(table);
    for(int i = from; i < to; i++){
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

void vmm_Swap(pagetable_t table){
    ASMWriteCr3((uint64_t)table);
}

pagetable_t vmm_GetPageTable(){
    return (pagetable_t)ASMReadCr3();
}

uint64_t vmm_Init(BootInfo* bootInfo){
    vmm_PageTable = Pmm_RequestPage();
    memset(vmm_PageTable, 0, PAGE_SIZE);

    /* map pmrs */
    uint64_t HeapAddress = bootInfo->PMRs->pmrs[0].base;
    for(uint64_t i = 0; i < bootInfo->PMRs->entries; i++){
        stivale2_pmr* entry = &bootInfo->PMRs->pmrs[i];
        for(uint64_t y = 0; y < entry->length; y += PAGE_SIZE){
            uint64_t virtualAddress = entry->base + y;
            uint64_t physicalAddress;
            if(bootInfo->KernelAddress != NULL){
                physicalAddress = bootInfo->KernelAddress->physical_base_address + (entry->base - bootInfo->KernelAddress->virtual_base_address) + y;
            }else{
                physicalAddress = virtualAddress - bootInfo->HHDM->addr;
            }
            vmm_Map(vmm_PageTable, (void*)virtualAddress, (void*)physicalAddress, false, true, true);
        }
    }

    /* map all the memory */
    vmm_HHDMAdress = bootInfo->HHDM->addr;

    for (uint64_t i = 0; i < bootInfo->Memory->entries; i++){
        uint64_t size = bootInfo->Memory->memmap[i].length;
        for(uint64_t y = 0; y <= size; y += PAGE_SIZE){
            uint64_t physicalAddress = bootInfo->Memory->memmap[i].base + y;
            uint64_t virtualAddress = physicalAddress + vmm_HHDMAdress;
            vmm_Map(vmm_PageTable, (void*)virtualAddress, (void*)physicalAddress, false, true, bootInfo->Memory->memmap[i].type == STIVALE2_MMAP_USABLE);
        }
    }

    /* map framebuffer */
    uint64_t FramebufferSize = bootInfo->Framebuffer->framebuffer_height * bootInfo->Framebuffer->framebuffer_width * bootInfo->Framebuffer->framebuffer_bpp;
    bootInfo->Framebuffer->framebuffer_addr = vmm_GetVirtualAddress((bootInfo->Framebuffer->framebuffer_addr - vmm_HHDMAdress));
    for(uint64_t i = 0; i < FramebufferSize; i += PAGE_SIZE){
        vmm_Map(vmm_PageTable, (void*)(bootInfo->Framebuffer->framebuffer_addr + i), (void*)((bootInfo->Framebuffer->framebuffer_addr - vmm_HHDMAdress) + i), true);
    }

    /* map ramfs */
    bootInfo->ramfs.ramfsBase = (void*)vmm_GetVirtualAddress((bootInfo->ramfs.ramfsBase - vmm_HHDMAdress));
    for(uint64_t i = 0; i < bootInfo->ramfs.Size; i += PAGE_SIZE){
        vmm_Map(vmm_PageTable, (void*)(bootInfo->ramfs.ramfsBase + i), (void*)((bootInfo->ramfs.ramfsBase - vmm_HHDMAdress) + i), true, false); /* App can't write into ramfs */
    }

    vmm_Fill(vmm_PageTable, VMM_LOWERHALF, VMM_HIGHERALF, false);

    /* Update variable in the lower half */
    Pmm_PageBitmap.Buffer = (uint8_t*)vmm_GetVirtualAddress(Pmm_PageBitmap.Buffer);

    vmm_Swap(vmm_PageTable);

    return HeapAddress;
}

pagetable_t vmm_SetupProcess(){
    pagetable_t PageTable = Pmm_RequestPage();
    memset((void*)vmm_GetVirtualAddress(PageTable), 0, PAGE_SIZE);
    vmm_Fill(PageTable, VMM_STARTRHALF, VMM_LOWERHALF, true);
    vmm_CopyPageTable(vmm_PageTable, PageTable, VMM_LOWERHALF, VMM_HIGHERALF);
    
    uint64_t VirtualAddress = (uint64_t)vmm_GetVirtualAddress(PageTable);
    vmm_SetFlags(vmm_PageTable, (void*)VirtualAddress, vmm_flag::vmm_Custom0, true);
    vmm_SetFlags(vmm_PageTable, (void*)VirtualAddress, vmm_flag::vmm_PhysicalStorage, false);
    vmm_SetFlags(vmm_PageTable, (void*)VirtualAddress, vmm_flag::vmm_Custom2, true);
    return PageTable;   
}

pagetable_t vmm_SetupThread(pagetable_t parent){
    pagetable_t PageTable = Pmm_RequestPage();

    uint64_t VirtualAddress = (uint64_t)vmm_GetVirtualAddress(PageTable);
    memset((void*)VirtualAddress, 0, PAGE_SIZE);
    
    vmm_CopyPageTable(parent, PageTable, VMM_STARTRHALF, VMM_LOWERHALF);
    vmm_CopyPageTable(vmm_PageTable, PageTable, VMM_LOWERHALF, VMM_HIGHERALF);

    /* identify this address as paging entry */
    vmm_SetFlags(vmm_PageTable, (void*)VirtualAddress, vmm_flag::vmm_Custom0, true);
    vmm_SetFlags(vmm_PageTable, (void*)VirtualAddress, vmm_flag::vmm_PhysicalStorage, false);
    vmm_SetFlags(vmm_PageTable, (void*)VirtualAddress, vmm_flag::vmm_Custom2, true);
    return PageTable;      
}
