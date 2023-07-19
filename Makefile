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
	xbstrap install --all

	bash Tools/BuildTools.sh

	cp grub.cfg Bin/BootFiles/boot/grub/grub.cfg
	grub-mkrescue --xorriso=xorriso -o Bin/kot.iso Bin/BootFiles

# debug with your own qemu build : ../qemu/bin/debug/native/x86_64-softmmu/
run:
	qemu-system-x86_64 $(QEMUFLAGS)

debug:
	qemu-system-x86_64 $(QEMUFLAGS) -s -S

update-pkgs:
	sudo apt update
	sudo apt upgrade

install-ninja:
	sudo apt install wget
	sudo wget -qO /usr/local/bin/ninja.gz https://github.com/ninja-build/ninja/releases/latest/download/ninja-linux.zip
	sudo gunzip /usr/local/bin/ninja.gz
	sudo chmod a+x /usr/local/bin/ninja

install-llvm:
	wget https://apt.llvm.org/llvm.sh
	chmod +x llvm.sh
	sudo ./llvm.sh 14 all
	rm -f llvm.sh	

deps-debian: update-pkgs install-llvm install-ninja
	sudo apt install kpartx nasm xorriso mtools grub-common grub-efi-amd64 grub-pc-bin build-essential qemu-system-x86 ovmf meson kpartx python3 python3-pip python3-setuptools python3-wheel ninja-build cmake automake bison gperf texinfo apt-file parted lld pkgconf gettext -y
	sudo apt-file update 
	apt-file search autopoint
	sudo apt install autopoint
	sudo pip3 install meson
	sudo pip3 install xbstrap

init:
	xbstrap init .

tools:
	xbstrap install-tool --all

clean:
	sudo rm -rf ./Bin ./Sysroot ./pkg-builds ./packages ./Sources/*/*/*/*/*/Lib ./Sources/*/*/*/*/Lib ./Sources/*/*/*/Lib ./Sources/*/*/Lib ./Sources/*/Lib

github-action: deps-debian init build
	qemu-img convert -f raw -O vmdk Bin/kot.img Bin/kot.vmdk
	qemu-img convert -f raw -O vdi Bin/kot.img Bin/kot.vdi

.PHONY: build run install-llvm install-llvm-toolchain deps-debian
