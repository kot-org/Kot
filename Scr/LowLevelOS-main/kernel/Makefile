OSNAME = LLOS

SRCDIR := src
OBJDIR := lib
BUILDDIR = bin
BOOTEFI := $(GNUEFI)/x86_64/bootloader/main.efi

GNUEFI = ../gnu-efi
OVMFDIR = ../OVMFbin
LDS = kern.ld
CC = gcc
LD = ld
ASM = nasm

CFLAGS = -fno-exceptions -ffreestanding -fshort-wchar -mno-red-zone -msse3
# uncomment for debugging
#CFLAGS += -g

LDFLAGS = -T $(LDS) -static -Bsymbolic -nostdlib
ASMFLAGS = 

rwildcard=$(foreach d,$(wildcard $(1:=/*)),$(call rwildcard,$d,$2) $(filter $(subst *,%,$2),$d))

SRC = $(call rwildcard,$(SRCDIR),*.cpp)     
ASMSRC = $(call rwildcard,$(SRCDIR),*.asm)
OBJS = $(patsubst $(SRCDIR)/%.cpp, $(OBJDIR)/%.o, $(SRC))
OBJS+= $(patsubst $(SRCDIR)/%.asm, $(OBJDIR)/%_nasm.o, $(ASMSRC))
DIRS = $(wildcard $(SRCDIR)/*)

kernel: $(OBJS) link

$(OBJDIR)/intrerupts/intrerupts.o: $(SRCDIR)/intrerupts/intrerupts.cpp
	@ tput setaf 3
	@ echo Compilez: $^
	@ tput sgr0
	@ mkdir -p $(@D)
	@ $(CC) -mno-red-zone -mgeneral-regs-only -ffreestanding -g -c $^ -o $@

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	@ tput setaf 3
	@ echo Compilez: $^
	@ tput sgr0
	@ mkdir -p $(@D)
	@ $(CC) $(CFLAGS) -c $^ -o $@
	
$(OBJDIR)/%_nasm.o: $(SRCDIR)/%.asm
	@ tput setaf 3
	@ echo Asamblez: $^
	@ tput sgr0
	@ mkdir -p $(@D)
	@ $(ASM) $(ASMFLAGS) $^ -f elf64 -o $@

link: $(OBJS)
	@ tput setaf 2
	@ echo Lincuiesc: $(OBJS)
	@ tput sgr0
	@ $(LD) $(LDFLAGS) -o $(BUILDDIR)/kernel.elf $(OBJS)
	
setup:
	@mkdir $(BUILDDIR)
	@mkdir $(SRCDIR)
	@mkdir $(OBJDIR)

buildimg: $(BUILDDIR)/kernel.elf
	@ tput setaf 14
	@ mv $(BUILDDIR)/kernel.elf $(BUILDDIR)/kernel.llexec

	@ dd if=/dev/zero of=$(BUILDDIR)/$(OSNAME).img bs=1024 count=32768
	@ mkfs.vfat $(BUILDDIR)/$(OSNAME).img
	@ mmd -i $(BUILDDIR)/$(OSNAME).img ::/EFI
	@ mmd -i $(BUILDDIR)/$(OSNAME).img ::/LLOS
	@ mmd -i $(BUILDDIR)/$(OSNAME).img ::/EFI/BOOT
	@ mcopy -i $(BUILDDIR)/$(OSNAME).img $(GNUEFI)/x86_64/bootloader/BOOTx64.efi ::/EFI/BOOT
	@ mcopy -i $(BUILDDIR)/$(OSNAME).img font.psf ::/LLOS
	@ mcopy -i $(BUILDDIR)/$(OSNAME).img $(BUILDDIR)/kernel.llexec ::/LLOS

	@ mkdir -p $(BUILDDIR)/iso
	@ mkdir -p $(BUILDDIR)/iso/LLOS
	@ mkdir -p $(BUILDDIR)/iso/EFI
	@ mkdir -p $(BUILDDIR)/iso/EFI/BOOT
	@ cp $(GNUEFI)/x86_64/bootloader/BOOTx64.efi $(BUILDDIR)/iso/EFI/BOOT
	@ cp font.psf $(BUILDDIR)/iso/LLOS/
	@ cp $(BUILDDIR)/kernel.llexec $(BUILDDIR)/iso/LLOS/
	@ cd $(BUILDDIR)
	@ xorrisofs -r -J -o $(BUILDDIR)/$(OSNAME).iso $(BUILDDIR)/iso
	@ tput sgr0
	
	@ rm -rf $(BUILDDIR)/iso