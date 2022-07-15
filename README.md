<p align="center">
	<img src="/Meta/Images/Logo/laptop.png?raw=true" width="406px" height="281px" /> 
</p>

# KotOS
A 64-bit x86 operating system written in C/C++

<a href="https://discord.gg/XJzhM7uKkz">Discord server</a>

# Required Build Tools

- Build Essential
- Xorriso
- LLVM 14
- NASM
- Qemu (optional)

# Building the OS
That's easy! Run `make deps-debian` to install all the required build tools listed before (only if you're building on Debian/Ubuntu) and then run `make build` to start the build process. After that running `make run` will start KotOS in Qemu.
