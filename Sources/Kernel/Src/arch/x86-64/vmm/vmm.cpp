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

    if(!vmm_GetFlag(&PDE, vmm_flag::vmm_Present)){
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

    PDE = PDVirtualAddress->entries[indexer.PT_i];

    vmm_page_table* PT;
    vmm_page_table* PTVirtualAddress;
    if (!vmm_GetFlag(&PDE, vmm_flag::vmm_Present)){
        return false;
    }else{
        PT = (vmm_page_table*)(vmm_GetAddress(&PDE) << 12);
        PTVirtualAddress = (vmm_page_table*)vmm_GetVirtualAddress(PT);
    }

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

uint64_t vmm_Map(void* physicalAddress, size64_t size){
    uint64_t virtualAddress = vmm_GetVirtualAddress(physicalAddress);
    uint64_t pageSize = DivideRoundUp(size, PAGE_SIZE);
    for(uint64_t i = 0; i < pageSize; i++){
        vmm_Map(vmm_PageTable, (void*)(virtualAddress + i * PAGE_SIZE), (void*)((uint64_t)physicalAddress + i * PAGE_SIZE));
    }
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
    vmm_Map(table, Address, physicalAddress, user, readWrite, true);
}

void vmm_Map(pagetable_t table, void* Address, void* physicalAddress, bool user, bool readWrite, bool isPureMemory){
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
    vmm_SetFlag(&PDE, vmm_flag::vmm_Slave, false);
    vmm_SetFlag(&PDE, vmm_flag::vmm_Master, false);
    vmm_SetFlag(&PDE, vmm_flag::vmm_Present, true);
    vmm_SetFlag(&PDE, vmm_flag::vmm_ReadWrite, readWrite);
    vmm_SetFlag(&PDE, vmm_flag::vmm_IsPureMemory, isPureMemory);
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

    PTVirtualAddress->entries[indexer.P_i] = NULL;  

    if(table == ASMGetPagingEntry()){
        ASMFlushTLB(Address);
    }
}

void* vmm_GetPhysical(void* Address){
    return vmm_GetPhysical(vmm_PageTable, Address);
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
    for(uint16_t i = from; i < to; i++){
        PML4VirtualAddressDestination->entries[i] = PML4VirtualAddressSource->entries[i];
    }
}

void vmm_FlagPageTable(pagetable_t tableSource, uint64_t from, uint64_t to, vmm_flag flags, bool enabled){
    vmm_page_table* PML4VirtualAddressSource = (vmm_page_table*)vmm_GetVirtualAddress(tableSource);
    for(uint16_t i = from; i < to; i++){
        vmm_SetFlag(&PML4VirtualAddressSource->entries[i], flags, enabled);
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

void vmm_Swap(kthread_t* self, pagetable_t table){
    ASMWriteCr3((uint64_t)table);
    self->Regs->cr3 = (uint64_t)table;
}

pagetable_t vmm_GetPageTable(){
    return (pagetable_t)ASMReadCr3();
}

uint64_t vmm_Init(ukl_boot_structure_t* bootInfo){
    vmm_PageTable = (pagetable_t)bootInfo->memory_info.page_table;

    uint64_t HeapAddress = bootInfo->kernel_address.virtual_base_address;

    vmm_Fill(vmm_PageTable, VMM_LOWERHALF, VMM_HIGHERALF, false);

    uint64_t initrdphysicaladdress = bootInfo->initrd.base;
    uint64_t initrdvirtualaddress = vmm_GetVirtualAddress(((uint64_t)bootInfo->initrd.base));

    // Update INITRD address
    bootInfo->initrd.base = initrdvirtualaddress;

    // Map new INITRD address
    for(uint64_t i = 0; i < bootInfo->initrd.size; i += PAGE_SIZE){
        vmm_Map(vmm_PageTable, (void*)initrdvirtualaddress, (void*)initrdphysicaladdress, true, false); /* App can't write into initrd */
        initrdphysicaladdress += PAGE_SIZE;
        initrdvirtualaddress += PAGE_SIZE;
    }

    return HeapAddress;
}

pagetable_t vmm_SetupProcess(){
    pagetable_t PageTable = Pmm_RequestPage();
    memset((void*)vmm_GetVirtualAddress(PageTable), 0, PAGE_SIZE);
    vmm_Fill(PageTable, VMM_STARTRHALF, VMM_LOWERHALF, true);
    vmm_CopyPageTable(vmm_PageTable, PageTable, VMM_LOWERHALF, VMM_HIGHERALF);
    
    uint64_t VirtualAddress = (uint64_t)vmm_GetVirtualAddress(PageTable);
    vmm_SetFlags(vmm_PageTable, (void*)VirtualAddress, vmm_flag::vmm_Master, true);
    vmm_SetFlags(vmm_PageTable, (void*)VirtualAddress, vmm_flag::vmm_IsPureMemory, true);
    vmm_SetFlags(vmm_PageTable, (void*)VirtualAddress, vmm_flag::vmm_Slave, true);
    return PageTable;   
}

pagetable_t vmm_Setupthread(pagetable_t parent){
    pagetable_t PageTable = Pmm_RequestPage();

    uint64_t VirtualAddress = (uint64_t)vmm_GetVirtualAddress(PageTable);
    
    vmm_CopyPageTable(parent, PageTable, VMM_STARTRHALF, VMM_LOWERHALF);
    vmm_CopyPageTable(vmm_PageTable, PageTable, VMM_LOWERHALF, VMM_HIGHERALF);

    /* Clear thread data */
    vmm_page_table* PML4VirtualAddress = (vmm_page_table*)vmm_GetVirtualAddress(PageTable);
    void* Page = (void*)PML4VirtualAddress->entries[0xfe];
    PML4VirtualAddress->entries[0xfe] = NULL;
    Pmm_FreePage(Page);

    return PageTable;      
}


/* TODO */

struct ScanTable_t{
    uint16_t start;
    uint16_t end;
    uint16_t index;
    vmm_page_table* Table;
};

uint64_t vmm_CopyProcessMemory(pagetable_t dst, pagetable_t src){
    vmm_page_table* PML4VirtualAddressDestination = (vmm_page_table*)vmm_GetVirtualAddress(dst);
    vmm_page_table* PML4VirtualAddressSource = (vmm_page_table*)vmm_GetVirtualAddress(src);

    uint64_t memoryUsed = 0;

    ScanTable_t scanner[VMM_MAXLEVEL];
    scanner[0].start = VMM_STARTRHALF;
    scanner[0].end = VMM_LOWERHALF;
    scanner[0].index = scanner[0].start;
    scanner[0].Table = PML4VirtualAddressSource;

    for(uint8_t i = 1; i < VMM_MAXLEVEL; i++){
        scanner[i].start = VMM_STARTRHALF;
        scanner[i].end = VMM_HIGHERALF;
        scanner[i].index = scanner[i].start;
        scanner[i].Table = 0;
    }

    uint8_t index = 0;
    void* PDP;
    uint64_t PDE = PML4VirtualAddressSource->entries[scanner[0].start];
    while(scanner[0].index < scanner[0].end){
        while(scanner[index].index < scanner[index].end){
            vmm_page_table* PDPVirtualAddress;
            if(vmm_GetFlag(&PDE, vmm_flag::vmm_Present)){
                if((index + 1) == VMM_MAXLEVEL){
                    void* virtualAddress = (void*)vmm_MapAddress(scanner[index - 3].index, scanner[index - 2].index, scanner[index - 1].index, scanner[index].index);
                    void* physicalAddress = (void*)Pmm_RequestPage();
                    memoryUsed += PAGE_SIZE;
                    vmm_Map(dst, virtualAddress, physicalAddress, vmm_GetFlag(&PDE, vmm_flag::vmm_User), vmm_GetFlag(&PDE, vmm_flag::vmm_ReadWrite), vmm_GetFlag(&PDE, vmm_flag::vmm_IsPureMemory));
                    void* physicalAddressSrc = (void*)(vmm_GetAddress(&PDE) << 12);
                    void* virtualAddressSrc = (void*)vmm_GetVirtualAddress(physicalAddressSrc);
                    void* virtualAddressDst = (void*)vmm_GetVirtualAddress(vmm_GetPhysical(dst, virtualAddress));
                    memcpy(virtualAddressDst, virtualAddressSrc, PAGE_SIZE);
                    PDE = PDPVirtualAddress->entries[scanner[index].index++];
                }else{
                    index++;                    
                    PDP = (void*)((uint64_t)vmm_GetAddress(&PDE) << 12);
                    PDPVirtualAddress = (vmm_page_table*)vmm_GetVirtualAddress(PDP); 
                    scanner[index].Table = PDPVirtualAddress; 
                    scanner[index].index = scanner[index].start;               
                    PDE = scanner[index].Table->entries[scanner[index].index];
                }
            }else{
                scanner[index].index++;
                PDE = scanner[index].Table->entries[scanner[index].index];
            }
        }
        scanner[index].index = scanner[index].start;
        index--;
        scanner[index].index++;
        PDE = scanner[index].Table->entries[scanner[index].index];
    }

    return memoryUsed;
}


void vmm_ClearMemory(pagetable_t Src){

}

void vmm_CopyPageTable(vmm_page_table* Dst, vmm_page_table* Src, uint64_t Start, uint64_t End, uint8_t Level){
    for(uint16_t i = Start; i < End; i++){
        if(vmm_GetFlag(&Src->entries[i], vmm_flag::vmm_Present)){
            vmm_page_table* NextLevelSrc = (vmm_page_table*)vmm_GetVirtualAddress((void*)((uint64_t)vmm_GetAddress(&Src->entries[i]) << 12));    
            uint64_t PDE = Src->entries[i];
            if(Level){
                void* NextLevelDstPhysical = (void*)Pmm_RequestPage();
                vmm_page_table* NextLevelDst = (vmm_page_table*)vmm_GetVirtualAddress(NextLevelDstPhysical);  
                memset(NextLevelDst, 0, PAGE_SIZE);
                vmm_SetAddress(&PDE, (uint64_t)NextLevelDstPhysical >> 12);
                Dst->entries[i] = PDE;
                vmm_CopyPageTable(NextLevelDst, NextLevelSrc, VMM_STARTRHALF, VMM_HIGHERALF, Level - 1);
            }else{
                if(!vmm_GetFlag(&Src->entries[i], vmm_Master) || !vmm_GetFlag(&Src->entries[i], vmm_Slave)){
                    void* Physical = (void*)Pmm_RequestPage();
                    void* Buffer = (vmm_page_table*)vmm_GetVirtualAddress(Physical);  
                    memcpy(Buffer, (void*)vmm_GetVirtualAddress((void*)((uint64_t)vmm_GetAddress(&Src->entries[i]) << 12)), PAGE_SIZE);
                    vmm_SetAddress(&PDE, (uint64_t)Physical >> 12);
                }
                Dst->entries[i] = PDE;
            }
        }
    } 
}

void vmm_ForkMemory(kprocess_t* Parent, kprocess_t* Child){
    Child->SharedPaging = vmm_SetupProcess();
    vmm_CopyPageTable((vmm_page_table*)vmm_GetVirtualAddress(Child->SharedPaging), (vmm_page_table*)vmm_GetVirtualAddress(Parent->SharedPaging), VMM_STARTRHALF, VMM_LOWERHALF, VMM_MAXLEVEL - 1);
    // TODO : Make write copy
    // vmm_page_table* PML4VirtualAddressSource = (vmm_page_table*)vmm_GetVirtualAddress(Parent->SharedPaging);
    // for(uint16_t i = VMM_STARTRHALF; i < VMM_LOWERHALF; i++){
    //     vmm_page_table* PDPVirtualAddress = (vmm_page_table*)vmm_GetVirtualAddress((void*)((uint64_t)vmm_GetAddress(&PML4VirtualAddressSource->entries[i]) << 12));    
    //     if(vmm_GetFlag(&PML4VirtualAddressSource->entries[i], vmm_flag::vmm_Present)){
    //         for(uint16_t y = VMM_STARTRHALF; y < VMM_HIGHERALF; y++){
    //             uint64_t PDE = PDPVirtualAddress->entries[y];
    //             vmm_SetFlag(&PDE, vmm_flag::vmm_Master, true); // set flag master and slave to know that is a fork
    //             vmm_SetFlag(&PDE, vmm_flag::vmm_Slave, true);
    //             vmm_SetFlag(&PDE, vmm_flag::vmm_ReadWrite, false);
    //             PDPVirtualAddress->entries[y] = PDE;
    //         }
    //     }
    // }
    // Child->SharedPaging = Parent->SharedPaging;
}

bool vmm_MapFork(pagetable_t Table, uint64_t Address){
    // TODO : Make write copy
    return false;

    // if(Address % PAGE_SIZE){
    //     Address -= Address % PAGE_SIZE;
    // }

    // vmm_index Indexer = vmm_Index((uint64_t)Address);
    // uint64_t PDE;
    // int8_t LevelForkProtected = -1;
    // bool IsPureMemory = false;

    // vmm_page_table* PML4VirtualAddress = (vmm_page_table*)vmm_GetVirtualAddress(Table);
    // PDE = PML4VirtualAddress->entries[Indexer.PDP_i];
    // vmm_page_table* PDP;
    // vmm_page_table* PDPVirtualAddress;

    // if(!vmm_GetFlag(&PDE, vmm_flag::vmm_Present)){
    //     return false;
    // }else{
    //     PDP = (vmm_page_table*)(vmm_GetAddress(&PDE) << 12);
    //     PDPVirtualAddress = (vmm_page_table*)(vmm_GetVirtualAddress(PDP)); 
    // }

    // if(vmm_GetFlag(&PDE, vmm_Master) && vmm_GetFlag(&PDE, vmm_Slave)) LevelForkProtected = 3;

    // PDE = PDPVirtualAddress->entries[Indexer.PD_i];

    // vmm_page_table* PD;
    // vmm_page_table* PDVirtualAddress;
    // if(!vmm_GetFlag(&PDE, vmm_flag::vmm_Present)){
    //     return false;
    // }else{
    //     PD = (vmm_page_table*)(vmm_GetAddress(&PDE) << 12);
    //     PDVirtualAddress = (vmm_page_table*)vmm_GetVirtualAddress(PD);  
    // }

    // if(vmm_GetFlag(&PDE, vmm_Master) && vmm_GetFlag(&PDE, vmm_Slave)) LevelForkProtected = 2;

    // PDE = PDVirtualAddress->entries[Indexer.PT_i];

    // vmm_page_table* PT;
    // vmm_page_table* PTVirtualAddress;
    // if(!vmm_GetFlag(&PDE, vmm_flag::vmm_Present)){
    //     return false;
    // }else{
    //     PT = (vmm_page_table*)(vmm_GetAddress(&PDE) << 12);
    //     PTVirtualAddress = (vmm_page_table*)vmm_GetVirtualAddress(PT);
    // }

    // if(vmm_GetFlag(&PDE, vmm_Master) && vmm_GetFlag(&PDE, vmm_Slave)) LevelForkProtected = 1;

    // PDE = PTVirtualAddress->entries[Indexer.P_i];

    // if(!vmm_GetFlag(&PDE, vmm_flag::vmm_Present)){
    //     return false;
    // }

    // IsPureMemory = vmm_GetFlag(&PDE, vmm_IsPureMemory);

    // if(vmm_GetFlag(&PDE, vmm_Master) && vmm_GetFlag(&PDE, vmm_Slave)) LevelForkProtected = 0;

    // if(LevelForkProtected == -1) return false;

    // switch (LevelForkProtected){
    //     case 3:{
    //         PDE = PML4VirtualAddress->entries[Indexer.PDP_i];
    //         vmm_SetFlag(&PDE, vmm_Slave, false);
    //         vmm_SetFlag(&PDE, vmm_Master, false);
    //         vmm_SetFlag(&PDE, vmm_ReadWrite, true);
    //         void* NewPage = Pmm_RequestPage();
    //         memcpy((void*)vmm_GetVirtualAddress((uint64_t)NewPage), (void*)PDPVirtualAddress, PAGE_SIZE);
    //         vmm_SetAddress(&PDE, ((uint64_t)NewPage) >> 12);
    //         PML4VirtualAddress->entries[Indexer.PDP_i] = PDE;

    //         PDPVirtualAddress = (vmm_page_table*)vmm_GetVirtualAddress((uint64_t)NewPage);
    //         for(uint16_t i = VMM_STARTRHALF; i < VMM_LOWERHALF; i++){
    //             uint64_t PDEChild = PDPVirtualAddress->entries[i];
    //             vmm_SetFlag(&PDEChild, vmm_Slave, true);
    //             vmm_SetFlag(&PDEChild, vmm_Master, true);
    //             vmm_SetFlag(&PDEChild, vmm_ReadWrite, false);
    //             PDPVirtualAddress->entries[i] = PDEChild;
    //         }
    //     }
    //     case 2:{
    //         PDE = PDPVirtualAddress->entries[Indexer.PD_i];
    //         vmm_SetFlag(&PDE, vmm_Slave, false);
    //         vmm_SetFlag(&PDE, vmm_Master, false);
    //         vmm_SetFlag(&PDE, vmm_ReadWrite, true);
    //         void* NewPage = Pmm_RequestPage();
    //         memcpy((void*)vmm_GetVirtualAddress((uint64_t)NewPage), (void*)PDVirtualAddress, PAGE_SIZE);
    //         vmm_SetAddress(&PDE, ((uint64_t)NewPage) >> 12);
    //         PDPVirtualAddress->entries[Indexer.PD_i] = PDE;

    //         PDVirtualAddress = (vmm_page_table*)vmm_GetVirtualAddress((uint64_t)NewPage);
    //         for(uint16_t i = VMM_STARTRHALF; i < VMM_LOWERHALF; i++){
    //             uint64_t PDEChild = PDVirtualAddress->entries[i];
    //             vmm_SetFlag(&PDEChild, vmm_Slave, true);
    //             vmm_SetFlag(&PDEChild, vmm_Master, true);
    //             vmm_SetFlag(&PDEChild, vmm_ReadWrite, false);
    //             PDVirtualAddress->entries[i] = PDEChild;
    //         }
    //     }
    //     case 1:{
    //         PDE = PDVirtualAddress->entries[Indexer.PT_i];
    //         vmm_SetFlag(&PDE, vmm_Slave, false);
    //         vmm_SetFlag(&PDE, vmm_Master, false);
    //         vmm_SetFlag(&PDE, vmm_ReadWrite, true);
    //         void* NewPage = Pmm_RequestPage();
    //         memcpy((void*)vmm_GetVirtualAddress((uint64_t)NewPage), (void*)PTVirtualAddress, PAGE_SIZE);
    //         vmm_SetAddress(&PDE, ((uint64_t)NewPage) >> 12);
    //         PDVirtualAddress->entries[Indexer.PT_i] = PDE;

    //         PTVirtualAddress = (vmm_page_table*)vmm_GetVirtualAddress((uint64_t)NewPage);
    //         for(uint16_t i = VMM_STARTRHALF; i < VMM_LOWERHALF; i++){
    //             uint64_t PDEChild = PTVirtualAddress->entries[i];
    //             vmm_SetFlag(&PDEChild, vmm_Slave, true);
    //             vmm_SetFlag(&PDEChild, vmm_Master, true);
    //             vmm_SetFlag(&PDEChild, vmm_ReadWrite, false);
    //             PTVirtualAddress->entries[i] = PDEChild;
    //         }
    //     }
    //     case 0:{
    //         PDE = PTVirtualAddress->entries[Indexer.P_i];
    //         vmm_SetFlag(&PDE, vmm_Slave, false);
    //         vmm_SetFlag(&PDE, vmm_Master, false);
    //         vmm_SetFlag(&PDE, vmm_ReadWrite, true);
    //         void* NewPage = Pmm_RequestPage();
    //         memcpy((void*)vmm_GetVirtualAddress((uint64_t)NewPage), (void*)Address, PAGE_SIZE);
    //         vmm_SetAddress(&PDE, ((uint64_t)NewPage) >> 12);
    //         PTVirtualAddress->entries[Indexer.P_i] = PDE;
    //         return true;
    //     }
    // }
    // return false;
}