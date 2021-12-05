<p align="center">
	<img src="Images/Logo/icon.png" /> 
</p>

# KonectOS 2
A 64-bit x86 operating system written in C++, which boots from UEFI

# Road map

## Features :

* :heavy_check_mark: UEFI bootloader, with splash bitmap (:house: home-made bitmap parser) and elf loader
* :heavy_check_mark: Paging & heap (physical and virtual memory)
* :heavy_check_mark: Interrupts :
    * Exceptions 
    * PIC, keyboard, mouse
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

* To issue a syscall, use the intrerrupt 0x80 (in assembly int 80h or int 0x80)

