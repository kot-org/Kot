<p align="center">
	<img src="/Meta/Images/Logo/371D3AB5-4CFA-46D3-9C26-DC4081C0F03A.png?raw=true" width="406px" height="281px" /> 
</p>

# KotOS
A 64-bit x86 operating system

# Road map

## Features :

* :heavy_check_mark: UEFI bootloader, with splash bitmap (:house: home-made bitmap parser) and elf loader
* :heavy_check_mark: Paging & heap (physical and virtual memory)
* :heavy_check_mark: Interrupts :
    * Exceptions 
    * IO APIC, keyboard, mouse
    * APIC
* :heavy_check_mark: PCI
* :heavy_check_mark: AHCI driver to read SATA hard disks
* :heavy_check_mark: Symmetric Multiprocessing (SMP)
* :heavy_check_mark: I/O
* :heavy_check_mark: Userspace with syscalls
* :heavy_check_mark: Task handler with scheduler
* :heavy_check_mark: Get CPU info
* :heavy_check_mark: File system : KFS
* :heavy_check_mark: ELF reader

## To do:

* :x: Beautiful GUI


# Documentation:

* To issue a syscall, use the syscall instruction 

