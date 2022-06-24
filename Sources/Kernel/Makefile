PREFIX = ../../Toolchain/Local/bin/x86_64-kot-

LDS = kernel.ld
CC = $(PREFIX)gcc
LD = $(PREFIX)ld
ASMC = nasm

CFLAGS = -ffreestanding -fshort-wchar -g -mno-red-zone -fno-exceptions -fPIC -Isrc
ASMFLAGS = 
LDFLAGS = -T $(LDS) -static -Bsymbolic -nostdlib --no-relax 

SRCDIR := src
OBJDIR := lib
BUILDDIR = ../../Bin
LIMINEBUIDLDIR = ../../Bin/Limine

rwildcard=$(foreach d,$(wildcard $(1:=/*)),$(call rwildcard,$d,$2) $(filter $(subst *,%,$2),$d))

SRC = $(call rwildcard,$(SRCDIR),*.c)
SRCCPP = $(call rwildcard,$(SRCDIR),*.cpp)    
ASMSRC = $(call rwildcard,$(SRCDIR),*.asm)     
OBJS = $(patsubst $(SRCDIR)/%.c, $(OBJDIR)/%.o, $(SRC))
OBJS += $(patsubst $(SRCDIR)/%.cpp, $(OBJDIR)/%.o, $(SRCCPP))
OBJS += $(patsubst $(SRCDIR)/%.asm, $(OBJDIR)/%_asm.o, $(ASMSRC))
DIRS = $(wildcard $(SRCDIR)/*)

all: 

limine:
	git clone https://github.com/limine-bootloader/limine.git --branch=v3.0-branch-binary --depth=1
	make -C limine

kernel: $(OBJS) link creatISO

$(OBJDIR)/%.o: $(SRCDIR)/%.c
	@ echo !==== COMPILING $^
	@ mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $^ -o $@

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	@ echo !==== COMPILING $^
	@ mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $^ -o $@

$(OBJDIR)/%_asm.o: $(SRCDIR)/%.asm
	@ echo !==== COMPILING $^
	@ mkdir -p $(@D)
	$(ASMC) $(ASMFLAGS) $^ -f elf64 -o $@
	
link:
	@ echo !==== LINKING
	$(LD) $(LDFLAGS) -o $(BUILDDIR)/kernel.elf $(OBJS) $(OBJSCPP)

creatISO:
	@ echo !==== ISO
	mkdir -p $(LIMINEBUIDLDIR)
	cp -v $(BUILDDIR)/kernel.elf limine.cfg limine/limine.sys limine/limine-cd.bin limine/limine-cd-efi.bin $(LIMINEBUIDLDIR)
	xorriso -as mkisofs -b limine-cd.bin -no-emul-boot -boot-load-size 4 -boot-info-table \
        --efi-boot limine-cd-efi.bin -efi-boot-part --efi-boot-image --protective-msdos-label \
        $(LIMINEBUIDLDIR) -o $(BUILDDIR)/kot.iso
	./limine/limine-deploy $(BUILDDIR)/kot.iso

creatISOEFI:
	@ echo !==== ISO EFi
	mkdir -p  $(LIMINEBUIDLDIR)
	cp $(BUILDDIR)/kernel.elf limine/limine.sys limine/limine-cd-efi.bin  $(LIMINEBUIDLDIR)/
	xorriso -as mkisofs --efi-boot limine-cd-efi.bin -efi-boot-part --efi-boot-image -J -o $(BUILDDIR)/kotEFI.iso  $(LIMINEBUIDLDIR)
	./limine/limine-deploy $(BUILDDIR)/kotEFI.iso

ovmf:
	mkdir -p ../../Meta/ovmf
	-wget -nc https://retrage.github.io/edk2-nightly/bin/RELEASEX64_OVMF.fd -O ../../Meta/ovmf/ovmf.fd
