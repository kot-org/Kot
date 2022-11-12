<p align="center">
	<img src="./Meta/Images/Logo/laptop.png?raw=true" width="90%"/> 
</p>

# KotOS
A 64-bit x86 operating system written in C/C++

<a href="https://discord.gg/XJzhM7uKkz">Discord server</a>

# Screenshots

<p align="center">
	<img src="./Meta/Screenshots/1.png?raw=true" width="90%"/> 
	<img src="./Meta/Screenshots/2.png?raw=true" width="90%"/> 
</p>

# Features
- Architecture specific :
	- x86-64 : 
		- PMM (physical memory manager)
		- VMM (virtrual memory manager)
		- GDT (global descriptor table)
		- IDT (interrupt descriptor table)
		- APIC (advanded programmable interrupt controller)
		- SMP (symetric multiprocessing)
		- SIMD (single instruction multiple data)
		- IO (input, ouput)
		- Atomic (lock system)
	- arm : 
		- WIP (Work in process)
- Architecture agnostic :
	- Kernel : 
		- Serial (console)
		- Multitask (process and thread system)
		- Syscall
	- System :
	 	- UISD (Universal interface for services and drivers, our interface system for drivers and services)
		- elf loading
		- pass boot informations (like framebuffer, current architecture, ...)
	- Drivers :
		- Bus :
			- PCI/PCIe
			- PS2 (mouse and keyboard)
		- Graphics :
			- Orb, windows manager
		- Filesystem : 
			- ext2 (read and write) | WIP
		- Net :
			- e1000 | WIP
		- Storage : 
			- AHCI (don't support optical disk)
		- Time :
			- HPET (high precision event timer)
			- RTC (real time clock)
	- Services :
		- HID : 
			- Mouse
			- Keyboard | WIP
		- Shell :
			- Input system | WIP
			- Output system | WIP
		- Storage : 
			- GPT partitions
			- Communicate with filesystem drivers
			- VFS | WIP
		- Time : 
			- Data centralization system for date and time
	- Libraries :
		- Libc :
			- Syscall functions
			- Miscellaneous
		- Libc++ :
			- Printf
			- Java
			- Json
			- Stack
		- Kot-graphics and kot-ui : 
			- For graphical apps

# Required Tools
- Build Essential
- Xorriso
- LLVM 14
- NASM
- Qemu (optional)

# Build it yourself
That's easy! Run `make deps-debian` to install all the required build tools listed before (only if you're building on Debian/Ubuntu) and then run `make build` to start the building process. Right after, `make run` will start KotOS in Qemu.

# Run Kot
Download the latest release <a href="https://github.com/kot-org/Kot/releases">here</a> and make it run on your x86-64 emulator such as qemu, bochs, vmware, virtual box, ...
