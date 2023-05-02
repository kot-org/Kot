QEMUFLAGS =	-no-reboot 														\
			-no-shutdown 													\
			-boot d 														\
			-M smm=off 														\
			-serial stdio 													\
			-machine q35 													\
			-cpu qemu64 													\
			-smp 8 															\
			-cdrom ./Bin/kot.iso											\
			-drive file=./Bin/kot.img										\
			-m 3G															\
			-rtc base=localtime												\
			-usb                                            				\
    		-device usb-ehci,id=ehci                        				\
			-device intel-hda -device hda-duplex 							\
			-device e1000,netdev=net0,romfile=Bin/Firmwares/efi-e1000.rom	\
			-netdev user,id=net0											\
			-object filter-dump,id=f1,netdev=net0,file=dump.dat				\
			-accel kvm

# -audiodev wav,id=snd0,path=output.wav 	 ,audiodev=snd0		

build:
	bash ./Build/build.sh 

# debug with your own qemu build : ../qemu/bin/debug/native/x86_64-softmmu/
run:
	~/Bureau/qemu/bin/debug/native/x86_64-softmmu/qemu-system-x86_64 $(QEMUFLAGS)

debug:
	qemu-system-x86_64 $(QEMUFLAGS) -s -S

deps-llvm:
	wget https://apt.llvm.org/llvm.sh
	chmod +x llvm.sh
	sudo ./llvm.sh 14 all
	rm -f llvm.sh

deps-debian: deps-llvm
	sudo apt update
	sudo apt install kpartx nasm xorriso mtools grub-common grub-efi-amd64 grub-pc-bin build-essential qemu-system-x86 ovmf meson kpartx  -y

clean:
	sudo rm -rf ./Bin ./Sysroot ./Sources/*/*/*/*/*/Lib ./Sources/*/*/*/*/Lib ./Sources/*/*/*/Lib ./Sources/*/*/Lib ./Sources/*/Lib

deps-github-action: deps-llvm
	sudo apt update
	sudo apt install kpartx nasm xorriso mtools qemu-utils

github-action: deps-github-action build
	qemu-img convert -f raw -O vmdk Bin/kot.img Bin/kot.vmdk
	qemu-img convert -f raw -O vdi Bin/kot.img Bin/kot.vdi


.PHONY: build run deps-llvm deps-debian
