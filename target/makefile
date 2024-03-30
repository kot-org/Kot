_END=$'\x1b[0m
_BOLD=$'\x1b[1m
_PURPLE=$'\x1b[35m

SOURCES = ../sources
KERNEL = $(SOURCES)/core/kernel/target
EXTERN = $(SOURCES)/extern

info_kernel = /bin/echo -e "$(_PURPLE)[core/kernel]$(_END) $(_BOLD)$1$(_END)"
info_system = /bin/echo -e "$(_PURPLE)[core/system]$(_END) $(_BOLD)$1$(_END)"

# Limine AMD64 (liamd)

liamd/build:
	@ cd build && bash liamd.sh rebuild  $(BOOT_DEVICE) $(PACKAGE) $(INSTALL)

liamd/configure:
	@ cd build && bash liamd.sh reconfigure  $(BOOT_DEVICE) $(PACKAGE) $(INSTALL)

liamd/update:
	@ cd build && bash liamd.sh update  $(BOOT_DEVICE) $(PACKAGE) $(INSTALL)
	
ovmf-amd64:
	@ make -C "$(EXTERN)" ovmf-amd64

LIAMD_QEMU_FLAGS = \
	-bios $(EXTERN)/ovmf-amd64/OVMF.fd \
	-drive file=liamd/boot-kot.img \
	-machine q35 \
	-m 2G \
	-smp cores=4 \
	-serial stdio \
	-device e1000e,netdev=net0 \
	-netdev user,id=net0,hostfwd=tcp::5000-:5000,hostfwd=udp::5000-:5000 \
	-object filter-dump,id=f1,netdev=net0,file=dump.bin	\
	-no-reboot \
	-no-shutdown

liamd/run: liamd/build ovmf-amd64
	sudo qemu-system-x86_64 $(LIAMD_QEMU_FLAGS) -enable-kvm

liamd/debug:
	sudo qemu-system-x86_64 $(LIAMD_QEMU_FLAGS) -S -s

# Limine AARCH64 (liarm)

liarm/build:
	@ rm -rf liarm/
	@ mkdir -m 777 -p liarm/sysroot
	@ mkdir -m 777 -p liarm/flash/limine
	@ $(call info_kernel,"Compiling...")
	@ make -C "$(KERNEL)/limine/aarch64" kernel.elf 
	@ cp $(KERNEL)/limine/aarch64/bin/kernel.elf liarm/flash/
	@ cp term_wallpaper.bmp liarm/flash/
	@ $(call info_kernel,"done.")
	@ make -C "$(EXTERN)" limine
	@ cp limine.cfg $(EXTERN)/limine/limine-bios.sys $(EXTERN)/limine/limine-bios-cd.bin $(EXTERN)/limine/limine-uefi-cd.bin liarm/flash/limine
	@ mkdir -p liarm/flash/EFI/BOOT
	@ cp $(EXTERN)/limine/BOOTAA64.EFI liarm/flash/EFI/BOOT/
	@ xorriso -as mkisofs -b limine/limine-bios-cd.bin \
		-no-emul-boot -boot-load-size 4 -boot-info-table \
		--efi-boot limine/limine-uefi-cd.bin \
		-efi-boot-part --efi-boot-image --protective-msdos-label \
		liarm/flash/ -o liarm/boot-kot.iso
	@ $(EXTERN)/limine/limine bios-install liarm/boot-kot.iso

ovmf-aarch64:
	@ make -C "$(EXTERN)" ovmf-aarch64

LIARM_QEMU_FLAGS = \
	-bios $(EXTERN)/ovmf-aarch64/OVMF.fd \
	-cdrom liarm/boot-kot.iso \
	-cpu cortex-a72 \
	-machine virt \
	-m 2G \
	-smp cores=1 \
	-serial stdio \
	-device ramfb \
	-no-reboot \
	-no-shutdown

liarm/run: liarm/build ovmf-aarch64
	qemu-system-aarch64 $(LIARM_QEMU_FLAGS)

liarm/debug: liarm/build ovmf-aarch64
	qemu-system-aarch64 $(LIARM_QEMU_FLAGS) -S -s

# OpenSBI RISC-V64 (opris)

opris/build:
	@ rm -rf opris/
	@ mkdir -m 777 -p opris/sysroot
	@ mkdir -m 777 -p liarm/flash/limine
	@ $(call info_kernel,"Compiling...")
	@ make -C "$(KERNEL)/opensbi/riscv64" kernel.elf 
	@ cp $(KERNEL)/opensbi/riscv64/bin/kernel.elf opris/flash/
	@ $(call info_kernel,"done.")
# todo

OPRIS_QEMU_FLAGS = 

opris/run: opris/build
	qemu-system-riscv64 $(OPRIS_QEMU_FLAGS)

opris/debug: opris/build
	qemu-system-riscv64 $(OPRIS_QEMU_FLAGS) -S -s
