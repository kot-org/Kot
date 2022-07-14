_HOME = ../../../../../

CC = clang-14
ASMC = nasm

CFLAGS = --target=x86_64-unknown-none-elf -ffreestanding -fshort-wchar -fno-exceptions -fPIC -I../Src -I$(_HOME)Sysroot/Include
LINKERFLAGS = -nostartfiles -Wall -static -L$(_HOME)Sysroot/Lib/ -lc
ASMFLAGS = 

SRCDIR := ../Src
OBJDIR := ../Lib
BUILDDIR = ../Bin
BINDIR = $(_HOME)Bin/Modules

rwildcard = $(foreach d,$(wildcard $(1:=/*)),$(call rwildcard,$d,$2) $(filter $(subst *,%,$2),$d))

SRCC = $(call rwildcard,$(SRCDIR),*.c)
ASMSRC = $(call rwildcard,$(SRCDIR),*.asm)   
OBJS += $(patsubst $(SRCDIR)/%.c, $(OBJDIR)/%.o, $(SRCC))
OBJS += $(patsubst $(SRCDIR)/%.asm, $(OBJDIR)/%_asm.o, $(ASMSRC))
DIRS = $(wildcard $(SRCDIR)/*)         

main: $(OBJS) link copyToFS

$(OBJDIR)/%.o: $(SRCDIR)/%.c
	@ echo !==== COMPILING $^
	@ mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $^ -o $@

$(OBJDIR)/%_asm.o: $(SRCDIR)/%.asm
	@ echo !==== COMPILING $^
	@ mkdir -p $(@D)
	$(ASMC) $(ASMFLAGS) $^ -f elf64 -o $@

link:
	@ echo !==== LINKING
	$(CC) $(LINKERFLAGS) -o $(BINDIR)/Wm.elf $(OBJS) $(OBJSCPP) $(_HOME)Sysroot/Lib/crt0.o
	
copyToFS:
	cd $(_HOME)Tools && bash ./BuildRamFS.sh
