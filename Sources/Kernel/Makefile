_HOME = ../../

LDS = kernel.ld
CC = clang-14
LD = ld.lld-14
ASMC = nasm

CFLAGS = -I$(_HOME)Sysroot/Include/ -ISrc --target=x86_64-unknown-none-elf -ffreestanding -fno-builtin -nostdlib -nostdinc -nostdinc++ -fshort-wchar -g -mno-red-zone -fno-exceptions -fPIC
ASMFLAGS = 
LDFLAGS = -T $(LDS) -static -Bsymbolic -nostdlib --no-relax

SRCDIR := Src
OBJDIR := Lib
BUILDDIR = $(_HOME)Bin
LIMINEBUIDLDIR = $(_HOME)Bin/Limine

rwildcard=$(foreach d,$(wildcard $(1:=/*)),$(call rwildcard,$d,$2) $(filter $(subst *,%,$2),$d))

SRCC = $(call rwildcard,$(SRCDIR),*.c)
SRCCPP = $(call rwildcard,$(SRCDIR),*.cpp)    
ASMSRC = $(call rwildcard,$(SRCDIR),*.asm)     
OBJS = $(patsubst $(SRCDIR)/%.c, $(OBJDIR)/%.o, $(SRCC))
OBJS += $(patsubst $(SRCDIR)/%.cpp, $(OBJDIR)/%.o, $(SRCCPP))
OBJS += $(patsubst $(SRCDIR)/%.asm, $(OBJDIR)/%_asm.o, $(ASMSRC))
DIRS = $(wildcard $(SRCDIR)/*)

all: limine kernel

limine:
	git clone https://github.com/limine-bootloader/limine.git --branch=v3.0-branch-binary --depth=1
	make -C limine

kernel: $(OBJS) link createISO

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
	$(LD) $(LDFLAGS) -o $(BUILDDIR)/Kernel.elf $(OBJS) $(OBJSCPP)
	sudo cp -v $(BUILDDIR)/Kernel.elf $(LIMINEBUIDLDIR)

createISO:
	@ echo !==== ISO
	echo $(shell pwd)
	xorriso -as mkisofs -b limine-cd.bin -no-emul-boot -boot-load-size 4 -boot-info-table \
        --efi-boot limine-cd-efi.bin -efi-boot-part --efi-boot-image --protective-msdos-label \
        $(LIMINEBUIDLDIR) -o $(BUILDDIR)/kot.iso
	./limine/limine-deploy $(BUILDDIR)/kot.iso

createISOEFI:
	@ echo !==== ISO EFI
	xorriso -as mkisofs --efi-boot limine-cd-efi.bin -efi-boot-part --efi-boot-image -J -o $(BUILDDIR)/kotEFI.iso  $(LIMINEBUIDLDIR)
	./limine/limine-deploy $(BUILDDIR)/kotEFI.iso