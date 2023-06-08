_HOME = ../../../../../../

CC = kot-x86_64-clang
ASMC = nasm

CFLAGS = -O3 -I../Src -fno-exceptions
LINKERFLAGS = -Wall -lc -lkot-c++
ASMFLAGS = 

SRCDIR := ../Src
OBJDIR := ../Lib
BUILDDIR = ../Bin
BINDIR = $(_HOME)Sources/Modules/Drivers/bus/ps2/Bin

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
	@ mkdir -p $(BINDIR)
	$(CC) $(LINKERFLAGS) -o $(BINDIR)/Ps2.elf $(OBJS) $(OBJSCPP)