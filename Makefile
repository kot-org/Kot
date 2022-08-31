QEMUFLAGS =	-no-reboot 										\
			-no-shutdown 									\
			-M smm=off 										\
			-serial stdio 									\
			-machine q35 									\
			-cpu qemu64 									\
			-smp 8 											\
			-cdrom ./Bin/kot.iso							\
			-drive file=./Bin/hdd.img						\
			-m 3G											\
			-netdev user,id=n0 -device rtl8139,netdev=n0 	\
			-usb                                            \
    		-device usb-ehci,id=ehci                        \
			-soundhw pcspk

build:
	bash ./Build/build.sh 

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
	sudo rm -rf ./Bin ./Sysroot ./Sources/*/*/*/*/*/Lib ./Sources/*/*/*/*/Lib ./Sources/*/*/*/Lib ./Sources/*/*/Lib ./Sources/*/Lib

deps-github-action: deps-llvm
	sudo apt update
	sudo apt install nasm xorriso

github-action: deps-github-action build

.PHONY: build run deps-llvm deps-debian
