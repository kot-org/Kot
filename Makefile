QEMUFLAGS =	-no-reboot 				\
			-no-shutdown 			\
			-M smm=off 				\
			-serial stdio 			\
			-machine q35 			\
			-cpu qemu64 			\
			-smp 8 					\
			-cdrom ./Bin/kot.iso	\
			-m 3G					\
			-netdev user,id=n0 -device rtl8139,netdev=n0 \
			-m 3G					\
			-drive id=disk,file=Bin/kot.iso,if=none \
			-device ahci,id=ahci \
			-device ide-hd,drive=disk,bus=ahci.0

build:
	sudo bash ./Build/build.sh

run:
	sudo qemu-system-x86_64 $(QEMUFLAGS)

debug:
	qemu-system-x86_64 $(QEMUFLAGS) -s -S

deps-llvm:
	wget https://apt.llvm.org/llvm.sh
	chmod +x llvm.sh
	sudo ./llvm.sh 14 all
	rm -f llvm.sh

deps-debian: deps-llvm
	sudo apt update
	sudo apt install nasm xorriso build-essential qemu-system-x86 -y

clean:
	sudo rm -rf ./Bin ./Sysroot ./Sources/Kernel/Lib ./Sources/Libs/*/Lib ./Sources/Modules/Drivers/*/Lib ./Sources/Modules/Services/*/Lib ./Sources/System/Lib

deps-github-action: deps-llvm
	sudo apt update
	sudo apt install nasm xorriso

github-action: deps-github-action build

.PHONY: build run deps-llvm deps-debian
