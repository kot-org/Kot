_HOME = ../../../../../

CC = clang-14
ASMC = nasm

CFLAGS = -O3 --target=x86_64-unknown-none-elf -ffreestanding -fshort-wchar -fno-exceptions -fPIC -I../Src -I$(_HOME)Sysroot/Include
LINKERFLAGS = -nostartfiles -Wall -static -L$(_HOME)Sysroot/Lib/ -lc

SRCDIR := ../Src
OBJDIR := ../Lib
BUILDDIR = ../Bin
BINDIR = ../../../../../Bin/Modules

rwildcard = $(foreach d,$(wildcard $(1:=/*)),$(call rwildcard,$d,$2) $(filter $(subst *,%,$2),$d))

SRCCPP = $(call rwildcard,$(SRCDIR),*.cpp)      
ASMSRC = $(call rwildcard,$(SRCDIR),*.asm)   
OBJS += $(patsubst $(SRCDIR)/%.cpp, $(OBJDIR)/%.o, $(SRCCPP))
OBJS += $(patsubst $(SRCDIR)/%.asm, $(OBJDIR)/%_asm.o, $(ASMSRC))
DIRS = $(wildcard $(SRCDIR)/*)         

main: $(OBJS) link

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
	$(CC) $(LINKERFLAGS) -o $(BINDIR)/Vga.elf $(OBJS) $(OBJSCPP) $(_HOME)Sysroot/Lib/crt0.o