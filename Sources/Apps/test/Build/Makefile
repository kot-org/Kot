_HOME = ../../../../

CC = $(_HOME)/Toolchain/local/clang --target=x86_64-kot --sysroot=$(_HOME)Sysroot
ASMC = nasm

CFLAGS = -O3 -fno-rtti -ffreestanding -fshort-wchar -fno-exceptions -fPIC -no-pie -I../Src
LINKERFLAGS = -Wall -lc
ASMFLAGS = 

SRCDIR := ../Src
OBJDIR := ../Lib
BINDIR = $(_HOME)Sources/Apps/test/Bin

rwildcard = $(foreach d,$(wildcard $(1:=/*)),$(call rwildcard,$d,$2) $(filter $(subst *,%,$2),$d))

SRCC = $(call rwildcard,$(SRCDIR),*.cpp)
ASMSRC = $(call rwildcard,$(SRCDIR),*.asm)   
OBJS += $(patsubst $(SRCDIR)/%.cpp, $(OBJDIR)/%.o, $(SRCC))
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
	@ mkdir -p $(BINDIR)
	$(CC) $(LINKERFLAGS) -o $(BINDIR)/Test.elf $(OBJS) $(OBJSCPP)
