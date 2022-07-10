LDS = main.ld
CC = clang
ASMC = nasm

CFLAGS = -ffreestanding -fshort-wchar -fno-exceptions -fPIC -I../Scr
LINKERFLAGS = -lc -Wall -static
ASMFLAGS = 

SRCDIR := ../Scr
OBJDIR := ../Lib
BUILDDIR = ../Bin
BINDIR = ../../../../../Bin/Modules

rwildcard = $(foreach d,$(wildcard $(1:=/*)),$(call rwildcard,$d,$2) $(filter $(subst *,%,$2),$d))

SRCC = $(call rwildcard,$(SRCDIR),*.c)
ASMSRC = $(call rwildcard,$(SRCDIR),*.asm)   
OBJS += $(patsubst $(SRCDIR)/%.c, $(OBJDIR)/%.o, $(SRCC))
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
	$(CC) $(LINKERFLAGS) -o $(BINDIR)/Wm.elf $(OBJS) $(OBJSCPP)
	
copyToFS:
	../../../../../Tools/Buildramfs.sh
