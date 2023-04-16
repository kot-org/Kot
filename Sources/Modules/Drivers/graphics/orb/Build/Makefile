_HOME = ../../../../../../

CC = clang-14
ASMC = nasm

CFLAGS = -O3 --target=x86_64-unknown-none-elf -fno-rtti -ffreestanding -fshort-wchar -fno-exceptions -fPIC -I../Src -I$(_HOME)Sysroot/include
LINKERFLAGS = -nostartfiles -Wall -static -L$(_HOME)Sysroot/lib/kot -lc
ASMFLAGS = 

SRCDIR := ../Src
OBJDIR := ../Lib
BINDIR = $(_HOME)Sources/Modules/Drivers/graphics/orb/Bin

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
	$(CC) $(LINKERFLAGS) -o $(BINDIR)/Orb.elf $(OBJS) $(OBJSCPP) $(_HOME)Sysroot/lib/kot/libcpp.a $(_HOME)Sysroot/lib/kot/uipp.a $(_HOME)Sysroot/lib/kot/audiopp.a $(_HOME)Sysroot/lib/kot/graphics.a $(_HOME)Sysroot/lib/kot/crt0.o
