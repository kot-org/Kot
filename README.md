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
