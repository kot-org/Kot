_HOME = ../../

LDS = ukl.ld
CC = clang-14
LD = ld.lld-14
ASMC = nasm

CFLAGS = -I$(_HOME)Sysroot/include -ISrc -m32 -fno-rtti -ffreestanding -fno-builtin -nostdlib -nostdinc -nostdinc++ -fshort-wchar -g -mno-red-zone -fno-exceptions -fPIC
ASMFLAGS = 
LDFLAGS = -T $(LDS) -static -Bsymbolic -nostdlib --no-relax -z max-page-size=0x1000

SRCDIR := Src
OBJDIR := Lib
BINDIR = $(_HOME)Bin
BUILDDIR = $(_HOME)Bin/BootFiles

rwildcard=$(foreach d,$(wildcard $(1:=/*)),$(call rwildcard,$d,$2) $(filter $(subst *,%,$2),$d))

SRCC = $(call rwildcard,$(SRCDIR),*.c)
SRCCPP = $(call rwildcard,$(SRCDIR),*.cpp)    
ASMSRC = $(call rwildcard,$(SRCDIR),*.asm)       
OBJS = $(patsubst $(SRCDIR)/%.c, $(OBJDIR)/%.o, $(SRCC))
OBJS += $(patsubst $(SRCDIR)/%.cpp, $(OBJDIR)/%.o, $(SRCCPP))
OBJS += $(patsubst $(SRCDIR)/%.asm, $(OBJDIR)/%_asm.o, $(ASMSRC))
DIRS = $(wildcard $(SRCDIR)/*)

all: ukl

ukl: $(OBJS) link createISO

$(OBJDIR)/%.o: $(SRCDIR)/%.c
	@ echo !==== COMPILING $^
	@ mkdir -m 777 -p $(@D)
	$(CC) $(CFLAGS) -c $^ -o $@

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	@ echo !==== COMPILING $^
	@ mkdir -m 777 -p $(@D)
	$(CC) $(CFLAGS) -c $^ -o $@

$(OBJDIR)/%_asm.o: $(SRCDIR)/%.asm
	@ echo !==== COMPILING $^
	@ mkdir -m 777 -p $(@D)
	$(ASMC) $(ASMFLAGS) $^ -f elf32 -o $@
	
link:
	@ echo !==== LINKING
	$(LD) $(LDFLAGS) -o $(BUILDDIR)/boot/ukl.elf $(OBJS) $(OBJSCPP)

createISO:
	@ echo !==== ISO

	bash $(_HOME)Tools/BuildTools.sh

	cp grub.cfg $(BUILDDIR)/boot/grub/grub.cfg
	grub-mkrescue --xorriso=xorriso -o $(BINDIR)/kot.iso $(BUILDDIR)