<p align="center">
	<img src="Images/Logo/icon.png" /> 
</p>

# KonectOS 2
An 64bits operating system in c++, which boot in uefi

# Road map

## Made :

* :heavy_check_mark: UEFI bootloader, made without gnu efi, with splash bmp (:house: home-made bmp parser) and elf loader
* :heavy_check_mark: Paging & heap (physical and virtual memory)
* :heavy_check_mark: Interrupts :
    * Exectpions (GP, PF, DF)
    * PIC, timer, keyboard, mouse
    * APIC timer
* :heavy_check_mark: PCI reader that lead to different drivers
* :heavy_check_mark: AHCI driver to read sata hard disk
* :heavy_check_mark: APIC, Symmetric Multiprocessing
* :heavy_check_mark: IO
* :heavy_check_mark: Userspace with syscalls
* :heavy_check_mark: Task handler with scheduler
* :heavy_check_mark: Get CPU info

## To do:

* :x: Our own FS, KFS (in progress :sweat_smile:)
* :x: ELF reader with realloc
* :x: Beautiful GUI


# Technical specifications:

* Kot is only debug for qemu and vbox
* To make syscall use int 0x80 (like linux)

