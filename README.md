<p align="center">
	<img src="/Meta/Images/Logo/laptop.png?raw=true" width="406px" height="281px" /> 
</p>

# KotOS
A 64-bit x86 operating system written in C/C++

<a href="https://discord.gg/XJzhM7uKkz">Discord server</a>

# Required Tools

- Build Essential
- Xorriso
- LLVM 14
- NASM
- Qemu (optional)

# Build Sources
That's easy! Run `make deps-debian` to install all required build tools listed before (only if you're building on Debian/Ubuntu) and then run `make build` to start build processes. After that running `make run` will start KotOS in Qemu.

# Run Kot
Download the latest release <a href="https://github.com/kot-org/Kot/releases">right here</a> and make it runs on your x86-64 emulator such as qemu-kvm, bochs, vmware, virtual box, ...
