_HOME = ../../../../../

CC = clang-14
ASMC = nasm

CFLAGS = --target=x86_64-unknown-none-elf -ffreestanding -fshort-wchar -fno-exceptions -fPIC -I../Src -I$(_HOME)Sysroot/include
LINKERFLAGS = -nostartfiles -Wall -static -L$(_HOME)Sysroot/lib/ -lc
ASMFLAGS = 

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

main: $(OBJS) link copyToFS

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
	$(CC) $(LINKERFLAGS) -o $(BINDIR)/UISD.elf $(OBJS) $(OBJSCPP) $(_HOME)Sysroot/lib/crt0.o
	
copyToFS:
	cd $(_HOME)Tools && bash ./BuildRamFS.sh
