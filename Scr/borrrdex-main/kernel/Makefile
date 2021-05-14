
OSNAME = borrrdex

GNUEFI = ../gnu-efi
OVMFDIR = ../OVMFbin
LDS = kernel.ld
CC = g++
ASMC = nasm
LD = ld

override CFLAGS := -I ../libc/include -I src -I src/arch/x86_64 -ffreestanding -fshort-wchar -fno-builtin -mno-red-zone -fno-rtti -fno-exceptions -Wno-address-of-packed-member -g $(CFLAGS)
ASMFLAGS = -g -F dwarf
LDFLAGS = -T $(LDS) -static -Bsymbolic -nostdlib
DEPFLAGS = -MM -MD -MP

SRCDIR := src
OBJDIR := lib
BUILDDIR = bin
BOOTEFI := $(GNUEFI)/x86_64/bootloader/BOOTX64.EFI

rwildcard=$(foreach d,$(wildcard $(1:=/*)),$(call rwildcard,$d,$2) $(filter $(subst *,%,$2),$d))

SRC = $(call rwildcard,$(SRCDIR),*.cpp)
ASMSRC = $(call rwildcard,$(SRCDIR),*.asm)
OBJS = $(patsubst $(SRCDIR)/%.cpp, $(OBJDIR)/%.o, $(SRC))
OBJS += $(patsubst $(SRCDIR)/%.asm, $(OBJDIR)/%_asm.o, $(ASMSRC))
DIRS = $(wildcard $(SRCDIR)/*)

-include $(OBJS:.o=.o.d)

kernel:	$(OBJS) link

.PHONY: grub

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	@ echo !== Compiling $<
	@ mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $< -o $@
	$(CC) $(CFLAGS) $< -MT $@ $(DEPFLAGS) -MF $@.d

$(OBJDIR)/%_asm.o: $(SRCDIR)/%.asm
	@ echo !== Assembling $^
	@ mkdir -p $(@D)
	$(ASMC) $(ASMFLAGS) $^ -f elf64 -o $@

link:
	@ echo !== Linking $^
	$(LD) $(LDFLAGS) -o $(BUILDDIR)/kernel.elf $(OBJS) $(BUILDDIR)/../../libc/bin/libc.a

setup:
	@mkdir -p $(BUILDDIR)
	@mkdir -p $(SRCDIR)
	@mkdir -p $(OBJDIR)

buildimg:
	@objcopy --only-keep-debug $(BUILDDIR)/kernel.elf $(BUILDDIR)/kernel.sym
	@strip --strip-debug --strip-unneeded $(BUILDDIR)/kernel.elf
	@objcopy --add-gnu-debuglink=$(BUILDDIR)/kernel.sym $(BUILDDIR)/kernel.elf
	@dd if=/dev/zero of=$(BUILDDIR)/$(OSNAME).img bs=512 count=93750
	@mformat -i $(BUILDDIR)/$(OSNAME).img -f 1440 ::
	@mmd -i $(BUILDDIR)/$(OSNAME).img ::/EFI
	@mmd -i $(BUILDDIR)/$(OSNAME).img ::/EFI/BOOT
	@mcopy -i $(BUILDDIR)/$(OSNAME).img $(BOOTEFI) ::/EFI/BOOT
	@mcopy -i $(BUILDDIR)/$(OSNAME).img $(BUILDDIR)/kernel.elf ::
	@mcopy -i $(BUILDDIR)/$(OSNAME).img $(BUILDDIR)/zap-light16.psf ::
	@# mkdir -p $(BUILDDIR)/iso/EFI/BOOT
	@# cp $(BOOTEFI) $(BUILDDIR)/iso/EFI/BOOT
	@# cp startup.nsh $(BUILDDIR)/iso/
	@# cp $(BUILDDIR)/zap-light16.psf $(BUILDDIR)/iso/
	@# cp $(BUILDDIR)/kernel.elf $(BUILDDIR)/iso/
	@# cd $(BUILDDIR)
	@# xorrisofs -r -J -o $(BUILDDIR)/$(OSNAME).iso $(BUILDDIR)/iso

run:
	qemu-system-x86_64 -drive file=$(BUILDDIR)/$(OSNAME).img -m 256M -cpu qemu64 -drive if=pflash,format=raw,unit=0,file="$(OVMFDIR)/OVMF_CODE-pure-efi.fd",readonly=on -drive if=pflash,format=raw,unit=1,file="$(OVMFDIR)/OVMF_VARS-pure-efi.fd" -net none

grub:
	cp $(BUILDDIR)/kernel.elf grub/iso/boot/
	grub-mkrescue -o $(BUILDDIR)/borrrdex.iso grub/iso/

clean:
	rm -f $(BUILDDIR)/kernel.*
	rm -f $(BUILDDIR)/$(OSNAME).img
	rm -rf $(OBJDIR)