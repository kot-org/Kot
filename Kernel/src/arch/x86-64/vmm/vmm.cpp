#include <arch/x86-64/vmm/vmm.h>

pagetable_t vmm_PageTable;
uint64_t vmm_HHDMAdress = 0;

bool vmm_GetFlag(uint64_t* entry, uint8_t flag){
    return *entry & (1 << flag); 
}

void vmm_SetFlag(uint64_t* entry, uint8_t flag, bool enabled){
    uint64_t bitSelector = (uint64_t)(1 << flag);
    *entry &= ~bitSelector;
    if (enabled){
        *entry |= bitSelector;
    }
}

uint64_t vmm_GetAddress(uint64_t* entry){
    return (*entry & 0x000ffffffffff000) >> 12;
}

void vmm_SetAddress(uint64_t* entry, uint64_t address){
    address &= 0x000000ffffffffff;
    *entry &= 0xfff0000000000fff;
    *entry |= (address << 12);
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
        PDP = (vmm_page_table*)vmm_GetAddress(&PDE);
        PDPVirtualAddress = (vmm_page_table*)(vmm_GetVirtualAddress(PDP) << 12); 
    }
    
    PML4VirtualAddress->entries[indexer.PDP_i] = PDE;

    PDE = PDPVirtualAddress->entries[indexer.PD_i];

    vmm_page_table* PD;
    vmm_page_table* PDVirtualAddress;
    if (!vmm_GetFlag(&PDE, vmm_flag::vmm_Present)){
        return false;
    }else{
        PD = (vmm_page_table*)vmm_GetAddress(&PDE);
        PDVirtualAddress = (vmm_page_table*)(vmm_GetVirtualAddress(PD) << 12);  
    }

    PDPVirtualAddress->entries[indexer.PD_i] = PDE;

    PDE = PDVirtualAddress->entries[indexer.PT_i];

    vmm_page_table* PT;
    vmm_page_table* PTVirtualAddress;
    if (!vmm_GetFlag(&PDE, vmm_flag::vmm_Present)){
        return false;
    }else{
        PT = (vmm_page_table*)vmm_GetAddress(&PDE);
        PTVirtualAddress = (vmm_page_table*)(vmm_GetVirtualAddress(PT) << 12);
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
        PDP = (void*)vmm_GetAddress(&PDE);
        PDPVirtualAddress = (vmm_page_table*)(vmm_GetVirtualAddress(PDP) << 12); 
    }
    
    PML4VirtualAddress->entries[indexer.PDP_i] = PDE;

    PDE = PDPVirtualAddress->entries[indexer.PD_i];

    void* PD;
    vmm_page_table* PDVirtualAddress;
    if (!vmm_GetFlag(&PDE, vmm_flag::vmm_Present)){
        return;
    }else{
        PD = (void*)vmm_GetAddress(&PDE);
        PDVirtualAddress = (vmm_page_table*)(vmm_GetVirtualAddress(PD) << 12);  
    }

    PDPVirtualAddress->entries[indexer.PD_i] = PDE;

    PDE = PDVirtualAddress->entries[indexer.PT_i];

    void* PT;
    vmm_page_table* PTVirtualAddress;
    if (!vmm_GetFlag(&PDE, vmm_flag::vmm_Present)){
        return;
    }else{
        PT = (void*)vmm_GetAddress(&PDE);
        PTVirtualAddress = (vmm_page_table*)(vmm_GetVirtualAddress(PT) << 12);
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

void vmm_Map(pagetable_t table, void* Address, void* physicalAddress, bool user, bool vmm_ReadWrite){
    vmm_index indexer = vmm_Index((uint64_t)Address);
    uint64_t PDE;

    vmm_page_table* PML4VirtualAddress = (vmm_page_table*)vmm_GetVirtualAddress(table);
    PDE = PML4VirtualAddress->entries[indexer.PDP_i];
    void* PDP;
    vmm_page_table* PDPVirtualAddress;

    if(!vmm_GetFlag(&PDE, vmm_flag::vmm_Present)){
        PDP = (void*)globalAllocator.RequestPage();
        PDPVirtualAddress = (vmm_page_table*)vmm_GetVirtualAddress(PDP);  
        memset(PDPVirtualAddress, 0, 0x1000);
        vmm_SetAddress(&PDE, (uint64_t)PDP >> 12);
        vmm_SetFlag(&PDE, vmm_flag::vmm_Present, true);
        vmm_SetFlag(&PDE, vmm_flag::vmm_ReadWrite, true);
        vmm_SetFlag(&PDE, vmm_flag::vmm_User, user);
        PML4VirtualAddress->entries[indexer.PDP_i] = PDE;
    }else{
        PDP = (void*)((uint64_t)vmm_GetAddress(&PDE) << 12);
        PDPVirtualAddress = (vmm_page_table*)vmm_GetVirtualAddress(PDP);    
    }

    
    PDE = PDPVirtualAddress->entries[indexer.PD_i];

    void* PD;
    vmm_page_table* PDVirtualAddress;
    if(!vmm_GetFlag(&PDE, vmm_flag::vmm_Present)){
        PD = (void*)globalAllocator.RequestPage();
        PDVirtualAddress = (vmm_page_table*)vmm_GetVirtualAddress(PD); 
        memset(PDVirtualAddress, 0, 0x1000);
        vmm_SetAddress(&PDE, (uint64_t)PD >> 12);
        vmm_SetFlag(&PDE, vmm_flag::vmm_Present, true);
        vmm_SetFlag(&PDE, vmm_flag::vmm_ReadWrite, true);
        vmm_SetFlag(&PDE, vmm_flag::vmm_User, user);
        PDPVirtualAddress->entries[indexer.PD_i] = PDE;
    }else{
        PD = (void*)((uint64_t)vmm_GetAddress(&PDE) << 12);
        PDVirtualAddress = (vmm_page_table*)vmm_GetVirtualAddress(PD);  
    }


    PDE = PDVirtualAddress->entries[indexer.PT_i];
    void* PT;
    vmm_page_table* PTVirtualAddress;
    if(!vmm_GetFlag(&PDE, vmm_flag::vmm_Present)){
        PT = (void*)globalAllocator.RequestPage();
        PTVirtualAddress = (vmm_page_table*)vmm_GetVirtualAddress(PT);
        memset(PTVirtualAddress, 0, 0x1000);
        vmm_SetAddress(&PDE, (uint64_t)PT >> 12);
        vmm_SetFlag(&PDE, vmm_flag::vmm_Present, true);
        vmm_SetFlag(&PDE, vmm_flag::vmm_ReadWrite, true);
        vmm_SetFlag(&PDE, vmm_flag::vmm_User, user);
        PDVirtualAddress->entries[indexer.PT_i] = PDE;
    }else{
        PT = (void*)((uint64_t)vmm_GetAddress(&PDE) << 12);
        PTVirtualAddress = (vmm_page_table*)vmm_GetVirtualAddress(PT);  
    }


    PDE = PTVirtualAddress->entries[indexer.P_i];
    vmm_SetAddress(&PDE, (uint64_t)physicalAddress >> 12);
    vmm_SetFlag(&PDE, vmm_flag::vmm_Present, true);
    vmm_SetFlag(&PDE, vmm_flag::vmm_ReadWrite, true);
    vmm_SetFlag(&PDE, vmm_flag::vmm_User, user);
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
        PDP = (void*)vmm_GetAddress(&PDE);
        PDPVirtualAddress = (vmm_page_table*)(vmm_GetVirtualAddress(PDP) << 12); 
    }
    
    PML4VirtualAddress->entries[indexer.PDP_i] = PDE;

    PDE = PDPVirtualAddress->entries[indexer.PD_i];

    void* PD;
    vmm_page_table* PDVirtualAddress;
    if (!vmm_GetFlag(&PDE, vmm_flag::vmm_Present)){
        return;
    }else{
        PD = (void*)vmm_GetAddress(&PDE);
        PDVirtualAddress = (vmm_page_table*)(vmm_GetVirtualAddress(PD) << 12);  
    }

    PDPVirtualAddress->entries[indexer.PD_i] = PDE;

    PDE = PDVirtualAddress->entries[indexer.PT_i];

    void* PT;
    vmm_page_table* PTVirtualAddress;
    if (!vmm_GetFlag(&PDE, vmm_flag::vmm_Present)){
        return;
    }else{
        PT = (void*)vmm_GetAddress(&PDE);
        PTVirtualAddress = (vmm_page_table*)(vmm_GetVirtualAddress(PT) << 12);
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

    if (!vmm_GetFlag(&PDE, vmm_flag::vmm_Present)){
        return NULL;
    }else{
        PDP = (void*)vmm_GetAddress(&PDE);
        PDPVirtualAddress = (vmm_page_table*)(vmm_GetVirtualAddress(PDP) << 12); 
    }
    
    PML4VirtualAddress->entries[indexer.PDP_i] = PDE;

    PDE = PDPVirtualAddress->entries[indexer.PD_i];

    void* PD;
    vmm_page_table* PDVirtualAddress;
    if (!vmm_GetFlag(&PDE, vmm_flag::vmm_Present)){
        return NULL;
    }else{
        PD = (void*)vmm_GetAddress(&PDE);
        PDVirtualAddress = (vmm_page_table*)(vmm_GetVirtualAddress(PD) << 12);  
    }

    PDPVirtualAddress->entries[indexer.PD_i] = PDE;

    PDE = PDVirtualAddress->entries[indexer.PT_i];

    void* PT;
    vmm_page_table* PTVirtualAddress;
    if (!vmm_GetFlag(&PDE, vmm_flag::vmm_Present)){
        return NULL;
    }else{
        PT = (void*)vmm_GetAddress(&PDE);
        PTVirtualAddress = (vmm_page_table*)(vmm_GetVirtualAddress(PT) << 12);
    }

    PDE = PTVirtualAddress->entries[indexer.P_i];
    return (void*)((uint64_t)vmm_GetAddress(&PDE) * PAGE_SIZE + (uint64_t)Address % PAGE_SIZE);
}

void vmm_CopyPageTable(pagetable_t tableSource, pagetable_t tableDestination, uint64_t from, uint64_t to){
    vmm_page_table* PML4VirtualAddressDestination = (vmm_page_table*)vmm_GetVirtualAddress(tableDestination);
    vmm_page_table* PML4VirtualAddressSource = (vmm_page_table*)vmm_GetVirtualAddress(tableSource);
    for(int i = from; i < to; i++){
        uint64_t PDE = PML4VirtualAddressDestination->entries[i];
        vmm_page_table* PDP;
        vmm_page_table* PDPVirtualAddress;
        vmm_SetFlag(&PDE, vmm_flag::vmm_Present, true);
        vmm_SetFlag(&PDE, vmm_flag::vmm_ReadWrite, true);

        PML4VirtualAddressDestination->entries[i] = PML4VirtualAddressSource->entries[i];
    }
}

void vmm_Fill(pagetable_t table, uint64_t from, uint64_t to){
    vmm_page_table* PML4VirtualAddress = (vmm_page_table*)vmm_GetVirtualAddress(table);
    for(int i = from; i < to; i++){
        uint64_t PDE = PML4VirtualAddress->entries[i];
        vmm_page_table* PDP;
        vmm_page_table* PDPVirtualAddress;
        if(!vmm_GetFlag(&PDE, vmm_flag::vmm_Present)){
            PDP = (vmm_page_table*)globalAllocator.RequestPage();
            PDPVirtualAddress = (vmm_page_table*)vmm_GetVirtualAddress(PDP);
            memset(PDPVirtualAddress, 0, 0x1000);
            vmm_SetAddress(&PDE, (uint64_t)PDP >> 12);
            vmm_SetFlag(&PDE, vmm_flag::vmm_Present, true);
            vmm_SetFlag(&PDE, vmm_flag::vmm_ReadWrite, true);
            PDPVirtualAddress->entries[i] = PDE;
        }
    }
}

void vmm_Swap(pagetable_t table){
    ASMWriteCr3((uint64_t)table);
}

void vmm_Init(BootInfo* bootInfo){
    vmm_PageTable = globalAllocator.RequestPage();
    memset(vmm_PageTable, 0, PAGE);

    /* map pmrs */
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
            vmm_Map(vmm_PageTable, (void*)virtualAddress, (void*)physicalAddress);
        }
    }


    /* map all the memory */
    vmm_HHDMAdress = bootInfo->HHDM->addr;
    uint64_t CurentAddress = vmm_HHDMAdress;
    uint64_t memorySize = globalAllocator.GetMemorySize(bootInfo->Memory);
    for(uint64_t i = 0; i < memorySize; i += PAGE_SIZE){
        vmm_Map(vmm_PageTable, (void*)CurentAddress, (void*)i);
        CurentAddress += PAGE_SIZE;
    }

    vmm_Fill(vmm_PageTable, PMM_LOWERHALF, PMM_HIGHERALF);

    /* Update variable in the lower half */
    globalAllocator.PageBitmap.Buffer = (uint8_t*)vmm_GetVirtualAddress(globalAllocator.PageBitmap.Buffer);

    vmm_Swap(vmm_PageTable);
}

pagetable_t vmm_SetupProcess(){
    pagetable_t PageTable = globalAllocator.RequestPage();
    memset((void*)vmm_GetVirtualAddress(PageTable), 0, 0x1000);
    vmm_CopyPageTable(vmm_PageTable, PageTable, PMM_LOWERHALF, PMM_HIGHERALF);
    vmm_Fill(PageTable, PMM_STARTRHALF, PMM_LOWERHALF);
    return PageTable;   
}

pagetable_t vmm_SetupThread(pagetable_t parent){
    pagetable_t PageTable = globalAllocator.RequestPage();

    uint64_t VirtualAddress = (uint64_t)vmm_GetVirtualAddress(PageTable);
    memset((void*)VirtualAddress, 0, 0x1000);
    
    vmm_CopyPageTable(vmm_PageTable, PageTable, PMM_LOWERHALF, PMM_HIGHERALF);
    vmm_CopyPageTable(parent, PageTable, PMM_STARTRHALF, PMM_LOWERHALF);

    /* identify this address as paging entry */
    vmm_SetFlags(vmm_PageTable, (void*)VirtualAddress, vmm_flag::vmm_Custom0, true);
    vmm_SetFlags(vmm_PageTable, (void*)VirtualAddress, vmm_flag::vmm_Custom1, false);
    vmm_SetFlags(vmm_PageTable, (void*)VirtualAddress, vmm_flag::vmm_Custom2, true);
    return PageTable;      
}
